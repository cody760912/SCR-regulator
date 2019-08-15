#include "stm32f0xx_tim.h"
#include "stm32f0xx.h"              /* STM32F10x Library Definitions      */
//#include "stm32f10x_gpio.h"
void SetupTIMBASE(void);

//vu16 CCR1_Val = 49152;
//vu16 CCR2_Val = 32768;
//vu16 CCR3_Val = 16384;
//vu16 CCR4_Val = 8192;
void SetupTIMBASE(void)
{
   	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	//TIM_OCInitTypeDef  TIM_OCInitStructure;

	//TIM_ICInitTypeDef  TIM_ICInitStructure;

	
	
	
  	/* TIM3 clock enable */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
    //48000000/((TIM_Period +1)*(TIM_Prescaler + 1)) = 48000000/(8*6000) = 1000HZ
	/* Time base configuration */
  	TIM_TimeBaseStructure.TIM_Period = 7;	  //如果CCRx>=APR,则FREquency=TIM2CLK/APR
  	TIM_TimeBaseStructure.TIM_Prescaler = 5999;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* TIM IT enable */
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	/* TIM3 enable counter */
 	TIM_Cmd(TIM3, ENABLE);
	

    
	/* TIM2 clock enable */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/* Time base configuration */
	//48000000/((TIM_Period +1)*(TIM_Prescaler + 1)) = 48000000/(40*6000) = 200HZ
  	TIM_TimeBaseStructure.TIM_Period = 39;	  //如果CCRx>=APR,则FREquency=TIM2CLK/APR
  	TIM_TimeBaseStructure.TIM_Prescaler = 5999;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	
  	#if 0
    //48000000/((TIM_Period +1)*(TIM_Prescaler + 1)) = 48000000/(160*6000) = 50HZ
	/* Time base configuration */
  	TIM_TimeBaseStructure.TIM_Period = 159;	  //如果CCRx>=APR,则FREquency=TIM2CLK/APR
  	TIM_TimeBaseStructure.TIM_Prescaler = 5999;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	#endif

  	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* TIM IT enable */
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	/* TIM3 enable counter */
 	TIM_Cmd(TIM2, ENABLE);


}

