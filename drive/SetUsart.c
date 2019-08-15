
//www.u-easytech.com	  
/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configures the USART1.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

#include "stm32f0xx.h"              /* STM32F10x Library Definitions      */
#include "stdio.h"

#include "work.h"

//int GetKey (void) ;
//int SendChar (int ch) ;

void SetupUSART (void)  {

  USART_InitTypeDef USART_InitStructure;

/* USART1 configuration ------------------------------------------------------*/
  /* USART1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */
/* USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_Clock = USART_Clock_Disable;
  USART_InitStructure.USART_CPOL = USART_CPOL_Low;
  USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
  USART_InitStructure.USART_LastBit = USART_LastBit_Disable;

  USART_Init(USART1, &USART_InitStructure);
    

  USART_Cmd(USART1, ENABLE);  */

  /* USART1 configuration ------------------------------------------------------*/
  /* USART1 configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - Two Stop Bit
        - Odd parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the second edge 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );


	USART_InitStructure.USART_BaudRate = 19200;//设置串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//设置数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//设置停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//设置效验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//设置流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//设置工作模式
    USART_Init(USART1, &USART_InitStructure); //配置入结构体


	USART_ClearFlag(USART1, USART_IT_RXNE|USART_IT_TXE);


	USART_ITConfig(USART1, USART_IT_RXNE|USART_IT_TXE, ENABLE);


    USART_Cmd(USART1, ENABLE);//使能串口1



	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE );


	USART_InitStructure.USART_BaudRate = 19200;//设置串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//设置数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//设置停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//设置效验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//设置流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//设置工作模式
    USART_Init(USART2, &USART_InitStructure); //配置入结构体


	USART_ClearFlag(USART2, USART_IT_RXNE|USART_IT_TXE);


	USART_ITConfig(USART2, USART_IT_RXNE|USART_IT_TXE, ENABLE);


    USART_Cmd(USART2, ENABLE);//使能串口1


  	//USART_InitStructure.USART_BaudRate            = 19200;
  	//USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  	//USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  	//USART_InitStructure.USART_Parity              = USART_Parity_No ;//USART_Parity_Odd
  	//USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	//USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

 
  	//USART_Init(USART3, &USART_InitStructure);
 	//	if you want to set the parity bit ,you must select 9bit data
 
  	/* Configure the USART3 */
	//USART_Init(USART2, &USART_InitStructure);

	/* Enable the USART Transmoit interrupt: this interrupt is generated when the 
   	USART1 transmit data register is empty */  
  	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

	/* Enable the USART Receive interrupt: this interrupt is generated when the 
  	 	USART3 receive data register is not empty */
  	//USART_ITConfig(USART2, USART_IT_RXNE|USART_IT_TXE, ENABLE);

  	/* Enable USART3 */
  	//USART_Cmd(USART2, ENABLE);

}
#if 0
/*******************************************************************************
* Function Name  : SetupUSARTBaudRate
* Description    : setup usart baudrate
* Input          : - USARTx: Select the USART or the UART peripheral. 
*                    This parameter can be one of the following values:
*                     - USART1, USART2, USART3, UART4 or UART5.
*                  - BaudRate: set usart baud rate.
* Output         : None
* Return         : None
*******************************************************************************/
void SetupUSARTBaudRate(USART_TypeDef* USARTx, u32 BaudRate)  
{
	u32 tmpreg = 0x00, apbclock = 0x00;
  	u32 integerdivider = 0x00;
  	u32 fractionaldivider = 0x00;
  	u32 usartxbase = 0;
  	RCC_ClocksTypeDef RCC_ClocksStatus;

	/* Check the parameters */
  	assert_param(IS_USART_BAUDRATE(BaudRate));  

  	usartxbase = (*(u32*)&USARTx);
	
	/* Configure the USART Baud Rate -------------------------------------------*/
  	RCC_GetClocksFreq(&RCC_ClocksStatus);
  	if (usartxbase == USART1_BASE)
  	{
    	apbclock = RCC_ClocksStatus.PCLK2_Frequency;
  	}
  	else
  	{
    	apbclock = RCC_ClocksStatus.PCLK1_Frequency;
  	}

  	/* Determine the integer part */
  	integerdivider = ((0x19 * apbclock) / (0x04 * (BaudRate)));
  	tmpreg = (integerdivider / 0x64) << 0x04;

  	/* Determine the fractional part */
  	fractionaldivider = integerdivider - (0x64 * (tmpreg >> 0x04));
  	tmpreg |= ((((fractionaldivider * 0x10) + 0x32) / 0x64)) & ((u8)0x0F);

  	/* Write to USART BRR */
  	USARTx->BRR = (u16)tmpreg;
}
#endif

/*******************************************************************************
* Function Name  : SetupUSARTMode
* Description    : setup usart trasmit/recevie
* Input          : - USARTx: Select the USART or the UART peripheral. 
*                    This parameter can be one of the following values:
*                     - USART1, USART2, USART3, UART4 or UART5.
*                  - USART_Mode: USART_Mode_Rx | USART_Mode_Tx.
* Output         : None
* Return         : None
*******************************************************************************/
void SetupUSARTMode(USART_TypeDef* USARTx, u16 USART_Mode)
{
	u32 tmpreg = 0x00;
	assert_param(IS_USART_MODE(USART_Mode));

	tmpreg = USARTx->CR1;
  	/* Clear TE and RE bits */
  	tmpreg &= 0xfff3;

  	/* Set TE and RE bits according to USART_Mode value */
  	tmpreg |= (u32)USART_Mode;

  	/* Write to USART CR1 */
  	USARTx->CR1 = (u16)tmpreg;
}





/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/

