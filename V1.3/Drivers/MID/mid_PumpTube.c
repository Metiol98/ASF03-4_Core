#include "mid_PumpTube.h"
#include <stddef.h>
#include <string.h>

#define PUMPTUBE_STORE_MAGIC    0x50545542UL
#define PUMPTUBE_STORE_VERSION  2U
#define PUMPTUBE_SCREEN_ID     11U
#define PUMPTUBE_ALERT_SCREEN  12U
#define PUMPTUBE_REGISTER_SCREEN 13U
#define PUMPTUBE_BOOT_ALERT_DELAY_MS  2000UL
#define PUMPTUBE_MAX_CYCLE 120U
#define PUMPTUBE_DEFAULT_CYCLE 3U
#define PUMPTUBE_DUE_DATE 0x01U
#define PUMPTUBE_DUE_TURNS 0x02U
#define PUMPTUBE_STORE_PAGE_COUNT (FLASH_TOTAL_PAGES - PARAM_FLASH_PUMPTUBE_NUM)

typedef struct
{
	uint32_t magic;
	uint16_t version;
	uint16_t reserved;
	uint32_t sequence;
	PumpTubeData data;
	uint32_t checksum;
} PumpTubeStore;

static PumpTubeData s_data;
static uint8_t s_motor_tracking[PUMPTUBE_CHANNEL_COUNT];
static uint8_t s_position_valid[PUMPTUBE_CHANNEL_COUNT];
static uint32_t s_last_position[PUMPTUBE_CHANNEL_COUNT];
static uint8_t s_presented_mask;
static uint32_t s_boot_alert_tick;
static uint8_t s_save_pending;
static uint8_t s_save_retry_limited;
static uint32_t s_last_save_attempt;
static uint32_t s_store_sequence;
static uint8_t s_last_icon_mask = 0xFFU;
static int8_t s_last_enabled = -1;
static uint32_t s_current_date;
static uint32_t s_rtc_request_at;
static uint8_t s_rtc_requested;
static uint8_t s_rtc_seen;
static uint8_t s_registration_pump;
static TFTDateTime s_registration_date;

static uint32_t PumpTube_Checksum(const uint8_t *data, uint32_t length)
{
	uint32_t hash = 2166136261UL;
	for (uint32_t i = 0; i < length; i++)
	{
		hash ^= data[i];
		hash *= 16777619UL;
	}
	return hash;
}

static uint8_t PumpTube_StoreValid(const PumpTubeStore *store)
{
	return (store->magic == PUMPTUBE_STORE_MAGIC) &&
	       ((store->version == 1U) || (store->version == PUMPTUBE_STORE_VERSION)) &&
	       (store->checksum == PumpTube_Checksum((const uint8_t *)store,
	                                            offsetof(PumpTubeStore, checksum)));
}

static uint8_t PumpTube_IsErased(uint32_t address)
{
	const uint8_t *bytes = (const uint8_t *)address;
	for (uint32_t i = 0; i < sizeof(PumpTubeStore); i++)
	{
		if (bytes[i] != 0xFFU)
			return 0U;
	}
	return 1U;
}

static uint8_t PumpTube_FindLatest(PumpTubeStore *latest, uint32_t *latest_address)
{
	uint8_t found = 0U;
	for (uint32_t page = 0; page < PUMPTUBE_STORE_PAGE_COUNT; page++)
	{
		uint32_t page_address = FLASH_BASE_ADDR +
		                        (PARAM_FLASH_PUMPTUBE_NUM + page) * FLASH_PAGE_SIZE;
		for (uint32_t offset = 0; offset + sizeof(PumpTubeStore) <= FLASH_PAGE_SIZE;
		     offset += sizeof(PumpTubeStore))
		{
			const PumpTubeStore *candidate = (const PumpTubeStore *)(page_address + offset);
			if (!PumpTube_StoreValid(candidate))
				continue;
			if ((found == 0U) || ((int32_t)(candidate->sequence - latest->sequence) > 0))
			{
				*latest = *candidate;
				*latest_address = page_address + offset;
				found = 1U;
			}
		}
	}
	return found;
}

static HAL_StatusTypeDef PumpTube_AppendStore(PumpTubeStore *store)
{
	PumpTubeStore latest;
	uint32_t latest_address = 0U;
	uint32_t write_address = 0U;
	uint32_t page_address;
	uint32_t page_index;
	uint8_t latest_found = PumpTube_FindLatest(&latest, &latest_address);

	if (latest_found != 0U)
	{
		page_index = (latest_address - FLASH_BASE_ADDR) / FLASH_PAGE_SIZE - PARAM_FLASH_PUMPTUBE_NUM;
		page_address = FLASH_BASE_ADDR +
		               (PARAM_FLASH_PUMPTUBE_NUM + page_index) * FLASH_PAGE_SIZE;
		uint32_t latest_offset = latest_address - page_address;
		for (uint32_t offset = latest_offset + sizeof(PumpTubeStore);
		     offset + sizeof(PumpTubeStore) <= FLASH_PAGE_SIZE;
		     offset += sizeof(PumpTubeStore))
		{
			if (PumpTube_IsErased(page_address + offset) != 0U)
			{
				write_address = page_address + offset;
				break;
			}
		}
		if (write_address == 0U)
		{
			page_index = (page_index + 1U) % PUMPTUBE_STORE_PAGE_COUNT;
			page_address = FLASH_BASE_ADDR +
			               (PARAM_FLASH_PUMPTUBE_NUM + page_index) * FLASH_PAGE_SIZE;
			if (FLASH_Param_EraseWholePage(page_address) != HAL_OK)
				return HAL_ERROR;
			write_address = page_address;
		}
	}
	else
	{
		page_address = FLASH_BASE_ADDR + PARAM_FLASH_PUMPTUBE_NUM * FLASH_PAGE_SIZE;
		if (PumpTube_IsErased(page_address) == 0U &&
		    FLASH_Param_EraseWholePage(page_address) != HAL_OK)
			return HAL_ERROR;
		write_address = page_address;
	}

	if (HAL_FLASH_Unlock() != HAL_OK)
		return HAL_ERROR;
	HAL_StatusTypeDef status = HAL_OK;
	const uint8_t *bytes = (const uint8_t *)store;
	for (uint32_t offset = 0; offset < sizeof(PumpTubeStore); offset += 2U)
	{
		uint16_t halfword = bytes[offset];
		if ((offset + 1U) < sizeof(PumpTubeStore))
			halfword |= (uint16_t)(bytes[offset + 1U] << 8);
		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
		                           write_address + offset, halfword);
		if (status != HAL_OK)
			break;
	}
	HAL_FLASH_Lock();
	if ((status == HAL_OK) && !PumpTube_StoreValid((const PumpTubeStore *)write_address))
		status = HAL_ERROR;
	return status;
}

static uint32_t PumpTube_EnterCritical(void)
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

static void PumpTube_ExitCritical(uint32_t primask)
{
	if (primask == 0U)
		__enable_irq();
}

static void PumpTube_RequestSave(void)
{
	s_save_pending = 1U;
	s_save_retry_limited = 0U;
}

HAL_StatusTypeDef PumpTube_SaveNow(void)
{
	PumpTubeStore store;
	uint8_t tracking_active = 0U;
	uint32_t primask;

	memset(&store, 0, sizeof(store));
	store.magic = PUMPTUBE_STORE_MAGIC;
	store.version = PUMPTUBE_STORE_VERSION;
	store.sequence = s_store_sequence + 1U;
	primask = PumpTube_EnterCritical();
	store.data = s_data;
	for (uint8_t i = 0; i < PUMPTUBE_CHANNEL_COUNT; i++)
		tracking_active |= s_motor_tracking[i];
	PumpTube_ExitCritical(primask);
	store.checksum = PumpTube_Checksum((const uint8_t *)&store,
	                                  offsetof(PumpTubeStore, checksum));

	s_last_save_attempt = HAL_GetTick();
	if (PumpTube_AppendStore(&store) != HAL_OK)
	{
		s_save_pending = 1U;
		s_save_retry_limited = 1U;
		return HAL_ERROR;
	}

	s_store_sequence = store.sequence;
	s_save_pending = tracking_active;
	s_save_retry_limited = 0U;
	return HAL_OK;
}

static uint8_t PumpTube_DaysInMonth(unsigned year, unsigned month)
{
	static const uint8_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if ((month == 0U) || (month > 12U))
		return 0U;
	if ((month == 2U) && (year % 4U == 0U) &&
	    ((year % 100U != 0U) || (year % 400U == 0U)))
		return 29U;
	return days[month - 1U];
}

/* Ordered year/month/day/hour/minute in a 32-bit Flash slot. Zero means no date. */
static uint32_t PumpTube_PackDate(TFTDateTime date)
{
	return ((uint32_t)date.year << 20) | ((uint32_t)date.month << 16) |
	       ((uint32_t)date.day << 11) | ((uint32_t)date.hour << 6) | date.minute;
}

static TFTDateTime PumpTube_UnpackDate(uint32_t value)
{
	TFTDateTime date = {
		(uint8_t)(value >> 20), (uint8_t)((value >> 16) & 15U),
		(uint8_t)((value >> 11) & 31U), (uint8_t)((value >> 6) & 31U),
		(uint8_t)(value & 63U), 0U
	};
	return date;
}

static uint8_t PumpTube_DateValid(TFTDateTime date)
{
	return (date.year >= 20U) && (date.year <= 99U) &&
	       (date.day != 0U) && (date.day <= PumpTube_DaysInMonth(2000U + date.year, date.month)) &&
	       (date.hour < 24U) && (date.minute < 60U) && (date.second < 60U);
}

static uint32_t PumpTube_Deadline(uint32_t replaced_at, uint32_t months)
{
	TFTDateTime date = PumpTube_UnpackDate(replaced_at);
	uint32_t month_index = (uint32_t)date.year * 12U + date.month - 1U + months;
	date.year = (uint8_t)(month_index / 12U);
	date.month = (uint8_t)(month_index % 12U + 1U);
	uint8_t days = PumpTube_DaysInMonth(2000U + date.year, date.month);
	if (date.day > days)
		date.day = days;
	return PumpTube_PackDate(date);
}

static void PumpTube_FormatDate(char *text, uint32_t value)
{
	TFTDateTime date = PumpTube_UnpackDate(value);
	sprintf(text, "%04u-%02u-%02u %02u:%02u", 2000U + (unsigned)date.year,
	        (unsigned)date.month, (unsigned)date.day, (unsigned)date.hour, (unsigned)date.minute);
}

static void PumpTube_UpdateDue(uint8_t index)
{
	PumpTubeChannelData *channel = &s_data.channel[index];
	uint8_t due = 0U;
	if ((channel->replaced_at != 0U) && (channel->cycle_months != 0U))
	{
		if (s_current_date == 0U)
			due |= channel->due_flags & PUMPTUBE_DUE_DATE;
		else if (s_current_date >= PumpTube_Deadline(channel->replaced_at, channel->cycle_months))
			due |= PUMPTUBE_DUE_DATE;
	}
	if ((channel->rotation_limit > 0U) &&
	    ((channel->motor_steps / PUMPTUBE_STEPS_PER_ROTATION) >= channel->rotation_limit))
		due |= PUMPTUBE_DUE_TURNS;
	if (due == 0U)
		s_presented_mask &= (uint8_t)~(1U << index);
	channel->due_flags = due;
}

void PumpTube_Init(void)
{
	PumpTubeStore store;
	uint32_t latest_address = 0U;
	uint8_t found = PumpTube_FindLatest(&store, &latest_address);
	memset(&s_data, 0, sizeof(s_data));
	if (found != 0U)
	{
		s_data = store.data;
		s_store_sequence = store.sequence;
	}

	/* The v1 store used the same layout with runtime seconds and a float hour limit. */
	for (uint8_t i = 0; i < PUMPTUBE_CHANNEL_COUNT; i++)
	{
		if ((found == 0U) || (store.version == 1U))
		{
			s_data.channel[i].replaced_at = 0U;
			s_data.channel[i].cycle_months = PUMPTUBE_DEFAULT_CYCLE;
			s_data.channel[i].due_flags = 0U;
		}
		PumpTube_UpdateDue(i);
	}
	s_boot_alert_tick = HAL_GetTick() + PUMPTUBE_BOOT_ALERT_DELAY_MS;
}

void PumpTube_StartMotor(uint8_t channel)
{
	if ((channel < 1U) || (channel > PUMPTUBE_CHANNEL_COUNT))
		return;

	uint8_t index = channel - 1U;
	uint32_t primask = PumpTube_EnterCritical();
	s_motor_tracking[index] = 1U;
	s_position_valid[index] = 1U;
	s_last_position[index] = 0U;
	PumpTube_ExitCritical(primask);
}

void PumpTube_UpdatePosition(uint8_t channel, int32_t position, uint8_t busy)
{
	if ((channel < 1U) || (channel > PUMPTUBE_CHANNEL_COUNT))
		return;

	uint8_t index = channel - 1U;
	uint32_t current = (uint32_t)position;
	uint32_t primask = PumpTube_EnterCritical();
	if (s_motor_tracking[index] != 0U)
	{
		if (s_position_valid[index] != 0U)
		{
			int32_t delta = (int32_t)(current - s_last_position[index]);
			uint64_t steps = (delta < 0) ? (uint64_t)(-(int64_t)delta) : (uint64_t)delta;
			if (s_data.enabled != 0U)
				s_data.channel[index].motor_steps += steps;
		}
		else
			s_position_valid[index] = 1U;

		s_last_position[index] = current;
		if (busy == 0U)
		{
			s_motor_tracking[index] = 0U;
			s_position_valid[index] = 0U;
			if (s_data.enabled != 0U)
				PumpTube_RequestSave();
		}
		PumpTube_UpdateDue(index);
	}
	PumpTube_ExitCritical(primask);
}

void PumpTube_StopMotor(uint8_t channel, int32_t position)
{
	PumpTube_UpdatePosition(channel, position, 0U);
}

void PumpTube_SetEnabled(uint8_t enabled)
{
	uint32_t primask = PumpTube_EnterCritical();
	uint8_t state = (enabled != 0U) ? 1U : 0U;
	if (s_data.enabled != state)
	{
		s_data.enabled = state;
		for (uint8_t i = 0; i < PUMPTUBE_CHANNEL_COUNT; i++)
		{
			if (s_motor_tracking[i] != 0U)
				s_position_valid[i] = 0U;
		}
		if (state == 0U)
			s_presented_mask = 0U;
		PumpTube_RequestSave();
	}
	PumpTube_ExitCritical(primask);
}

uint8_t PumpTube_IsEnabled(void)
{
	return s_data.enabled;
}

uint8_t PumpTube_GetDueMask(void)
{
	uint8_t mask = 0U;
	uint32_t primask = PumpTube_EnterCritical();
	if (s_data.enabled != 0U)
	{
		for (uint8_t i = 0; i < PUMPTUBE_CHANNEL_COUNT; i++)
		{
			if (s_data.channel[i].due_flags != 0U)
				mask |= (uint8_t)(1U << i);
		}
	}
	PumpTube_ExitCritical(primask);
	return mask;
}

void PumpTube_SetCycleMonths(uint8_t channel, uint32_t months)
{
	if ((channel < 1U) || (channel > PUMPTUBE_CHANNEL_COUNT) || (months > PUMPTUBE_MAX_CYCLE))
		return;
	uint32_t primask = PumpTube_EnterCritical();
	s_data.channel[channel - 1U].cycle_months = months;
	s_data.channel[channel - 1U].due_flags &= (uint8_t)~PUMPTUBE_DUE_DATE;
	PumpTube_UpdateDue(channel - 1U);
	PumpTube_RequestSave();
	PumpTube_ExitCritical(primask);
}

void PumpTube_SetRotationLimit(uint8_t channel, uint32_t rotations)
{
	if ((channel < 1U) || (channel > PUMPTUBE_CHANNEL_COUNT))
		return;
	uint32_t primask = PumpTube_EnterCritical();
	s_data.channel[channel - 1U].rotation_limit = rotations;
	PumpTube_UpdateDue(channel - 1U);
	PumpTube_RequestSave();
	PumpTube_ExitCritical(primask);
}

void PumpTube_GetData(PumpTubeData *data)
{
	if (data == NULL)
		return;
	uint32_t primask = PumpTube_EnterCritical();
	*data = s_data;
	PumpTube_ExitCritical(primask);
}

void PumpTube_RefreshManagementPage(void)
{
	PumpTubeData data;
	char text[32];
	static const uint8_t date_ids[] = {11U, 19U, 27U, 35U};
	static const uint8_t cycle_ids[] = {12U, 20U, 28U, 36U};
	static const uint8_t rotation_ids[] = {13U, 21U, 29U, 37U};
	static const uint8_t limit_ids[] = {14U, 22U, 30U, 38U};
	static const uint8_t status_ids[] = {15U, 23U, 31U, 39U};

	PumpTube_GetData(&data);
	if (s_last_enabled != (int8_t)data.enabled)
	{
		TFTDisplay_SwitchControl(PUMPTUBE_SCREEN_ID, 1U, data.enabled);
		s_last_enabled = (int8_t)data.enabled;
	}
	for (uint8_t i = 0; i < PUMPTUBE_CHANNEL_COUNT; i++)
	{
		const PumpTubeChannelData *channel = &data.channel[i];
		if (channel->replaced_at != 0U)
			PumpTube_FormatDate(text, channel->replaced_at);
		else
			strcpy(text, "未登记");
		TFTDisplay_SetTextValue(PUMPTUBE_SCREEN_ID, date_ids[i], text);
		sprintf(text, "%lu", (unsigned long)channel->cycle_months);
		TFTDisplay_SetTextValue(PUMPTUBE_SCREEN_ID, cycle_ids[i], text);
		uint64_t rotations = channel->motor_steps / PUMPTUBE_STEPS_PER_ROTATION;
		if (rotations > 0xFFFFFFFFULL)
			rotations = 0xFFFFFFFFULL;
		sprintf(text, "%lu", (unsigned long)rotations);
		TFTDisplay_SetTextValue(PUMPTUBE_SCREEN_ID, rotation_ids[i], text);
		sprintf(text, "%lu", (unsigned long)channel->rotation_limit);
		TFTDisplay_SetTextValue(PUMPTUBE_SCREEN_ID, limit_ids[i], text);

		uint8_t due = (data.enabled != 0U) && (channel->due_flags != 0U);
		char *status = "正常";
		if (data.enabled == 0U)
			status = "已关闭";
		else if ((channel->replaced_at == 0U) && (channel->cycle_months != 0U))
			status = "待登记";
		else if ((s_current_date == 0U) && (channel->cycle_months != 0U))
			status = "待校时";
		TFTDisplay_DisplayControl(PUMPTUBE_SCREEN_ID, (uint16_t)(45U + i), due);
		TFTDisplay_SetTextValue(PUMPTUBE_SCREEN_ID, status_ids[i], status);
		TFTDisplay_DisplayControl(PUMPTUBE_SCREEN_ID, status_ids[i], !due);
	}
}

/* Numeric keyboard reports text after the type byte, followed by an optional NUL. */
static uint8_t PumpTube_ReadNumber(const uint8_t *frame, uint16_t length, uint32_t *value)
{
	if ((length < 13U) || (frame[1] != 0xB1U) || (frame[2] != 0x11U) || (frame[7] != 0x11U))
		return 0U;
	uint16_t end = length - 4U;
	if (frame[end - 1U] == 0U)
		end--;
	if ((end <= 8U) || (end - 8U > 10U))
		return 0U;
	*value = 0U;
	for (uint16_t i = 8U; i < end; i++)
	{
		if ((frame[i] < '0') || (frame[i] > '9') ||
		    (*value > (1000000000UL - (frame[i] - '0')) / 10U))
			return 0U;
		*value = *value * 10U + (frame[i] - '0');
	}
	return 1U;
}

void PumpTube_HandleManagementInput(uint16_t control, const uint8_t *frame, uint16_t length)
{
	uint32_t value;
	static const uint8_t cycle_ids[] = {12U, 20U, 28U, 36U};
	static const uint8_t limit_ids[] = {14U, 22U, 30U, 38U};
	for (uint8_t i = 0; i < PUMPTUBE_CHANNEL_COUNT; i++)
	{
		if ((control != cycle_ids[i]) && (control != limit_ids[i]))
			continue;
		if (PumpTube_ReadNumber(frame, length, &value) &&
		    ((control == limit_ids[i]) || (value <= PUMPTUBE_MAX_CYCLE)))
		{
			if (control == cycle_ids[i])
				PumpTube_SetCycleMonths(i + 1U, value);
			else
				PumpTube_SetRotationLimit(i + 1U, value);
			if (PumpTube_SaveNow() == HAL_OK)
				Beep_S();
		}
		PumpTube_RefreshManagementPage();
		break;
	}
}

static void PumpTube_RefreshRegistration(uint8_t refresh_fields)
{
	char text[56], date[24];
	if (refresh_fields != 0U)
	{
		unsigned values[] = {2000U + (unsigned)s_registration_date.year,
		                     s_registration_date.month, s_registration_date.day,
		                     s_registration_date.hour, s_registration_date.minute};
		for (uint8_t i = 0; i < 5U; i++)
		{
			text[0] = '\0';
			if (s_registration_date.year != 0U || i != 0U)
				sprintf(text, (i == 0U) ? "%04u" : "%02u", values[i]);
			TFTDisplay_SetTextValue(PUMPTUBE_REGISTER_SCREEN, (uint16_t)(8U + i), text);
		}
	}

	for (uint8_t i = 0; i < PUMPTUBE_CHANNEL_COUNT; i++)
		TFTDisplay_SwitchControl(PUMPTUBE_REGISTER_SCREEN, (uint16_t)(4U + i),
		                         (s_registration_pump == i + 1U));
	if (s_registration_pump == 0U)
		strcpy(text, "请选择泵号");
	else
	{
		uint32_t months = s_data.channel[s_registration_pump - 1U].cycle_months;
		uint32_t replaced = PumpTube_PackDate(s_registration_date);
		if (s_current_date == 0U)
			strcpy(text, "时钟未就绪，请先校时");
		else if (!PumpTube_DateValid(s_registration_date))
			strcpy(text, "请输入有效日期和时间");
		else if (replaced > s_current_date)
			strcpy(text, "更换时间不能晚于当前时间");
		else if (months == 0U)
			strcpy(text, "未开启周期提醒");
		else
		{
			PumpTube_FormatDate(date, PumpTube_Deadline(replaced, months));
			sprintf(text, "周期到期  %s", date);
		}
	}
	TFTDisplay_SetTextValue(PUMPTUBE_REGISTER_SCREEN, 19U, text);
	text[0] = '\0';
	if (s_registration_pump != 0U)
		sprintf(text, "保存后清零%u号泵累计圈数", (unsigned)s_registration_pump);
	TFTDisplay_SetTextValue(PUMPTUBE_REGISTER_SCREEN, 22U, text);
}

void PumpTube_OpenRegistration(void)
{
	s_registration_pump = 0U;
	s_registration_date = PumpTube_UnpackDate(s_current_date);
	g_NowStatus.NowScreenPage = Screen_PumpTubeRegistration;
	PumpTube_RefreshRegistration(1U);
	TFTDisplay_SwitchScreen(PUMPTUBE_REGISTER_SCREEN);
}

static void PumpTube_CloseRegistration(void)
{
	g_NowStatus.NowScreenPage = Screen_PumpTubeManagement;
	PumpTube_RefreshManagementPage();
	TFTDisplay_SwitchScreen(PUMPTUBE_SCREEN_ID);
}

void PumpTube_HandleRegistration(uint16_t control, const uint8_t *frame, uint16_t length)
{
	uint32_t number;
	if ((control >= 8U) && (control <= 12U))
	{
		static const uint16_t min_value[] = {2020U, 1U, 1U, 0U, 0U};
		static const uint16_t max_value[] = {2099U, 12U, 31U, 23U, 59U};
		uint8_t field = (uint8_t)(control - 8U);
		if (PumpTube_ReadNumber(frame, length, &number) &&
		    (number >= min_value[field]) && (number <= max_value[field]))
		{
			uint8_t *fields[] = {&s_registration_date.year, &s_registration_date.month,
			                     &s_registration_date.day, &s_registration_date.hour,
			                     &s_registration_date.minute};
			*fields[field] = (uint8_t)(number - ((field == 0U) ? 2000U : 0U));
		}
		PumpTube_RefreshRegistration(1U);
		return;
	}

	if ((length != 14U) || (frame[1] != 0xB1U) || (frame[2] != 0x11U) ||
	    (frame[7] != 0x10U))
		return;

	if ((control >= 4U) && (control <= 7U))
	{
		/* A switch reports its new state, so either state selects the pump. */
		if (frame[9] <= 1U)
		{
			s_registration_pump = (uint8_t)(control - 3U);
			PumpTube_RefreshRegistration(0U);
		}
		return;
	}

	if ((control != 21U) && (frame[9] != 0U))
		return;
	switch (control)
	{
	case 18U:
		if (s_current_date != 0U)
			s_registration_date = PumpTube_UnpackDate(s_current_date);
		PumpTube_RefreshRegistration(1U);
		break;
	case 20U:
		PumpTube_CloseRegistration();
		break;
	case 21U:
	{
		uint32_t date = PumpTube_PackDate(s_registration_date);
		if ((s_registration_pump == 0U) || (s_current_date == 0U) ||
		    !PumpTube_DateValid(s_registration_date) || (date > s_current_date))
		{
			PumpTube_RefreshRegistration(0U);
			return;
		}
		uint8_t index = s_registration_pump - 1U;
		uint32_t primask = PumpTube_EnterCritical();
		PumpTubeChannelData previous = s_data.channel[index];
		uint8_t previous_presented = s_presented_mask;
		uint8_t previous_pending = s_save_pending;
		if ((s_motor_tracking[index] != 0U) || (g_NowStatus.WorkingBusy != USER_FREE))
		{
			PumpTube_ExitCritical(primask);
			TFTDisplay_SetTextValue(PUMPTUBE_REGISTER_SCREEN, 19U, "请在泵停止后保存");
			return;
		}
		s_data.channel[index].replaced_at = date;
		s_data.channel[index].motor_steps = 0U;
		s_data.channel[index].due_flags = 0U;
		s_presented_mask &= (uint8_t)~(1U << index);
		PumpTube_UpdateDue(index);
		PumpTube_ExitCritical(primask);

		if (PumpTube_SaveNow() == HAL_OK)
		{
			Beep_S();
			PumpTube_CloseRegistration();
		}
		else
		{
			primask = PumpTube_EnterCritical();
			s_data.channel[index] = previous;
			s_presented_mask = previous_presented;
			s_save_pending = previous_pending;
			PumpTube_ExitCritical(primask);
			TFTDisplay_SetTextValue(PUMPTUBE_REGISTER_SCREEN, 19U, "保存失败，请重试");
		}
		break;
	}
	default:
		break;
	}
}

static void PumpTube_ShowAlert(uint8_t mask)
{
	PumpTubeData data;
	char text[48];
	PumpTube_GetData(&data);
	for (uint8_t i = 0; i < PUMPTUBE_CHANNEL_COUNT; i++)
	{
		text[0] = '\0';
		if ((mask & (uint8_t)(1U << i)) != 0U)
		{
			char *reason = "圈数达到限值";
			if (data.channel[i].due_flags == PUMPTUBE_DUE_DATE)
				reason = "更换周期已到期";
			else if (data.channel[i].due_flags == (PUMPTUBE_DUE_DATE | PUMPTUBE_DUE_TURNS))
				reason = "周期和圈数均到限";
			sprintf(text, "%u号泵：%s", (unsigned)(i + 1U), reason);
		}
		TFTDisplay_SetTextValue(PUMPTUBE_ALERT_SCREEN, (uint16_t)(3U + i), text);
	}
	TFTDisplay_SwitchScreen(PUMPTUBE_ALERT_SCREEN);
	s_presented_mask |= mask;
}

static void PumpTube_ServiceRTC(uint32_t now)
{
	if ((s_rtc_requested == 0U) || (now - s_rtc_request_at >= 1000UL))
	{
		s_rtc_requested = 1U;
		s_rtc_request_at = now;
		TFTDisplay_RequestRTC();
	}

	TFTDateTime date;
	uint32_t received_at;
	uint32_t packed = 0U;
	if (TFTDisplay_ReadRTC(&date, &received_at) && (HAL_GetTick() - received_at < 5000UL))
	{
		s_rtc_seen = 1U;
		if (PumpTube_DateValid(date))
			packed = PumpTube_PackDate(date);
	}
	uint32_t previous = s_current_date;
	if (packed != previous)
	{
		uint32_t primask = PumpTube_EnterCritical();
		s_current_date = packed;
		for (uint8_t i = 0; i < PUMPTUBE_CHANNEL_COUNT; i++)
			PumpTube_UpdateDue(i);
		PumpTube_ExitCritical(primask);
	}
	if (g_NowStatus.NowScreenPage == Screen_PumpTubeManagement)
	{
		if (packed != previous)
			PumpTube_RefreshManagementPage();

	}
	else if ((g_NowStatus.NowScreenPage == Screen_PumpTubeRegistration) &&
	         ((packed == 0U) != (previous == 0U)))
		PumpTube_RefreshRegistration(0U);
}

void PumpTube_Service(void)
{
	uint32_t now = HAL_GetTick();
	if ((s_data.enabled != 0U) || (g_NowStatus.NowScreenPage == Screen_PumpTubeManagement) ||
	    (g_NowStatus.NowScreenPage == Screen_PumpTubeRegistration))
		PumpTube_ServiceRTC(now);
	else
		s_current_date = 0U;

	uint8_t due_mask = PumpTube_GetDueMask();
	if (due_mask != s_last_icon_mask)
	{
		for (uint8_t i = 0; i < PUMPTUBE_CHANNEL_COUNT; i++)
			TFTDisplay_DisplayControl(0U, (uint16_t)(96U + i),
			                          (due_mask & (uint8_t)(1U << i)) != 0U);
		s_last_icon_mask = due_mask;
	}
	uint8_t tracking_active = 0U;
	for (uint8_t i = 0; i < PUMPTUBE_CHANNEL_COUNT; i++)
		tracking_active |= s_motor_tracking[i];

	if ((tracking_active == 0U) && (g_NowStatus.WorkingBusy == USER_FREE) &&
	    (g_NowStatus.NowScreenPage == Screen_Working) &&
	    ((s_rtc_seen != 0U) || ((int32_t)(now - s_boot_alert_tick) >= 0)) &&
	    ((due_mask & (uint8_t)~s_presented_mask) != 0U))
	{
		g_NowStatus.NowScreenPage = Screen_PumpTubeAlert;
		PumpTube_ShowAlert(due_mask);
	}

	if ((s_save_pending != 0U) && (tracking_active == 0U) &&
	    ((s_save_retry_limited == 0U) || ((now - s_last_save_attempt) >= 1000UL)))
		(void)PumpTube_SaveNow();
}
