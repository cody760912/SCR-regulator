
/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configure the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/


#include "stm32f0xx.h"              /* STM32F10x Library Definitions      */

void SetupNVIC(void);
void SetupNVIC(void)
 {
  NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM
  /* Set the Vector Table base location at 0x20000000 */
  //NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */
  //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

	/* Configure one bit for preemption priority */
  	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);


	/* Enable the TIM3 gloabal Interrupt */
  	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);


	/* Enable the TIM3 gloabal Interrupt */
  	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);



	/* Set SysTick interrupt vector Preemption Priority to 1 */
  	//NVIC_SystemHandlerPriorityConfig(SystemHandler_SysTick, 1, 0);

	/* Enable the USART1 gloabal Interrupt */
	//NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPriority =0;   	      
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPriority =0;   	      
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC_Init(&NVIC_InitStructure);

}

 





