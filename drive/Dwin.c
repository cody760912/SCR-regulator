/*******************************************************************************
 Copyright  All rights reserved.
 �ļ�����: Dwin.c
 �ļ�����: ����DGUS��ͨѶģ��ʵ���ļ�
 ������  : 
 ��������: 2017.06.05 
 �汾����:
*******************************************************************************/
#define DWIN_GLOBAL
#include "stm32f0xx.h"
#include "stdio.h"
#include "math.h"
#include "work.h"
#include "commu.h"
#include "dwin.h"

static SENDQUEUE DwinQueue;	/* ͨѶ���У�����ʹ�ö��з��ͣ��ӻ���ʹ�� */
extern UARTBUF Uart[MAX_UART];			/* ͨѶ������ */

DWINSETPSWD  SetPswdData;

DWINSETNUM SetNum;


/* ע��:�������ʱ�䶨�峬��255ʱ���轫��Ӧ�ı�������ΪU16 */
#define DWINCOM_TIME_INTERVAL		1		/* *2 ͨѶ�����ʱʱ��1*2ms=2ms */
#define DWINCOM_TIME_RX_FRAME		4		/* *2 һ֡�������ʱ�䣬�����2*2msδ�յ���һ�ֽ���Ϊ������� */
#define DWINCOM_TIME_TX_DLY			4		/* *2 ������ʱʱ�䣬����485����оƬΪ����״̬����ʱ���ٷ������� */
#define DWINCOM_TIME_LED			20		/* *2 LED����˸ʱ�� */

/* DWINָ�� */
#define DWIN_WR_REG 0X80
#define DWIN_WR_RAM 0X82
#define DWIN_RD_RAM 0X83

#define DWIN_FRAME_HEADER_1 0XAA
#define DWIN_FRAME_HEADER_2 0X55

static U16 KeyDwin = K_NULL;


#define DWIN_RX_OVERTIME 2000 /* ���ճ�ʱʱ�䣬��λ���룬2�� */

														
static BOOL HaveRxOk;															

static void DWIN_QInit(void);
static BOOL DWIN_QSendNodeData(void);
static void DWIN_QNextNode(void);
static BOOL DWIN_TxProtocol(UARTBUF *pCOM, const COMMSG *pComMsg);
static BOOL DWIN_RxProtocol(UARTBUF *pCOM);


/*******************************************************************************
	����Ϊ������·�㺯���������������֮�ϣ���Ӳ�����
*******************************************************************************/

/*******************************************************************************
 �� ��: DWIN_UartRxInt()
 �� ��: ���ڽ����ж�
 �� ��: ��
 �� ��: ��
 �� ��: ��
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
 �� ��: DWIN_UartTxInt()
 �� ��: ���ڷ����ж�
 �� ��: ��
 �� ��: ��
 �� ��: ��
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
 �� ��: DWIN_Init()
 �� ��: ��ʼ��ͨѶ
 �� ��: ��
 �� ��: ��
 �� ��: ��
*******************************************************************************/
void DWIN_Init(void)
{
	UARTBUF *pCOM;
	
	pCOM = &Uart[COM_UART1];
	
	/* ��ȫ����ʼ��Ϊ0, �ٳ�ʼ������������� */
	
	pCOM->addr = 0x01;
	pCOM->step = COMMU_IDLE;
	
	pCOM->commu_error = 0;
	
	/* ��ʼ�����Ͷ��� */
	DWIN_QInit();
}

/*******************************************************************************
 �� ��: DWIN_CommuProg()
 �� ��: ȫ˫��ͨѶ����
 �� ��: �� 
 �� ��: ��
 �� ��: 
*******************************************************************************/
void DWIN_CommuProg(void)
{
	UARTBUF *pCOM;	

	pCOM = &Uart[COM_UART1];
	
	/* ���ʹ��� */
	switch (pCOM->step)
	{		
		case COMMU_IDLE:	/* ���Ͷ����д����ͽڵ㣬ת������ʱ */		
			if (DWIN_QSendNodeData()) 
			{									
				pCOM->fail_cnt = 0;
				pCOM->step = COMMU_WAIT;
				pCOM->overtime_dly = DWINCOM_TIME_TX_DLY;
			}
			break;

		case COMMU_WAIT:	/* ��ʱʱ�䵽��ת�������� */
			if ((pCOM->overtime_dly==0))
			{
				pCOM->step = COMMU_START;
			}
			else
			{
				break;
			}
		
		case COMMU_START:	/* д���ͻ������Ĵ������������� */
			pCOM->tras_cnt = 0;

			UartTrasStart(COM_UART1);
			pCOM->step = COMMU_TRAS;
			break;

		case COMMU_TRAS:		/* ���� */
			if (pCOM->tras_cnt >= pCOM->tras_total)
			{
				DWIN_QNextNode();		/* ָ���Ͷ�����һ���ڵ� */
				pCOM->step = COMMU_IDLE;
				pCOM->LedFlashCnt = DWINCOM_TIME_LED;
				pCOM->overtime_dly = DWINCOM_TIME_INTERVAL;	
			}			
			break;
			
		case COMMU_ALARM:	/* �����Զ���λ��ת���� */
			pCOM->step = COMMU_IDLE;
			break;
			
		default :
			pCOM->step = COMMU_IDLE;
			break;
	}

	/* ���մ��� */
	/* һ֡�������:����֡ʱ����δ�յ���һ���ݻ���ջ������� */
	if ((pCOM->rece_cnt>0 && pCOM->rece_end_dly==0)
		|| (pCOM->rece_cnt >= 255)) 
	{	 
//		DWIN_DisableRece();			/* �������֡ʱ�رս���ʹ�� */
		//USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);

		//DWIN_WriteOneRAM(DWIN_ADDR_DATA_TIME, pCOM->rece_buf[8]);

		if (DWIN_RxProtocol(pCOM)) 
		{
			pCOM->LedFlashCnt = DWINCOM_TIME_LED;
//			pCOM->time.commIntProt = DWINCOM_TIME_PROTECT;
			pCOM->commu_error = 0;			/* �Զ���λ */    
		}
		else		/* ����֡ʱ��׼��������һ֡ */
		{				
			UartReceStart(COM_UART1);
		}
		
		pCOM->rece_cnt = 0;			/* ����һ֡���ݺ�����ռ����� */
	}	
}

/*******************************************************************************
 �� ��: DWIN_WaitOver()
 �� ��: �ȴ������ڵ�ͨѶ���ݶ�ͨѶ���
 �� ��: ��
 �� ��: TRUE/FALSE--ͨѶ�ɹ��˳�/ͨѶ�����˳�
 �� ��: ��
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
	����Ϊ����㺯�������ڲ���׼Э�����
*******************************************************************************/

/*******************************************************************************
 �� ��: DWIN_QEmpty()
 �� ��: �жϷ��Ͷ����Ƿ�Ϊ��
 �� ��: �� 
 �� ��: TRUE/FALSE--���Ͷ���Ϊ��/�ǿ�
 �� ��: ��
*******************************************************************************/
BOOL DWIN_QEmpty(void)		
{	
	return  (DwinQueue.cnt == 0);			
}

/*******************************************************************************
 �� ��: DWIN_QFull()
 �� ��: �жϷ��Ͷ����Ƿ�Ϊ��
 �� ��: �� 
 �� ��: TRUE/FALSE--���Ͷ���Ϊ��/����
 �� ��: ��
*******************************************************************************/
BOOL DWIN_QFull(void)		
{	
//	return  (DwinQueue.cnt == MAX_NODE_NUM);	
	return  (DwinQueue.cnt >= MAX_NODE_NUM);	/* COM-V100A0-1 */
}

/*******************************************************************************
 �� ��: DWIN_QGetNodeCnt()
 �� ��: ��ȡ���Ͷ�����Ч����
 �� ��: �� 
 �� ��: ���Ͷ�����Ч����
 �� ��: ��
*******************************************************************************/
U08 DWIN_QGetNodeCnt(void)				
{	
	return  DwinQueue.cnt;					
}

/*******************************************************************************
 �� ��: DWIN_QInit()
 �� ��: ��ʼ�����Ͷ���
 �� ��: �� 
 �� ��: ��
 �� ��: ��
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
 �� ��: DWIN_QNextNode()
 �� ��: ָ���Ͷ�����һ���ڵ�
 �� ��: �� 
 �� ��: ��
 �� ��: ��
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
 �� ��: DWIN_QInsertNode()
 �� ��: ������в���һ���ڵ�
 �� ��: prio--���ȼ� Q_PRIO_LOW/Q_PRIO_HIGH; 
		pComMessage--ͨѶ�ڵ�ָ��; 
 �� ��: TRUE/FALSE--�ɹ�/ʧ��
 �� ��: ��
*******************************************************************************/
BOOL DWIN_QInsertNode(U08 prio, const QNODE *pNode)
{
	U08 no;
	SENDQUEUE *pq;

	pq = &DwinQueue;

	if (DWIN_QEmpty())		/* ���п� */
	{
		no = pq->end;
	}
	else if (DWIN_QFull())	/* ������ */
	{
		if (prio == Q_PRIO_HIGH)			/* �����ȼ� */
		{		
			if (pq->insertPos == pq->end)  
			{
				return FALSE;				/* ȫ�����Ǹ����ȼ������� */
			}

			no = pq->node[pq->insertPos].next;
			pq->insertPos = no;
		}
		else 
		{					/* �����ȼ� */
			return FALSE;
		}
	}
	else
	{
		if (prio == Q_PRIO_HIGH)			/* �����ȼ� */
		{ 		
			if (pq->insertPos == pq->end)	/* ������ȫ�����Ǹ����ȼ��� */
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
		{					/* �����ȼ� */
			no = pq->node[pq->end].next;
			pq->end = no;
		}
	}

	pq->node[no].driverType = pNode->driverType;
	pq->node[no].data = pNode->data;
	
/*==============================================================================
COM-V100A0-0
1.���������жϣ����в���ʱ�����нڵ�������ż�1
2.��ǰ��û�������жϵģ��������ʱ�����:
  ���������Ǹ����ȼ�����(������ĳ����֧����)�������Ƚڵ�Ḳ�ǵ����ȼ��ڵ㣬
  �������ж�ʱ�����нڵ���������1�����Ѵ������Ľڵ���(MAX_NODE_NUM)��
  �´��ٲ���һ�������ȼ����DWIN_QFull(u)�᷵��FALSE��
  ���ջᵼ�²���Ľڵ����ݳ�������Ľڵ����鷶Χ�������⸲�ǽڵ�������涨��ı�����
==============================================================================*/
//	pq->cnt++;	
	if (!DWIN_QFull())
	{
		pq->cnt++;
	}
	
	return TRUE;
}

/*******************************************************************************
	����Ϊ����㺯��(ͨѶЭ���)����ͨ�Ŷ������
*******************************************************************************/

/*******************************************************************************
 �� ��: DWIN_QAcceptNodeData()
 �� ��: �ӽ��ջ��������սڵ�����
 �� ��: �� 
 �� ��: TRUE/FALSE--����������ȷ/����
 �� ��: ��
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
 �� ��: DWIN_CopyMemForModbusType()
 �� ��: �洢�ռ临�ƣ���Դ����������Modbus����(��8λ��ǰ����8λ�ں�)
 �� ��: pDst--Ŀ������ָ��; 
		pSrc--Դ����ָ��; 
		size--�ֽڴ�С(����Ϊż��); 
 �� ��: ��
 �� ��: ��
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
 �� ��: DWIN_QSendNodeData()
 �� ��: ���ͽڵ����������ͻ�����
 �� ��: �� 
 �� ��: TRUE/FALSE--�÷��ͻ������ɹ�/ʧ��
 �� ��: ��
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
	    /* ����ʧ��ʱ�����ýڵ㣬ָ���¸��ڵ� */
	    DWIN_QNextNode();
	    return FALSE;		
	}
}

/*******************************************************************************
 �� ��: DWIN_TxProtocol()
 �� ��: ����DGUS����Э�飬�����нڵ�����ת���ɴ�������(���÷��ͻ�����)
 �� ��: pCOM--���ڽṹ��ָ��; 
		pComMsg--ͨѶ����ָ��; 
 �� ��: TRUE/FALSE--���ͳɹ�/����ʧ��
 �� ��: ��
*******************************************************************************/
static BOOL DWIN_TxProtocol(UARTBUF *pCOM, const COMMSG *pComMsg)
{	
	U08 len;
	W16 crc;
	
	/* ����֡ͷ */
	pCOM->tras_buf[0] = DWIN_FRAME_HEADER_1; 
	pCOM->tras_buf[1] = DWIN_FRAME_HEADER_2;

	/* �������ݳ��ȡ�ָ����� */
	switch (pComMsg->cmd)
	{
		case DWIN_WR_REG:
			len = (U08)pComMsg->length;
			/* ���� */
			pCOM->tras_buf[2] = 2 + len;
			/* ָ�� */
			pCOM->tras_buf[3] = DWIN_WR_REG;
			/* ��ַ */
			pCOM->tras_buf[4] = (U08)pComMsg->startAddr;
			/* ���� */
			
			pCOM->tras_buf[5] = HIBYTE(pComMsg->oneWrVal);

			pCOM->tras_buf[6] = LOBYTE(pComMsg->oneWrVal);
			
			/* �������� */
			pCOM->tras_total = 5 + len;
			break;
			
		case DWIN_WR_RAM:
			len = (U08)pComMsg->length*2;
			/* ���� */
			pCOM->tras_buf[2] = 3 + len;
			/* ָ�� */
			pCOM->tras_buf[3] = DWIN_WR_RAM;
			/* ��ַ */
			pCOM->tras_buf[4] = HIBYTE(pComMsg->startAddr);
			pCOM->tras_buf[5] = LOBYTE(pComMsg->startAddr);
			/* ���� */
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
			/* �������� */
			pCOM->tras_total = 6 + len;			
			break;
		case DWIN_RD_RAM:
			/* ���� */
			pCOM->tras_buf[2] = 4;
			/* ָ�� */
			pCOM->tras_buf[3] = DWIN_RD_RAM;
			/* ��ַ */
			pCOM->tras_buf[4] = HIBYTE(pComMsg->startAddr);
			pCOM->tras_buf[5] = LOBYTE(pComMsg->startAddr);
			/* �����ݳ��� */
			pCOM->tras_buf[6] = (U08)pComMsg->length;
			/* �������� */
			pCOM->tras_total = 7;					
			break;
		default:
			break;
	}

	/* ����֡β(crc) */
	crc.word = CRC16(&pCOM->tras_buf[3], pCOM->tras_buf[2]);/* ָ������ݵ�crc */
	pCOM->tras_buf[2] += 2;/* ���ݳ�������2���ֽ� */
	pCOM->tras_buf[pCOM->tras_total++] = crc.byte.low;
	pCOM->tras_buf[pCOM->tras_total++] = crc.byte.high;	

	return  TRUE;
}


/*******************************************************************************
 �� ��: DWIN_TxProtocol()
 �� ��: ����DGUS����Э�飬�ӽ��ջ������н�������
 �� ��: pCOM--���ڽṹ��ָ��; 
 �� ��: TRUE/FALSE--���ճɹ�/ʧ��
 �� ��: ��
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
	
	/* ����ƥ���������ʧ�� */


	/* �����������:��RAM */
	if (pCOM->rece_buf[3] != DWIN_RD_RAM)
	{
		return FALSE;
	}
	address = MAKEWORD(pCOM->rece_buf[4], pCOM->rece_buf[5]);
	val = MAKEWORD(pCOM->rece_buf[7], pCOM->rece_buf[8]);
	
	switch (address)
	{
		case DWIN_ADDR_KEY:/* �����ذ���ֵ */
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
	����ΪӦ�ò㺯�����ṩģ��ӿ�
*******************************************************************************/

/*******************************************************************************
 �� ��: DWIN_WriteOneRAM()
 �� ��: дһ��RAM
 �� ��: startAddr--��ʼ��ַ; 
		val--дֵ; 
 �� ��: TRUE/FALSE--�ɹ�/ʧ��
*******************************************************************************/
BOOL DWIN_WriteRAM(U16 startAddr, U08 Length, U16 *pData )
{
	QNODE node; 

	node.driverType = DRIVER_DGUS;
	node.data.driverNo = 1;
	node.data.cmd = DWIN_WR_RAM;
	node.data.startAddr = startAddr;
	node.data.length = Length;
	node.data.pData = pData;/* ��Ϊ��ָ��������DWIN_WriteRAM() */
	
	return DWIN_QInsertNode(Q_PRIO_HIGH, &node);
}


/*******************************************************************************
 �� ��: DWIN_WriteOneRAM()
 �� ��: дһ��RAM
 �� ��: startAddr--��ʼ��ַ; 
		val--дֵ; 
 �� ��: TRUE/FALSE--�ɹ�/ʧ��
*******************************************************************************/
BOOL DWIN_WriteOneRAM(U16 startAddr, U16 val)
{
	QNODE node;	

	node.driverType = DRIVER_DGUS;
	node.data.driverNo = 1;
	node.data.cmd = DWIN_WR_RAM;
	node.data.startAddr = startAddr;
	node.data.length = 1;
	node.data.pData = NULL;/* ��Ϊ��ָ��������DWIN_WriteRAM() */
	node.data.oneWrVal = val;
	
	return DWIN_QInsertNode(Q_PRIO_HIGH, &node);
}

/*******************************************************************************
 �� ��: DWIN_WriteOneREG()
 �� ��: дһ��REG
 �� ��: startAddr--��ʼ��ַ; 
		val--дֵ; 
 �� ��: TRUE/FALSE--�ɹ�/ʧ��
*******************************************************************************/
BOOL DWIN_WriteOneREG(U16 startAddr, U16 val)
{
	QNODE node;	

	node.driverType = DRIVER_DGUS;
	node.data.driverNo = 1;
	node.data.cmd = DWIN_WR_REG;
	node.data.startAddr = startAddr;
	node.data.length = 2;
	node.data.pData = NULL;/* ��Ϊ��ָ��������DWIN_WriteReg() */
	node.data.oneWrVal = val;
	
	return DWIN_QInsertNode(Q_PRIO_HIGH, &node);
}

/*******************************************************************************
 �� ��: DWIN_ReadRAM()
 �� ��: ���Ĵ���
 �� ��: startAddr--��ʼ��ַ; 
		length--����; 
	    //pData--����ָ��; 
 �� ��: TRUE/FALSE--�ɹ�/ʧ��
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
 �� ��: DWIN_ReadKey()
 �� ��: ��ȡ���İ���
 �� ��: ��
 �� ��: ���İ���
 �� ��: ��
*******************************************************************************/
U16 DWIN_ReadKey(void)
{
	U16 i;
	
	i = KeyDwin;
	KeyDwin = K_NULL;
	return i;
}


/*******************************************************************************
 �� ��: DWIN_SetVal()
 �� ��: �����ص����ݸ���
 �� ��: address--�������ݵ�ַ; 
 		val--ֵ;
 �� ��: ����
 �� ��: ��
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



