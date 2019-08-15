/******************** (C) COPYRIGHT 2012 ������ ********************
 * �ļ���  ��main
 * ����    ��         
 * ʵ��ƽ̨�����stm32f051������
 * ����    ��ADC����
 * ����    �����
 * ����    ��qfv5.taobao.com
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

