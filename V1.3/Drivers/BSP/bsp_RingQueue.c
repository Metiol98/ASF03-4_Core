#include "bsp_RingQueue.h"

RingBuffTypeDef g_DisPlayRingBuff;		/* 来自屏幕指令的数据环 */

/**
  * @brief  环形队列的初始化，
  * @param  *p.环形队列结构体形参
  * @retval None. 
  */
void RingBuff_Init(RingBuffTypeDef *p)
{
	for(int i=0;i<sizeof(p->array);i++)
		p->array[i] =0;
	p->front = 0;
	p->rear = 0;
	p->size = 0;
}

/**
  * @brief  判断环形队列是否已经满了
  * @param  *p.环形队列结构体形参
  * @retval 8bit. 
	*     @arg USER_TURE    			: 满了
  *     @arg USER_FALSE    			: 没满
  */
uint8_t RingBuff_GetFFullstate(RingBuffTypeDef *p)
{
	/*如果写位置和读的位置相等，就说明这个环形队列为空*/
	if(p->size == RINTQUEUESIZE)
		return RingBuff_TRUE;
	else
		return RingBuff_FALSE;
	
}

/**
  * @brief  判断环形队列为空
  * @param  *p.环形队列结构体形参
  * @retval 8bit. 
	*     @arg USER_TURE    			: 空
  *     @arg USER_FALSE    			: 没空
  */
uint8_t RingBuff_GetEmptystate(RingBuffTypeDef *p)
{
	/*如果写位置和读的位置相等，就说明这个环形队列为空*/
	if(p->size == 0)
		return RingBuff_TRUE;
	else
		return RingBuff_FALSE;
}

/**
  * @brief  环形队列插入数据,自动吧数据的长度放在单帧的开头
  * @param  *p.环形队列结构体形参
	* @param  *Data.要插入的数组形参
	*	@param  len.数据长度
  * @retval 8bit. 
	*     @arg USER_TURE    			: 完成
  *     @arg USER_FALSE    			: 失败
  */
uint8_t RingBuff_Insert(RingBuffTypeDef *p,uint8_t *Data,uint32_t len)
{
	if(sizeof(p->array)-p->size < (len+1))
		return RingBuff_FALSE; 			/* 没有足够的空间，保存失败 */

	p->array[p->rear] = len;	/* 保存一帧的长度 */
	p->rear++;
	if(p->rear>=RINTQUEUESIZE)
		p->rear = 0;

	for(int i=0; i<len; i++)
	{
			p->array[p->rear] = Data[i];
			p->rear++;
			if(p->rear>=RINTQUEUESIZE)
				p->rear=0;
	}
	p->size += (len+1);			/* 数据量更新 */
	return RingBuff_TRUE;
}

/**
  * @brief  环形队列读出数据
  * @param  *p.环形队列结构体形参
	* @param  *Data.要插入的数组形参
	*	@param  len.读出的数据长度
  * @retval 8bit. 
	*     @arg USER_TURE    			: 完成
  *     @arg USER_FALSE    			: 失败
  */
uint8_t RingBuff_Getout(RingBuffTypeDef *p,uint8_t *Data,uint32_t *len)
{
	if(p->size == 0)
		return RingBuff_FALSE; /* 没有足够的数据，读出失败 */

	*len = p->array[p->front];	/* 获取数据帧的长度 */
	p->front++;
	if(p->front >= RINTQUEUESIZE)
		p->front=0;

	for(int i=0; i<*len; i++)
	{
		Data[i] = p->array[p->front];
		p->front++;
		if(p->front >= RINTQUEUESIZE)
			p->front=0;
	}
	p->size -= (*len+1);	/* 数据量更新 */
	return RingBuff_TRUE;
}



























