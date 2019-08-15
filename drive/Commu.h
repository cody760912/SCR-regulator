/******************** (C) COPYRIGHT 2008 Sendy *********************************
* File Name          : commu.h
* Author             : Sendy
* Version            : V1.0
* Date               : 08/14/2009
* Description        : Header file for commu.s.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED 
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMU_H
#define __COMMU_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_type.h"

#include "work.h"

/* Exported types ------------------------------------------------------------*/

enum COMMU_STATUS	/* ͨ��״̬�� */
{	
    COMMU_IDLE, 						/* ����	*/
    COMMU_START, 						/* ��ʼ */
    COMMU_WAIT, 						/* �ȴ� */
    COMMU_RECE, 						/* ���� */
    COMMU_TRAS,							/* ���� */
    
    COMMU_ALARM, 						/* ���� */
    COMMU_END,  						/* ���� */
};


typedef struct tagUART
{
    U08  rece_cnt;   					/* �ѽ��յ��ֽ��� */
    U08  tras_cnt;	    				/* �ѷ��͵��ֽ��� */
    U08  tras_total;					/* �跢�͵��ֽ��� */
	U08  rece_total;					/* ����յ��ֽ��� */

    U08  rece_end_dly;					/* ����������ʱ */
    U16  overtime_dly; 					/* ���ճ�ʱ */

	U08  LedFlashCnt;					/* ����ͨѶָʾ�� */

    U08  step;							/* ͨѶ״̬ */
	u8	 addr;							/* ��ַ */
    U08  err;							/* �쳣�� */

	U16  fail_cnt;						/* ͨѶʧ���ظ��ۼ� */
	
    U16  commu_error;					/* �������16��� */

    /* ͨѶ��¼�ṹ �����Ϣ================= */
    U16  Bus_Message_Count;				/* �������߱��ļ��� CRC ����ȷ�ر��ģ����ڼ�����Χ */
	U16  Bus_Overtime_Conut;			/* ��ʱ���� */
	U16  Bus_CRC_Error_Count;			/* ��������ͨ�Ŵ������ CRC���� �� ����С��3���ֽ� */
	U16  Slave_No_Response_Count;  		/* �����ӽڵ�����Ӧ���� ���㲥���� */
	U16  Slave_Message_Count;			/* �����ӽڵ㱨�ļ��� �����㲥 */
	U16  Slave_Except_Error_Count;		/* �����ӽڵ��쳣������� */
	
	U08  rece_buf[256];					/* ���ջ����� */
    U08  tras_buf[256];					/* ���ͻ����� */    

} UARTBUF;


/* Exported constants --------------------------------------------------------*/

#define MODBUS_01       	0x01		/* MODBUS������ */
#define MODBUS_02       	0x02
#define MODBUS_03       	0x03
#define MODBUS_04       	0x04
#define MODBUS_05       	0x05
#define MODBUS_06       	0x06
#define MODBUS_08       	0x08
#define MODBUS_0F       	0x0f
#define MODBUS_10       	0x10
#define MODBUS_17       	0x17


#define COMMU_RECE_END_DLY		6		/* ���ս�����ʱ */
#define COMMU_RECE_WAIT_DLY		20		/* ��ȴ�����ʱ�� */
#define COMMU_OVER_TIME			40		/* �ͨѶʱ�� */
#define COMMU_FAIL_CNT_MAX		3		/* ���ʧ�ܴ��� */


#define COM_UART1				0
#define COM_UART2				1


//======================================================================
#define IDLE 					0
#define BUSY 					1

#define SELECT					0
#define NO_SELECT 				1

#define PWR_ON 					0
#define PWR_OFF 				1

#define COM_BUSY 				0XE1
#define COM_SUCESS 				0XE2
#define COM_TIME_OUT 			0XE3
#define VOID_DATA 				0X55


#define MAX_UART			2


#define MODBUS_READ_ONLY_NUM		200

#define MODBUS_WRITE_NUM			200

#define COMMU_RECE_MAX_NUM 			255


/* ͨѶ������Ϣ����Ӧ�ò�������ϵ����(����Ϊ���нڵ��е�������) */
typedef struct tagCOMMSG
{
    U08 driverNo;			/* ͨѶ�豸�ĵ�ַ */
    U08 cmd;				/* ���� */
    U16 startAddr;			/* ͨѶЭ���ж�д��ʼ��ַ */
    U16 length;				/* ��д���ݳ��� */
    U16 *pData;				/* ���ͻ�������ݵ�ַ(λbitʱ��bit0Ϊ��ʼ��ַ������) */
    U16 oneWrVal;			/* дһ������ʱ����ʹ�øñ�������ʹ��pData */
} COMMSG;
//#define oneWrVal length		/* Ϊ��ʡ�ռ䣬дһ������ʱ��ʹ��length����ʹ��pData */

/* ���нڵ� */
typedef struct tagQNODE
{
    U08 next;				/* �ýڵ�����һ���ڵ� */
    U08 driverType;			/* ͨѶ�豸������ */
    COMMSG data;			/* �ڵ������� */
} QNODE;


#define DRIVER_DGUS		1	/* ����DGUS */

#define Q_PRIO_HIGH		0	/* �����ȼ� */
#define Q_PRIO_LOW		1	/* �����ȼ� */
//#define MAX_NODE_NUM 	10	/* ���������Ľڵ���Ŀ */
#define MAX_NODE_NUM 	30	/* ���������Ľڵ���Ŀ(�������ͨѶ�ڵ����϶�) */

#define MAX_MB_WORD_LEN	120	/* ����modbus�ַ��ʳ���(�磬��崮�ڻ������������) */

/* ͨ�Ŷ��� */
typedef struct tagSENDQUEUE
{
	U08 head;				/* ������ */
	U08 end;				/* ����β */
	U08 cnt;				/* ���нڵ���� */
    U08 insertPos;			/* �������Ų���λ�� */
    QNODE node[MAX_NODE_NUM];/* ���нڵ����� */
} SENDQUEUE;


/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void CommuInitial(void);
void UartTimer(void);
void UartReceStart(U08 u);
void UartTrasStart(u8 u);
U08 COM_UartSlave(u8 u);
U16 CRC16(U08 *data_in, U08 length);
U08 CrcCheck(U08 *data_in, U08 length);

#endif /* __COMMU_H */

/******************* (C) COPYRIGHT 2009 SENDY *****END OF FILE****/

