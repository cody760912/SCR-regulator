/******************** u-Sendy ********************
* File Name          : commu.c
* Author             : Sendy
* Version            : V1.0
* Date               : 08/13/2009
* Description        : communication program body
********************************************************************************

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stdio.h"
//#include "uet_gpio.h"
#include "work.h"
#include "commu.h"

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

#define MODBUS_NO_ERR       0    /* 无异常 */
#define MODBUS_ERR_01       1    /* 非法功能 */
#define MODBUS_ERR_02       2    /* 非法数据地址 */
#define MODBUS_ERR_03       3    /* 非法数据值 */
#define MODBUS_ERR_04       4    /* 从设备故障 */
#define MODBUS_ERR_05       5    /* 确认  客户机(或主站)可以继续发送轮询程序完成报文来确定是否完成处理 */
#define MODBUS_ERR_06       6    /* 从设备忙 发送轮询程序完成报文来确定是否完成处理，空闲后重发数据 */


#define MODBUS_ADDR_R_BIT_START		0
#define MODBUS_ADDR_R_BIT_END		1600

#define MODBUS_ADDR_W_BIT_START		0
#define MODBUS_ADDR_W_BIT_END		1600

#define MODBUS_ADDR_R_WORD_START    0
#define MODBUS_ADDR_R_WORD_END		(MODBUS_ADDR_R_WORD_START+MODBUS_READ_ONLY_NUM)

#define MODBUS_ADDR_W_WORD_START    0
#define MODBUS_ADDR_W_WORD_END		(MODBUS_ADDR_W_WORD_START+MODBUS_WRITE_NUM)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

UARTBUF Uart[MAX_UART];			/* 通讯缓冲区 */

extern BIT16 ROArray[];
extern BIT16 WRArray[];


/* Private function prototypes -----------------------------------------------*/

extern void SetupUSARTMode(USART_TypeDef * USARTx,u16 USART_Mode);

BOOL ProtrolPcRece(UARTBUF * pCOM);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : SetupUsartAddr
* Description    : Set usart address, used when slave
* Input          : delay number
* Output         : None
* Return         : None
*******************************************************************************/
void SetupUsartAddr(u8 u, u8 Addr)
{
	Uart[u].addr = Addr;
}

/*******************************************************************************
* Function Name  : UartTrasStart
* Description    : uart start trasmit
* Input          : which uart
* Output         : None
* Return         : None
*******************************************************************************/
void UartTrasStart(u8 u)
{
	Uart[u].tras_cnt = 0;
	//Uart[u].rece_cnt = 0;
	
	switch (u)		/* 置为发送 */
	{
		case COM_UART1 : 
			/* 关接收允许 */
			//GPIO_A8(1); 	/* set usart send mode */

			//GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_SET);
			
			SetupUSARTMode(USART1,USART_Mode_Tx|USART_Mode_Rx);

			USART_ClearFlag(USART1,USART_FLAG_TC);

			/* must clear txe interrupt flag */
			USART_SendData(USART1,(u16)Uart[u].tras_buf[Uart[u].tras_cnt]);

			USART_ITConfig(USART1,USART_IT_TXE,ENABLE);

			
			break;
		case COM_UART2 : 
			/* 关接收允许 */
			//GPIO_A8(1); 	/* set usart send mode */

			//GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_SET);
			
			SetupUSARTMode(USART2,USART_Mode_Tx);

			USART_ClearFlag(USART2,USART_FLAG_TC);

			/* must clear txe interrupt flag */
			USART_SendData(USART2,(u16)Uart[u].tras_buf[Uart[u].tras_cnt]);

			USART_ITConfig(USART2,USART_IT_TXE,ENABLE);

			
			break;	

	}	
}

/*******************************************************************************
* Function Name  : UartReceStart
* Description    : uart start receive
* Input          : which uart
* Output         : None
* Return         : None
*******************************************************************************/
void UartReceStart(U08 u)
{
	volatile U16 receData;
	
	switch (u)
	{
		case COM_UART1 : 
			/* 关接收允许 */
			SetupUSARTMode(USART1,USART_Mode_Tx|USART_Mode_Rx);

			receData = USART_ReceiveData(USART1);/* 虚读 */

			USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);

			//GPIO_A8(0);

			//GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_RESET);
	
			break;
		case COM_UART2 : 
			/* 关接收允许 */
			SetupUSARTMode(USART2,USART_Mode_Rx);

			receData = USART_ReceiveData(USART2);/* 虚读 */

			USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);

			//GPIO_A8(0);

			//GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_RESET);
	
			break;	
	}
	
	Uart[u].rece_cnt = 0;
}

/*==========================================
	通讯故障标识
/==========================================*/
BOOL COM_error_get(U08 u, U08 n)	
{	
	n = n%16;	// 由于地址超过16
	return (Uart[u].commu_error >> n) & 0x0001;		
}
void COM_error_set(U08 u, U08 n)	
{	
	n = n%16;
	Uart[u].commu_error |= (0x0001 << n);			
}
void COM_error_clr(void)
{
	U08  i;
	for (i=0; i<4; i++) Uart[i].commu_error = 0;
}
/*==========================================
	获取通讯状态
/==========================================*/
U08 COM_status_get(U08 u)
{
	return Uart[u].step;
}



/* ================================================================================================= */
/*		以下为传输层函数																			 */
/* ================================================================================================= */




/* ================================================================================================= */
/*		应用层																						 */
/* ================================================================================================= */
/*=========================================================================
		串口从机处理接口
/=========================================================================*/
U08 COM_UartSlave(u8 u)
{
	UARTBUF *pCOM;

	pCOM = &Uart[u];

	switch(pCOM->step)
	{
  	case COMMU_IDLE : 	/* 空闲 */
  		pCOM->step = COMMU_RECE;/* 接收到数据 */
  		pCOM->overtime_dly = 4000/*0*/;	/* 30s */
		
  		break;
  	case COMMU_RECE :	/* 接收 */
  		if ((pCOM->rece_cnt > 0) && (pCOM->rece_end_dly == 0))
  		{	
  			if (ProtrolPcRece(pCOM)) 
			{	
				pCOM->LedFlashCnt = 20;
				pCOM->step = COMMU_TRAS;		/* 回应 */
				pCOM->rece_cnt = 0;
			}
			else 
			{
				pCOM->step = COMMU_IDLE;
				UartReceStart(u);
			}
  		}
		
  		if (pCOM->overtime_dly == 0)
		{
			pCOM->step = COMMU_ALARM;	/* 通讯超时 */
  		}
  		break;
  	case COMMU_TRAS :	/* 发送 */
  		UartTrasStart(u);
  		pCOM->step = COMMU_IDLE;
  		break;
  	case COMMU_ALARM :
  		pCOM->step = COMMU_IDLE;
		pCOM->Bus_Overtime_Conut++;		/* 超时计数 */
		
		COM_error_set(u, 0);		/* 故障标识 */
  		break;
	default:
		pCOM->step = COMMU_IDLE;
		UartReceStart(u);
		break;
	}

	return pCOM->step;
}


/*=========================================================================
		通讯计时(5ms调用一次)
/=========================================================================*/
void UartTimer(void)
{
	U08  i;
	UARTBUF *pCOM;
	
	for (i=0; i<MAX_UART; i++)
	{
		pCOM = &Uart[i];

		if (pCOM->rece_end_dly)  pCOM->rece_end_dly--;
		if (pCOM->overtime_dly)	 pCOM->overtime_dly--;
		if (pCOM->LedFlashCnt)   pCOM->LedFlashCnt--;
	}

	if (Uart[COM_UART1].LedFlashCnt)
	{
		GPIO_WriteBit(GPIOF,GPIO_Pin_0,Bit_SET);
	}
	else
	{
		GPIO_WriteBit(GPIOF,GPIO_Pin_0,Bit_RESET);
	}

#if 0
	if (Uart[COM_UART2].LedFlashCnt)
	{
		GPIO_WriteBit(GPIOB,GPIO_Pin_2,Bit_SET);
	}
	else
	{
		GPIO_WriteBit(GPIOB,GPIO_Pin_2,Bit_RESET);
	}
#endif
}

/*=========================================================================
		串口初始化
/=========================================================================*/

void CommuInitial(void)
{
	U16  i, j;
	UARTBUF *pCOM;
	
	for (i=0; i<MAX_UART; i++)
	{
		pCOM = &Uart[i];
		
		pCOM->rece_cnt = 0;   	/* 接收字节数 */
		pCOM->tras_cnt = 0;	    /* 发送字节数 */
		pCOM->tras_total = 0;	/* 需要发送的总数 */

		pCOM->rece_end_dly = COMMU_RECE_END_DLY;	/* 结束接收延时 */

		pCOM->step = COMMU_IDLE;	/* 通讯状态 */
		pCOM->err = 0;

		pCOM->addr = 0x01;

		pCOM->fail_cnt = 0;
		
		pCOM->commu_error = 0;
		
		pCOM->Bus_Message_Count = 0;
		pCOM->Bus_Overtime_Conut = 0;
		pCOM->Bus_CRC_Error_Count = 0;
		pCOM->Slave_No_Response_Count = 0;
		pCOM->Slave_Message_Count = 0;
		pCOM->Slave_Except_Error_Count = 0;
		
		for (j=0; j<256; j++)  
		{
			pCOM->rece_buf[j] = 0;
			pCOM->tras_buf[j] = 0;
		}

		UartReceStart(i);
	}
}

/***********************************************************************************
  	功能：CRC校验码
  	输入：数据与个数
  	输出：校验码
***********************************************************************************/
U16 CRC16(U08 *data_in, U08 length)
{
	U08  j;
	U16  tmp = 0xffff;
		
	while (length--)	//IS : X16 + X14 + 1
	{
		tmp ^= *data_in++;
		for (j=0; j<8; j++) {
			tmp = (tmp&0x01) ? ((tmp>>1)^0xa001) : (tmp>>1);
		}
	}
	return tmp;
	
}

/*====================================================
	功能: CRC校验
	输出: 0/1  -  败/成
/====================================================*/
U08 CrcCheck(U08 *data_in, U08 length)
{
	U16  crc;
	
	crc = CRC16(data_in, length);
	return (crc == 0x0000);		/* crc校验 */
}


/*==========================================
  	功  能: [读]读写/只读位变量
/==========================================*/
U08 ModbusCommand_01_02(UARTBUF *pCOM)
{
	//U16 Baddr;
	U16 rece_num;

	//Baddr = MAKEWORD(pCOM->rece_buf[2], pCOM->rece_buf[3]);
	rece_num = MAKEWORD(pCOM->rece_buf[4], pCOM->rece_buf[5]);	/* 读取数据的个数 */
	if (pCOM->rece_cnt != 8)	return  MODBUS_ERR_03;
	if (rece_num < 1)			return  MODBUS_ERR_03;
	if (rece_num > 0x007D)		return  MODBUS_ERR_03;

	return MODBUS_NO_ERR;
}

U08 ModbusCommand_05(UARTBUF *pCOM)
{
	//U16 Baddr;
	U16 rece_num;

	//Baddr = MAKEWORD(pCOM->rece_buf[2], pCOM->rece_buf[3]);
	rece_num = MAKEWORD(pCOM->rece_buf[4], pCOM->rece_buf[5]);	/* 读取数据的个数 */
	
	if (pCOM->rece_cnt != 8)  							return  MODBUS_ERR_03;
	if ((rece_num != 0x0000) && (rece_num != 0xff00)) return  MODBUS_ERR_03;

	return MODBUS_NO_ERR;
}

/*==========================================
  	功  能: [写多个]读写位变量
/==========================================*/
U08 ModbusCommand_15(UARTBUF *pCOM)
{
	//U16 Baddr;
	U16 rece_num;

	//Baddr = MAKEWORD(pCOM->rece_buf[2], pCOM->rece_buf[3]);
	rece_num = MAKEWORD(pCOM->rece_buf[4], pCOM->rece_buf[5]);	/* 读取数据的个数 */
			
	if (pCOM->rece_cnt != pCOM->rece_buf[6]+9)  return  MODBUS_ERR_03;
	if (rece_num < 1) 			return  MODBUS_ERR_03;
	if (rece_num > 0x007D/*0x07B0*/)	return  MODBUS_ERR_03;

	return MODBUS_NO_ERR;
}

/*==========================================
  	功  能: [读]读写/只读整型变量
/==========================================*/
U08 ModbusCommand_03_04(UARTBUF *pCOM)
{
	//U16 Baddr;
	U16 rece_num;

	//Baddr = MAKEWORD(pCOM->rece_buf[2], pCOM->rece_buf[3]);
	rece_num = MAKEWORD(pCOM->rece_buf[4], pCOM->rece_buf[5]);	/* 读取数据的个数 */

	if (pCOM->rece_cnt != 8)	return  MODBUS_ERR_03;
	if (rece_num < 1)			return  MODBUS_ERR_03;
	if (rece_num > 0x007D)		return  MODBUS_ERR_03;

	return MODBUS_NO_ERR;
}

/*==========================================
  	功  能: [写单个]读写整型变量
/==========================================*/
U08 ModbusCommand_06(UARTBUF *pCOM)
{
	//U16 Baddr;
	//U16 rece_num;

	//Baddr = MAKEWORD(pCOM->rece_buf[2], pCOM->rece_buf[3]);
	//rece_num = MAKEWORD(pCOM->rece_buf[4], pCOM->rece_buf[5]);	/* 读取数据的个数 */

	if (pCOM->rece_cnt != 8)  
	{
		return MODBUS_ERR_03;
	}

	return MODBUS_NO_ERR;
}

/*==========================================
  	功  能: [写多个]读写整型变量
/==========================================*/
U08 ModbusCommand_16(UARTBUF *pCOM)
{
	//U16 Baddr;
	U16 rece_num;

	//Baddr = MAKEWORD(pCOM->rece_buf[2], pCOM->rece_buf[3]);
	rece_num = MAKEWORD(pCOM->rece_buf[4], pCOM->rece_buf[5]);	/* 读取数据的个数 */

	if (pCOM->rece_cnt != pCOM->rece_buf[6]+9)  
	{
		return  MODBUS_ERR_03;
	}
	
	if (rece_num < 1) 			return  MODBUS_ERR_03;
	if (rece_num > 0x007B) 		return  MODBUS_ERR_03;

	return MODBUS_NO_ERR;
}

/*==========================================
  	回应
/==========================================*/
void ModbusTras(UARTBUF *pCOM)
{
	W16  crc;

	pCOM->tras_buf[0] = pCOM->rece_buf[0];	/* 地址 */
	pCOM->tras_buf[1] = pCOM->rece_buf[1];	/* 功能码 */

	if (pCOM->err == MODBUS_NO_ERR) 		/* 正确应答 */
	{	
		crc.word = CRC16(&pCOM->tras_buf[0], pCOM->tras_total);	/* crc */
		pCOM->tras_buf[pCOM->tras_total++] = crc.byte.low;
		pCOM->tras_buf[pCOM->tras_total++] = crc.byte.high;
	}
	else 									/* 错误回答 */
	{								
		pCOM->Slave_Except_Error_Count++;

		pCOM->tras_buf[1] |= 0x80;
		pCOM->tras_buf[2] = pCOM->err;		/* 错误码 */
		pCOM->tras_total = 3;

		crc.word = CRC16(&pCOM->tras_buf[0], pCOM->tras_total);
		pCOM->tras_buf[pCOM->tras_total++] = crc.byte.low;
		pCOM->tras_buf[pCOM->tras_total++] = crc.byte.high;
	}
}

/*==========================================
  说明： 检查了CRC、地址、功能码
  主机用
/==========================================*/
U08 ModbusRece(UARTBUF *pCOM, U08 addr)
{
	/* 诊断数据帧 */
	pCOM->Bus_Message_Count++;			/* 报文总和 */
	
	if (addr == 0)						/* 广播地址 */
	{
		pCOM->Slave_No_Response_Count++;	
		pCOM->Slave_Message_Count++;
		return  1;
	}
	else 
	{
		if ((pCOM->rece_cnt < 3) || !CrcCheck(&pCOM->rece_buf[0], pCOM->rece_cnt))	/* crc校验失败 */
		{	
	   		pCOM->Bus_CRC_Error_Count++;	/* 报文CRC错误 */
	    	return 0;
	    }

  		if (pCOM->rece_buf[0] != addr) return 0;	/* 地址错误 */

		pCOM->Slave_Message_Count++;
		pCOM->err = MODBUS_NO_ERR;
		return 2;
    }
}

/*==========================================
  说明： 检查了CRC、地址、功能码
  从机用
/==========================================*/
U08 ModbusRequest(UARTBUF *pCOM, U08 Addr)
{
	/* crc校验失败 */
	if ((pCOM->rece_cnt < 3) || !CrcCheck(&pCOM->rece_buf[0], pCOM->rece_cnt)) 
	{	
		pCOM->Bus_CRC_Error_Count++;	/* 报文CRC错误 */

		return  0;
    }

    pCOM->Bus_Message_Count++;		/* 报文总和 */

    if (pCOM->rece_buf[0] == 0) pCOM->Slave_No_Response_Count++;	/* 广播地址 */

	else if (pCOM->rece_buf[0] != Addr)  return 0;	/* 地址错误 */

    pCOM->Slave_Message_Count++;
	pCOM->err = MODBUS_NO_ERR;

    return 1;
}

/*******************************************************************************
* Function Name  : ProtrolPcRece
* Description    : slave protrol
* Input          : uart data
* Output         : None
* Return         : TURE:Success FALSE: Failure
*******************************************************************************/
BOOL ProtrolPcRece(UARTBUF *pCOM)
{
	U16  rece_num;
	U16  Baddr;
	U16  Eaddr;
	U16 i;

    if (!ModbusRequest(pCOM, pCOM->addr))  return FALSE;	/* 不回送 */

    /* 解释 */
   	Baddr = MAKEWORD(pCOM->rece_buf[2], pCOM->rece_buf[3]);
   	rece_num  = MAKEWORD(pCOM->rece_buf[4], pCOM->rece_buf[5]);
	Eaddr = Baddr+rece_num;
	
    switch (pCOM->rece_buf[1])
    {
  
	case MODBUS_01 :
		pCOM->err = ModbusCommand_01_02(pCOM);
    	if (pCOM->err != MODBUS_NO_ERR)	 break;
		/* 具体应用协议 */
				
		pCOM->tras_buf[2] = ((rece_num%8)== 0)?(rece_num /8):((rece_num /8)+1);	/* 数据字节数，至少一个字节 */
		pCOM->tras_total  = 3;

		for (i=0; i<pCOM->tras_buf[2]; i++)
		{
			pCOM->tras_buf[pCOM->tras_total+i] = 0;
		}

		/* 地址判断 *//* 0-1023 */
		if ((Baddr >=MODBUS_ADDR_W_BIT_START) && (Eaddr <= MODBUS_ADDR_W_BIT_END))
		{
			for (i=0; i<rece_num; i++)
			{
				pCOM->tras_buf[pCOM->tras_total + i/8] >>= 1;
				
				if (GETBIT(WRArray[(Baddr+i-MODBUS_ADDR_W_BIT_START)/16].word,(Baddr+i-MODBUS_ADDR_W_BIT_START)%16)) 
				{
					pCOM->tras_buf[pCOM->tras_total + i/8] |= 0x80;
				}
			}
		}
		else
		{
			pCOM->err = MODBUS_ERR_02;	 
			break;
		}

		
		pCOM->tras_total += pCOM->tras_buf[2];
		
		if ((rece_num %8) !=0) 
		{
			rece_num = 8- (rece_num %8);

			pCOM->tras_buf[pCOM->tras_total-1] >>= rece_num;
		}		
		break;
	case MODBUS_02:
		pCOM->err = ModbusCommand_01_02(pCOM);
    	if (pCOM->err != MODBUS_NO_ERR)	 break;
		/* 具体应用协议 */
				
		pCOM->tras_buf[2] = ((rece_num%8)== 0)?(rece_num /8):((rece_num /8)+1);	/* 数据字节数，至少一个字节 */
		pCOM->tras_total  = 3;

		for(i=0; i<pCOM->tras_buf[2]; i++)
		{
			pCOM->tras_buf[pCOM->tras_total+ i] = 0;
		}

		/* 地址判断 *//* 0-1023 */
		if((Baddr >=MODBUS_ADDR_R_BIT_START) && (Eaddr <= MODBUS_ADDR_R_BIT_END))
		{
			for (i=0; i<rece_num; i++)
			{
				pCOM->tras_buf[pCOM->tras_total + i/8] >>= 1;
				
				if (GETBIT(ROArray[(Baddr+i-MODBUS_ADDR_R_BIT_START)/16].word,(Baddr+i-MODBUS_ADDR_R_BIT_START)%16)) 
				{
					pCOM->tras_buf[pCOM->tras_total + i/8] |= 0x80;
				}
			}
		}
		else
		{
			pCOM->err = MODBUS_ERR_02;	 
			break;
		}

		
		pCOM->tras_total += pCOM->tras_buf[2];
		
		if ((rece_num %8) != 0) 
		{
			rece_num = 8- (rece_num %8);

			pCOM->tras_buf[pCOM->tras_total-1] >>= rece_num;
		}		
		break;
    case MODBUS_03 :
    	pCOM->err = ModbusCommand_03_04(pCOM);
    	if (pCOM->err != MODBUS_NO_ERR)	 break;

		pCOM->tras_buf[2] = rece_num << 1;	/* 数据字节数 */
		pCOM->tras_total  = 3;

		if ((Baddr >= MODBUS_ADDR_W_WORD_START) && (Eaddr <= MODBUS_ADDR_W_WORD_END))
		{
			for (i=0; i<rece_num; i++) 
			{
				pCOM->tras_buf[pCOM->tras_total + i*2]  	= HIBYTE(WRArray[Baddr+i-MODBUS_ADDR_W_WORD_START].word);
				pCOM->tras_buf[pCOM->tras_total + i*2+1]	= LOBYTE(WRArray[Baddr+i-MODBUS_ADDR_W_WORD_START].word);
			}
		}
		else
		{
			pCOM->err = MODBUS_ERR_02;	 
			break;
		}
		
		pCOM->tras_total += pCOM->tras_buf[2];
    	break;
    case MODBUS_04 :
    	pCOM->err = ModbusCommand_03_04(pCOM);
    	if (pCOM->err != MODBUS_NO_ERR)	 break;
    	/* 具体应用协议 *//* 只读 */
		pCOM->tras_buf[2] = rece_num << 1;	/* 数据字节数 */
		pCOM->tras_total  = 3;

		if ((Baddr >= MODBUS_ADDR_R_WORD_START) && (Eaddr <= MODBUS_ADDR_R_WORD_END))
		{
			for(i=0;i<rece_num;i++) 
			{
				pCOM->tras_buf[pCOM->tras_total + i*2]  	= HIBYTE(ROArray[Baddr+i-MODBUS_ADDR_R_WORD_START].word);
				pCOM->tras_buf[pCOM->tras_total + i*2+1]	= LOBYTE(ROArray[Baddr+i-MODBUS_ADDR_R_WORD_START].word);
			}
		}
		else
		{
			pCOM->err = MODBUS_ERR_02;	 
			break;
		}

		pCOM->tras_total += pCOM->tras_buf[2];
    	break;
    case MODBUS_05 :
    	pCOM->err = ModbusCommand_05(pCOM);
    	if (pCOM->err != MODBUS_NO_ERR)	 break;

		/* 地址判断 *//* 0-1023 */
		if((Baddr >=MODBUS_ADDR_W_BIT_START) && (Baddr < MODBUS_ADDR_W_BIT_END))
		{
			if (rece_num == 0x0000) 
			{
				CLRBIT(WRArray[(Baddr-MODBUS_ADDR_W_BIT_START)/16].word,(Baddr-MODBUS_ADDR_W_BIT_START)%16);	
			}
			else if(rece_num == 0xff00) 
			{
				SETBIT(WRArray[(Baddr-MODBUS_ADDR_W_BIT_START)/16].word,(Baddr-MODBUS_ADDR_W_BIT_START)%16);			
			}
		}
		else
		{
			pCOM->err = MODBUS_ERR_02;	 
			break;
		}
		
		pCOM->tras_buf[2] = pCOM->rece_buf[2];
		pCOM->tras_buf[3] = pCOM->rece_buf[3];
		pCOM->tras_buf[4] = pCOM->rece_buf[4];
		pCOM->tras_buf[5] = pCOM->rece_buf[5];
		pCOM->tras_total = 6;
    	break;
    case MODBUS_06 :
		//01 06 00 00 00 ff c9 8a
		//01 01 00 00 00 0a bc 0d
    	pCOM->err = ModbusCommand_06(pCOM);
    	if (pCOM->err != MODBUS_NO_ERR)	 break;

		if((Baddr >= MODBUS_ADDR_W_WORD_START) && (Baddr < MODBUS_ADDR_W_WORD_END))
		{
			WRArray[Baddr-MODBUS_ADDR_W_WORD_START].word = rece_num;
		}
		else
		{
			pCOM->err = MODBUS_ERR_02;	 
			break;
		}
		
		pCOM->tras_buf[2] = pCOM->rece_buf[2];
		pCOM->tras_buf[3] = pCOM->rece_buf[3];
		pCOM->tras_buf[4] = pCOM->rece_buf[4];
		pCOM->tras_buf[5] = pCOM->rece_buf[5];
		pCOM->tras_total = 6;
    	break;
    case MODBUS_0F :
    	pCOM->err = ModbusCommand_15(pCOM);
    	if (pCOM->err != MODBUS_NO_ERR)	 break;

		/* 地址判断 *//* 0-1023 */
		if ((Baddr >=MODBUS_ADDR_W_BIT_START) && (Eaddr <= MODBUS_ADDR_W_BIT_END))
		{
			for (i=0; i<rece_num; i++) 
			{
				if (GETBITBYTE(pCOM->rece_buf[7+i/8],i%8)) 
				{
					SETBIT(ROArray[(Baddr+i-MODBUS_ADDR_W_BIT_START)/16].word,(Baddr+i-MODBUS_ADDR_W_BIT_START)%16);
				}
				else 
				{
					CLRBIT(ROArray[(Baddr+i-MODBUS_ADDR_W_BIT_START)/16].word,(Baddr+i-MODBUS_ADDR_W_BIT_START)%16);
				}			
			}
		}
		else
		{
			pCOM->err = MODBUS_ERR_02;	 
			break;
		}

		pCOM->tras_buf[2] = pCOM->rece_buf[2];
		pCOM->tras_buf[3] = pCOM->rece_buf[3];
		pCOM->tras_buf[4] = pCOM->rece_buf[4];
		pCOM->tras_buf[5] = pCOM->rece_buf[5];
		pCOM->tras_total = 6;
    	break;
    case MODBUS_10:
    	pCOM->err = ModbusCommand_16(pCOM);
    	if (pCOM->err != MODBUS_NO_ERR)	 break;

		if ((Baddr >= MODBUS_ADDR_W_WORD_START) && (Eaddr <= MODBUS_ADDR_W_WORD_END))
		{
			for (i=0; i<rece_num; i++) 
			{
				WRArray[Baddr+i-MODBUS_ADDR_W_WORD_START].word = MAKEWORD(pCOM->rece_buf[7+i*2], pCOM->rece_buf[7+i*2+1]);
			}
		}
		else
		{
			pCOM->err = MODBUS_ERR_02;	 
			break;
		}
		
		pCOM->tras_buf[2] = pCOM->rece_buf[2];
		pCOM->tras_buf[3] = pCOM->rece_buf[3];
		pCOM->tras_buf[4] = pCOM->rece_buf[4];
		pCOM->tras_buf[5] = pCOM->rece_buf[5];
		pCOM->tras_total = 6;
    	break;
    default :
    	break;
    }

    if  (pCOM->rece_buf[0] == 0) return FALSE;	/* 如果是广播, 则不需要回送 */
	
    /* 回送 */
    ModbusTras(pCOM);

    return  TRUE;
}

