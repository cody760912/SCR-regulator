/*******************************************************************************
 Copyright  All rights reserved.
 文件名称: Dwin.c
 文件描述: 迪文DGUS屏通讯模块实现文件
 创建人  : 
 创建日期: 2017.06.05 
 版本更改:
*******************************************************************************/
#define DWIN_GLOBAL
#include "stm32f0xx.h"
#include "stdio.h"
#include "math.h"
#include "work.h"
#include "commu.h"
#include "dwin.h"

static SENDQUEUE DwinQueue;	/* 通讯队列，主机使用队列发送，从机不使用 */
extern UARTBUF Uart[MAX_UART];			/* 通讯缓冲区 */

DWINSETPSWD  SetPswdData;

DWINSETNUM SetNum;


/* 注意:当下面的时间定义超过255时，需将相应的变量更新为U16 */
#define DWINCOM_TIME_INTERVAL		1		/* *2 通讯间隔延时时间1*2ms=2ms */
#define DWINCOM_TIME_RX_FRAME		4		/* *2 一帧接收完成时间，即如果2*2ms未收到下一字节认为接收完成 */
#define DWINCOM_TIME_TX_DLY			4		/* *2 发送延时时间，先置485控制芯片为发送状态，延时后再发送数据 */
#define DWINCOM_TIME_LED			20		/* *2 LED灯闪烁时间 */

/* DWIN指令 */
#define DWIN_WR_REG 0X80
#define DWIN_WR_RAM 0X82
#define DWIN_RD_RAM 0X83

#define DWIN_FRAME_HEADER_1 0XAA
#define DWIN_FRAME_HEADER_2 0X55

static U16 KeyDwin = K_NULL;


#define DWIN_RX_OVERTIME 2000 /* 接收超时时间，单位毫秒，2秒 */

														
static BOOL HaveRxOk;															

static void DWIN_QInit(void);
static BOOL DWIN_QSendNodeData(void);
static void DWIN_QNextNode(void);
static BOOL DWIN_TxProtocol(UARTBUF *pCOM, const COMMSG *pComMsg);
static BOOL DWIN_RxProtocol(UARTBUF *pCOM);


/*******************************************************************************
	以下为数据链路层函数处理，基于物理层之上，与硬件相关
*******************************************************************************/

/*******************************************************************************
 函 数: DWIN_UartRxInt()
 功 能: 串口接收中断
 参 数: 空
 返 回: 空
 其 它: 空
*******************************************************************************/
void DWIN_UartRx(void) 
{
	UARTBUF *pCOM;
	
//	DISABLE_IRQ();
	
	pCOM = &Uart[COM_UART1];

  	if (pCOM->rece_cnt < COMMU_RECE_MAX_NUM) 
	{
		pCOM->rece_end_dly = DWINCOM_TIME_RX_FRAME;
		
		pCOM->rece_buf[pCOM->rece_cnt++] = (U08)USART_ReceiveData(USART1);
	}

//	ENABLE_IRQ();
}

/*******************************************************************************
 函 数: DWIN_UartTxInt()
 功 能: 串口发送中断
 参 数: 空
 返 回: 空
 其 它: 空
*******************************************************************************/
void DWIN_UartTx(void) 
{
	UARTBUF *pCOM;
//	volatile U08 receData;
	
//	DISABLE_IRQ();
	
	pCOM = &Uart[COM_UART1];
	  	
	pCOM->tras_cnt++;	
	
	if (pCOM->tras_cnt < pCOM->tras_total)
	{
		USART_SendData(USART1,pCOM->tras_buf[pCOM->tras_cnt]);

		if (pCOM->tras_cnt == pCOM->tras_total-1)	/* send last one close txe interrupt */
		{
			USART_ITConfig(USART1,USART_IT_TXE,DISABLE);

			USART_ITConfig(USART1,USART_IT_TC,ENABLE);
		}
	}
	else
	{
		USART_ITConfig(USART1,USART_IT_TXE,DISABLE);
	}
	
//	ENABLE_IRQ();
}


/*******************************************************************************
 函 数: DWIN_Init()
 功 能: 初始化通讯
 参 数: 空
 返 回: 空
 其 它: 空
*******************************************************************************/
void DWIN_Init(void)
{
	UARTBUF *pCOM;
	
	pCOM = &Uart[COM_UART1];
	
	/* 先全部初始化为0, 再初始化其它特殊变量 */
	
	pCOM->addr = 0x01;
	pCOM->step = COMMU_IDLE;
	
	pCOM->commu_error = 0;
	
	/* 初始化发送队列 */
	DWIN_QInit();
}

/*******************************************************************************
 函 数: DWIN_CommuProg()
 功 能: 全双工通讯处理
 参 数: 空 
 返 回: 空
 其 它: 
*******************************************************************************/
void DWIN_CommuProg(void)
{
	UARTBUF *pCOM;	

	pCOM = &Uart[COM_UART1];
	
	/* 发送处理 */
	switch (pCOM->step)
	{		
		case COMMU_IDLE:	/* 发送队列有待发送节点，转发送延时 */		
			if (DWIN_QSendNodeData()) 
			{									
				pCOM->fail_cnt = 0;
				pCOM->step = COMMU_WAIT;
				pCOM->overtime_dly = DWINCOM_TIME_TX_DLY;
			}
			break;

		case COMMU_WAIT:	/* 延时时间到，转启动发送 */
			if ((pCOM->overtime_dly==0))
			{
				pCOM->step = COMMU_START;
			}
			else
			{
				break;
			}
		
		case COMMU_START:	/* 写发送缓冲区寄存器，启动发送 */
			pCOM->tras_cnt = 0;

			UartTrasStart(COM_UART1);
			pCOM->step = COMMU_TRAS;
			break;

		case COMMU_TRAS:		/* 发送 */
			if (pCOM->tras_cnt >= pCOM->tras_total)
			{
				DWIN_QNextNode();		/* 指向发送队列下一个节点 */
				pCOM->step = COMMU_IDLE;
				pCOM->LedFlashCnt = DWINCOM_TIME_LED;
				pCOM->overtime_dly = DWINCOM_TIME_INTERVAL;	
			}			
			break;
			
		case COMMU_ALARM:	/* 故障自动复位，转空闲 */
			pCOM->step = COMMU_IDLE;
			break;
			
		default :
			pCOM->step = COMMU_IDLE;
			break;
	}

	/* 接收处理 */
	/* 一帧接收完成:接收帧时间内未收到下一数据或接收缓冲区满 */
	if ((pCOM->rece_cnt>0 && pCOM->rece_end_dly==0)
		|| (pCOM->rece_cnt >= 255)) 
	{	 
//		DWIN_DisableRece();			/* 处理接收帧时关闭接收使能 */
		//USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);

		//DWIN_WriteOneRAM(DWIN_ADDR_DATA_TIME, pCOM->rece_buf[8]);

		if (DWIN_RxProtocol(pCOM)) 
		{
			pCOM->LedFlashCnt = DWINCOM_TIME_LED;
//			pCOM->time.commIntProt = DWINCOM_TIME_PROTECT;
			pCOM->commu_error = 0;			/* 自动复位 */    
		}
		else		/* 错误帧时，准备接收下一帧 */
		{				
			UartReceStart(COM_UART1);
		}
		
		pCOM->rece_cnt = 0;			/* 处理一帧数据后，清接收计数器 */
	}	
}

/*******************************************************************************
 函 数: DWIN_WaitOver()
 功 能: 等待队列内的通讯数据都通讯完成
 参 数: 空
 返 回: TRUE/FALSE--通讯成功退出/通讯故障退出
 其 它: 空
*******************************************************************************/
BOOL DWIN_WaitOver(void)
{
	while (1)
	{	
		//WDG_Refresh();
		DWIN_CommuProg();

		if (DWIN_QEmpty()/* && DWIN_Idle(u)*/)
		{
			return TRUE;
		}		
	}	
}

/*******************************************************************************
	以下为传输层函数，与内部标准协议相关
*******************************************************************************/

/*******************************************************************************
 函 数: DWIN_QEmpty()
 功 能: 判断发送队列是否为空
 参 数: 空 
 返 回: TRUE/FALSE--发送队列为空/非空
 其 它: 空
*******************************************************************************/
BOOL DWIN_QEmpty(void)		
{	
	return  (DwinQueue.cnt == 0);			
}

/*******************************************************************************
 函 数: DWIN_QFull()
 功 能: 判断发送队列是否为满
 参 数: 空 
 返 回: TRUE/FALSE--发送队列为满/非满
 其 它: 空
*******************************************************************************/
BOOL DWIN_QFull(void)		
{	
//	return  (DwinQueue.cnt == MAX_NODE_NUM);	
	return  (DwinQueue.cnt >= MAX_NODE_NUM);	/* COM-V100A0-1 */
}

/*******************************************************************************
 函 数: DWIN_QGetNodeCnt()
 功 能: 获取发送队列有效个数
 参 数: 空 
 返 回: 发送队列有效个数
 其 它: 空
*******************************************************************************/
U08 DWIN_QGetNodeCnt(void)				
{	
	return  DwinQueue.cnt;					
}

/*******************************************************************************
 函 数: DWIN_QInit()
 功 能: 初始化发送队列
 参 数: 空 
 返 回: 空
 其 它: 空
*******************************************************************************/
static void DWIN_QInit(void)
{
	U08 i;
	SENDQUEUE *pq;
	
	pq = &DwinQueue;
	
	pq->head = 0;
	pq->end = 0;
	pq->cnt = 0;
	pq->insertPos = 0;
	for (i=0; i<MAX_NODE_NUM; i++)
	{
		pq->node[i].next = i+1;
	}
}

/*******************************************************************************
 函 数: DWIN_QNextNode()
 功 能: 指向发送队列下一个节点
 参 数: 空 
 返 回: 空
 其 它: 空
*******************************************************************************/
static void DWIN_QNextNode(void)
{
	U08 newHead;
	SENDQUEUE *pq;

	pq = &DwinQueue;
	if (pq->cnt == 0)
	{
		return;
	}
	
	if (pq->cnt > 1) 
	{
		newHead = pq->node[pq->head].next;

		pq->node[pq->head].next = pq->node[pq->end].next;
		pq->node[pq->end].next = pq->head;

		if (pq->insertPos == pq->head) 
		{
			pq->insertPos = newHead;
		}
		
		pq->head = newHead;
	}
	pq->cnt--;
}

/*******************************************************************************
 函 数: DWIN_QInsertNode()
 功 能: 向队列中插入一个节点
 参 数: prio--优先级 Q_PRIO_LOW/Q_PRIO_HIGH; 
		pComMessage--通讯节点指针; 
 返 回: TRUE/FALSE--成功/失败
 其 它: 空
*******************************************************************************/
BOOL DWIN_QInsertNode(U08 prio, const QNODE *pNode)
{
	U08 no;
	SENDQUEUE *pq;

	pq = &DwinQueue;

	if (DWIN_QEmpty())		/* 队列空 */
	{
		no = pq->end;
	}
	else if (DWIN_QFull())	/* 队列满 */
	{
		if (prio == Q_PRIO_HIGH)			/* 高优先级 */
		{		
			if (pq->insertPos == pq->end)  
			{
				return FALSE;				/* 全部都是高优先级队列了 */
			}

			no = pq->node[pq->insertPos].next;
			pq->insertPos = no;
		}
		else 
		{					/* 低优先级 */
			return FALSE;
		}
	}
	else
	{
		if (prio == Q_PRIO_HIGH)			/* 高优先级 */
		{ 		
			if (pq->insertPos == pq->end)	/* 队列里全部都是高优先级的 */
			{	
				no = pq->node[pq->end].next;
				pq->end = no;
				pq->insertPos = no;
			}
			else 
			{
				no = pq->node[pq->end].next;
				pq->node[pq->end].next = pq->node[no].next;

				pq->node[no].next = pq->node[pq->insertPos].next;
				pq->node[pq->insertPos].next = no;
				pq->insertPos = no;
			}
		}
		else 
		{					/* 低优先级 */
			no = pq->node[pq->end].next;
			pq->end = no;
		}
	}

	pq->node[no].driverType = pNode->driverType;
	pq->node[no].data = pNode->data;
	
/*==============================================================================
COM-V100A0-0
1.增加条件判断，队列不满时，队列节点计数器才加1
2.以前是没有条件判断的，下述情况时会出错:
  队列满且是高优先级命令(见上面的程序分支处理)，高优先节点会覆盖低优先级节点，
  无条件判断时，队列节点计数器会加1，即已大于最大的节点数(MAX_NODE_NUM)；
  下次再插入一个低优先级命令，DWIN_QFull(u)会返回FALSE；
  最终会导致插入的节点数据超过定义的节点数组范围，即意外覆盖节点数组后面定义的变量。
==============================================================================*/
//	pq->cnt++;	
	if (!DWIN_QFull())
	{
		pq->cnt++;
	}
	
	return TRUE;
}

/*******************************************************************************
	以下为网络层函数(通讯协议层)，与通信对象相关
*******************************************************************************/

/*******************************************************************************
 函 数: DWIN_QAcceptNodeData()
 功 能: 从接收缓冲区接收节点数据
 参 数: 空 
 返 回: TRUE/FALSE--接收数据正确/错误
 其 它: 空
*******************************************************************************/
/*
static BOOL DWIN_QAcceptNodeData(void)
{
	QNODE *pNode;
	UART *pCOM;
	
	pNode = &DwinQueue.node[DwinQueue.head];
	pCOM = &DwinUart;	
	return TRUE;
}
*/

/*******************************************************************************
 函 数: DWIN_CopyMemForModbusType()
 功 能: 存储空间复制，且源数据类似于Modbus类型(高8位在前，低8位在后)
 参 数: pDst--目的数据指针; 
		pSrc--源数据指针; 
		size--字节大小(必须为偶数); 
 返 回: 空
 其 它: 空
*******************************************************************************/
void DWIN_CopyMemForModbusType(U08 *pDst, const U08 *pSrc, U16 size)
{
	if (size%2 == 0)
	{
		while (size != 0)
		{
//			*pDst++ = *(pSrc+1);	
//			*pDst++ = *pSrc;		
			*pDst++ = (*(W16 *)pSrc).byte.high;	
			*pDst++ = (*(W16 *)pSrc).byte.low;	
			pSrc += 2;
			size -= 2;
		}	
	}
}

/*******************************************************************************
 函 数: DWIN_QSendNodeData()
 功 能: 发送节点数据至发送缓冲区
 参 数: 空 
 返 回: TRUE/FALSE--置发送缓冲区成功/失败
 其 它: 空
*******************************************************************************/
static BOOL DWIN_QSendNodeData(void)
{
	QNODE *pNode;
	UARTBUF *pCOM;
    
    if (DWIN_QEmpty()) 
    {
    	return FALSE;
    }
       
    pNode = &DwinQueue.node[DwinQueue.head];
    pCOM = &Uart[COM_UART1];
    
	if (DWIN_TxProtocol(pCOM, &pNode->data)) 
	{
		return TRUE;
	}	
	else
	{
	    /* 发送失败时丢弃该节点，指向下个节点 */
	    DWIN_QNextNode();
	    return FALSE;		
	}
}

/*******************************************************************************
 函 数: DWIN_TxProtocol()
 功 能: 处理DGUS发送协议，将队列节点数据转换成串口数据(如置发送缓冲区)
 参 数: pCOM--串口结构体指针; 
		pComMsg--通讯数据指针; 
 返 回: TRUE/FALSE--发送成功/发送失败
 其 它: 空
*******************************************************************************/
static BOOL DWIN_TxProtocol(UARTBUF *pCOM, const COMMSG *pComMsg)
{	
	U08 len;
	W16 crc;
	
	/* 发送帧头 */
	pCOM->tras_buf[0] = DWIN_FRAME_HEADER_1; 
	pCOM->tras_buf[1] = DWIN_FRAME_HEADER_2;

	/* 发送数据长度、指令、数据 */
	switch (pComMsg->cmd)
	{
		case DWIN_WR_REG:
			len = (U08)pComMsg->length;
			/* 长度 */
			pCOM->tras_buf[2] = 2 + len;
			/* 指令 */
			pCOM->tras_buf[3] = DWIN_WR_REG;
			/* 地址 */
			pCOM->tras_buf[4] = (U08)pComMsg->startAddr;
			/* 数据 */
			
			pCOM->tras_buf[5] = HIBYTE(pComMsg->oneWrVal);

			pCOM->tras_buf[6] = LOBYTE(pComMsg->oneWrVal);
			
			/* 发送总数 */
			pCOM->tras_total = 5 + len;
			break;
			
		case DWIN_WR_RAM:
			len = (U08)pComMsg->length*2;
			/* 长度 */
			pCOM->tras_buf[2] = 3 + len;
			/* 指令 */
			pCOM->tras_buf[3] = DWIN_WR_RAM;
			/* 地址 */
			pCOM->tras_buf[4] = HIBYTE(pComMsg->startAddr);
			pCOM->tras_buf[5] = LOBYTE(pComMsg->startAddr);
			/* 数据 */
			if (pComMsg->pData != NULL)
			{
				DWORD TmpData;
				
				DWIN_CopyMemForModbusType(&pCOM->tras_buf[6], (U08 *)pComMsg->pData, len);

				TmpData = (*(U32 *)pComMsg->pData);

				pCOM->tras_buf[6] =  (U08)(TmpData >> 24);
				pCOM->tras_buf[7] = (U08)(TmpData >> 16);
				pCOM->tras_buf[8] =  (U08)(TmpData >> 8);
				pCOM->tras_buf[9] = (U08)(TmpData);
			}
			else
			{
				pCOM->tras_buf[6] = HIBYTE(pComMsg->oneWrVal);
				pCOM->tras_buf[7] = LOBYTE(pComMsg->oneWrVal);
			}
			/* 发送总数 */
			pCOM->tras_total = 6 + len;			
			break;
		case DWIN_RD_RAM:
			/* 长度 */
			pCOM->tras_buf[2] = 4;
			/* 指令 */
			pCOM->tras_buf[3] = DWIN_RD_RAM;
			/* 地址 */
			pCOM->tras_buf[4] = HIBYTE(pComMsg->startAddr);
			pCOM->tras_buf[5] = LOBYTE(pComMsg->startAddr);
			/* 读数据长度 */
			pCOM->tras_buf[6] = (U08)pComMsg->length;
			/* 发送总数 */
			pCOM->tras_total = 7;					
			break;
		default:
			break;
	}

	/* 发送帧尾(crc) */
	crc.word = CRC16(&pCOM->tras_buf[3], pCOM->tras_buf[2]);/* 指令和数据的crc */
	pCOM->tras_buf[2] += 2;/* 数据长度增加2个字节 */
	pCOM->tras_buf[pCOM->tras_total++] = crc.byte.low;
	pCOM->tras_buf[pCOM->tras_total++] = crc.byte.high;	

	return  TRUE;
}


/*******************************************************************************
 函 数: DWIN_TxProtocol()
 功 能: 处理DGUS接收协议，从接收缓冲区中接收数据
 参 数: pCOM--串口结构体指针; 
 返 回: TRUE/FALSE--接收成功/失败
 其 它: 空
*******************************************************************************/
static BOOL DWIN_RxProtocol(UARTBUF *pCOM)
{
	U16 address;
	U16 val;

	if (pCOM->rece_buf[0] == 'O' && pCOM->rece_buf[1] == 'K')			/* GD008AX33009_003 */
	{
		HaveRxOk = TRUE;
		return TRUE;		
	}
	
	/* 主机匹配接收数据失败 */


	/* 处理接收数据:读RAM */
	if (pCOM->rece_buf[3] != DWIN_RD_RAM)
	{
		return FALSE;
	}
	address = MAKEWORD(pCOM->rece_buf[4], pCOM->rece_buf[5]);
	val = MAKEWORD(pCOM->rece_buf[7], pCOM->rece_buf[8]);
	
	switch (address)
	{
		case DWIN_ADDR_KEY:/* 处理返回按键值 */
			KeyDwin = val;
			break;
		case DWIN_ADDR_DEBUG_PASSWORD:
		case DWIN_ADDR_TERM_PSWD:
		case DWIN_ADDR_CUR_PSWD:
		case DWIN_ADDR_USE_TIMES0_PSWD:
		case DWIN_ADDR_USE_TIMES1_PSWD:
		case DWIN_ADDR_USE_TIMES2_PSWD:
		case DWIN_ADDR_USE_TIMES3_PSWD:
		case DWIN_ADDR_USE_TIMES4_PSWD:
		case DWIN_ADDR_USE_TIMES5_PSWD:
		case DWIN_ADDR_USE_TIMES6_PSWD:
		case DWIN_ADDR_USE_TIMES7_PSWD:
		case DWIN_ADDR_USE_TIMES8_PSWD:
		case DWIN_ADDR_USE_TIMES9_PSWD:
			SetPswdData.addr = address;
			SetPswdData.update  = TRUE;
			SetPswdData.val = val;
			SetPswdData.val <<= 16;
			val = MAKEWORD(pCOM->rece_buf[9], pCOM->rece_buf[10]);
			SetPswdData.val |= val;
			break;
		default:
			DWIN_SetVal(address, val);
			break;
	}
	
	return  TRUE;
}

/*******************************************************************************
	以下为应用层函数，提供模块接口
*******************************************************************************/

/*******************************************************************************
 函 数: DWIN_WriteOneRAM()
 功 能: 写一个RAM
 参 数: startAddr--起始地址; 
		val--写值; 
 返 回: TRUE/FALSE--成功/失败
*******************************************************************************/
BOOL DWIN_WriteRAM(U16 startAddr, U08 Length, U16 *pData )
{
	QNODE node; 

	node.driverType = DRIVER_DGUS;
	node.data.driverNo = 1;
	node.data.cmd = DWIN_WR_RAM;
	node.data.startAddr = startAddr;
	node.data.length = Length;
	node.data.pData = pData;/* 设为空指针来区分DWIN_WriteRAM() */
	
	return DWIN_QInsertNode(Q_PRIO_HIGH, &node);
}


/*******************************************************************************
 函 数: DWIN_WriteOneRAM()
 功 能: 写一个RAM
 参 数: startAddr--起始地址; 
		val--写值; 
 返 回: TRUE/FALSE--成功/失败
*******************************************************************************/
BOOL DWIN_WriteOneRAM(U16 startAddr, U16 val)
{
	QNODE node;	

	node.driverType = DRIVER_DGUS;
	node.data.driverNo = 1;
	node.data.cmd = DWIN_WR_RAM;
	node.data.startAddr = startAddr;
	node.data.length = 1;
	node.data.pData = NULL;/* 设为空指针来区分DWIN_WriteRAM() */
	node.data.oneWrVal = val;
	
	return DWIN_QInsertNode(Q_PRIO_HIGH, &node);
}

/*******************************************************************************
 函 数: DWIN_WriteOneREG()
 功 能: 写一个REG
 参 数: startAddr--起始地址; 
		val--写值; 
 返 回: TRUE/FALSE--成功/失败
*******************************************************************************/
BOOL DWIN_WriteOneREG(U16 startAddr, U16 val)
{
	QNODE node;	

	node.driverType = DRIVER_DGUS;
	node.data.driverNo = 1;
	node.data.cmd = DWIN_WR_REG;
	node.data.startAddr = startAddr;
	node.data.length = 2;
	node.data.pData = NULL;/* 设为空指针来区分DWIN_WriteReg() */
	node.data.oneWrVal = val;
	
	return DWIN_QInsertNode(Q_PRIO_HIGH, &node);
}

/*******************************************************************************
 函 数: DWIN_ReadRAM()
 功 能: 读寄存器
 参 数: startAddr--起始地址; 
		length--长度; 
	    //pData--数据指针; 
 返 回: TRUE/FALSE--成功/失败
*******************************************************************************/
BOOL DWIN_ReadRAM(U16 startAddr, U08 length/*, U16 *pData*/)
{
	QNODE node;	

	node.driverType = DRIVER_DGUS;
	node.data.driverNo = 1;
	node.data.cmd = DWIN_RD_RAM;
	node.data.startAddr = startAddr;
	node.data.length = length;
//	node.data.pData = pData;
	node.data.pData = NULL;
	
	return DWIN_QInsertNode(Q_PRIO_HIGH, &node);
}

/*******************************************************************************
 函 数: DWIN_ReadKey()
 功 能: 获取迪文按键
 参 数: 空
 返 回: 迪文按键
 其 它: 空
*******************************************************************************/
U16 DWIN_ReadKey(void)
{
	U16 i;
	
	i = KeyDwin;
	KeyDwin = K_NULL;
	return i;
}


/*******************************************************************************
 函 数: DWIN_SetVal()
 功 能: 屏返回的数据更新
 参 数: address--迪文数据地址; 
 		val--值;
 返 回: 子项
 其 它: 空
*******************************************************************************/
void DWIN_SetVal(U16 address, U16 val)
{
	if (address == DWIN_ADDR_USE_TIMES0_V_N)
   	 {
        		SetNum.update = TRUE;
	    	SetNum.val = val;
	    	SetNum.addr = address;
    	}
	else if (address == DWIN_ADDR_ICON_BODY)
	{
		//ProgQueryNoDwinSetNum.update = TRUE;
		//ProgQueryNoDwinSetNum.val = val;
	}
	//else if ((DWIN_ADDR_RTC_YEAR <= address) && (address <= DWIN_ADDR_RTC_SEC))
	//{
		//RTC_DwinSet(address, val);
	//}

	SetNum.update = TRUE;
	SetNum.val = val;
	SetNum.addr = address;
}



