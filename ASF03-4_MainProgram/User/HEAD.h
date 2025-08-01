#ifndef _HEAD_H
#define _HEAD_H

#include "stm32f10x.h"
#include <math.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "bsp_Delay.h"
#include "bsp_BEEP.h"
#include "bsp_TFT_Display.h"
#include "bsp_ADC.h"
#include "bsp_SRX885.h"
#include "bsp_BasicTIM.h"
#include "bsp_TMC5240.h"
#include "bsp_TMC5240SPI.h"
#include "bsp_Flash.h"
#include "Function.h"


extern float Current_liquid_volume; //当前液量，屏幕输入
extern float Add_liquid[8];					//4排待加液的体积，每一排需要加液的体积
extern u8 Add_laction[4][10];			//加液状态，1，已完成加液或是不需要加液，0，需要加液
extern u8 AllSwitchFlag;	//【全开/闭】标志位，1表示开，0表示关。默认为开
extern u8 CheckPipeFlag;	//管道检测开启标志位，1表示为开启，0表示为未开启。默认不开启
extern u8 ChoosePump[4];		//蠕动泵校准界面，选中标志位，1表示选中，0表示没选中



extern char Str[30];		//临时存储变量

#endif

