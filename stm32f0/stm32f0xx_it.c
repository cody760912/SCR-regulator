/**
  ******************************************************************************
  * @file    EXTI/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"

#include "uet_gpio.h"

#include "work.h"


#include "commu.h"

#include "dwin.h"

/** @addtogroup STM32F0_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup EXTI_Example
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


extern UARTBUF Uart[MAX_UART];

extern BIT16 ROArray[];

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	 TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{
//   if(EXTI_GetITStatus(EXTI_Line7) != RESET)
//   {
//     /* Toggle LED3 and LED4 */
//     GPIO_WriteBit(GPIOA, GPIO_Pin_11, 
// 		             (BitAction)((1-GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_11))));
//     GPIO_WriteBit(GPIOA, GPIO_Pin_12, 
// 		             (BitAction)((1-GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_12))));
//     /* Clear the EXTI line 0 pending bit */
//     EXTI_ClearITPendingBit(EXTI_Line7);
//   }
}

/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{

  	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  	{
    	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    	
		DealStepOutputCom();
  	}
}

/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
	extern void IT_Time(void);
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  	{
    	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	
		
		IT_Time();
  	}
}

/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1,USART_IT_TXE) != RESET )
	{
		USART_ClearITPendingBit(USART1,USART_IT_TXE);

		DWIN_UartTx();

	}
	else if (USART_GetITStatus(USART1,USART_IT_RXNE))
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);

		DWIN_UartRx();
		
	}
	else if (USART_GetITStatus(USART1,USART_IT_TC))
	{
		USART_ClearITPendingBit(USART1,USART_IT_TC);

		USART_ITConfig(USART1,USART_IT_TC,DISABLE);

		Uart[COM_UART1].tras_cnt++;

		//UartReceStart(COM_UART1);
	}
}

/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2,USART_IT_TXE) != RESET )
	{
		Uart[COM_UART2].tras_cnt++;

		USART_ClearITPendingBit(USART2,USART_IT_TXE);

		if (Uart[COM_UART2].tras_cnt < Uart[COM_UART2].tras_total)
		{
			USART2->TDR = (Uart[COM_UART2].tras_buf[Uart[COM_UART2].tras_cnt] & (u16)0x01FF);

			if (Uart[COM_UART2].tras_cnt == Uart[COM_UART2].tras_total-1)	/* send last one close txe interrupt */
			{
				USART_ITConfig(USART2,USART_IT_TXE,DISABLE);

				USART_ITConfig(USART2,USART_IT_TC,ENABLE);
			}
		}
		else
		{
			USART_ITConfig(USART2,USART_IT_TXE,DISABLE);
		}
	}
	else if (USART_GetITStatus(USART2,USART_IT_RXNE))
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);

		if (Uart[COM_UART2].rece_cnt < COMMU_RECE_MAX_NUM)
		{
			Uart[COM_UART2].rece_buf[Uart[COM_UART2].rece_cnt++] = (u8)USART2->RDR;
		}

		Uart[COM_UART2].rece_end_dly = COMMU_RECE_END_DLY;
		
	}
	else if (USART_GetITStatus(USART2,USART_IT_TC))
	{
		USART_ClearITPendingBit(USART2,USART_IT_TC);

		USART_ITConfig(USART2,USART_IT_TC,DISABLE);

		UartReceStart(COM_UART2);
	}
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
