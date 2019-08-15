/******************** (C) COPYRIGHT 2012 青风电子 ********************
 * 文件名  ：main
 * 描述    ：         
 * 实验平台：青风stm32f051开发板
 * 描述    ：ADC采样
 * 作者    ：青风
 * 店铺    ：qfv5.taobao.com
**********************************************************************/

#include "stm32f0xx.h"


void SetupGPIO(void);
void SetupNVIC(void);
void SetupTIMBASE(void);
void SetupADC(void);
void SysTick_Configuration(void);
void SetupUSART (void);
void WorkProgram(void);




void delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
} 

int main(void)
{
	  SystemInit();
	  
	  SetupGPIO();

	  SetupADC();

	  SetupTIMBASE();

	  SetupUSART();

	  SetupNVIC();
	  
	  WorkProgram();
}

