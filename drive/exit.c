#include "exit.h"


void EXIT_KEY_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct; 
  	EXTI_InitTypeDef EXTI_InitStruct;
	  NVIC_InitTypeDef NVIC_InitStruct;
	  /* config the extiline(PB0) clock and AFIO clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
	  

  
    /* Configyre P[A|B|C|D|E]0  NIVC  */
		NVIC_InitStruct.NVIC_IRQChannel = EXTI4_15_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelPriority = 0x00;
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStruct);
	
	  	/* EXTI line gpio config(PF7) */	
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7; 
	  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; // 上拉输入
		GPIO_Init(GPIOF, &GPIO_InitStruct);

		/* EXTI line(PB0) mode config */
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource7);
		EXTI_InitStruct.EXTI_Line = EXTI_Line7;
		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStruct); 
	}
	
	