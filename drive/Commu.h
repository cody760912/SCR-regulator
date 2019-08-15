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

enum COMMU_STATUS	/* 通信状态字 */
{	
    COMMU_IDLE, 						/* 空闲	*/
    COMMU_START, 						/* 开始 */
    COMMU_WAIT, 						/* 等待 */
    COMMU_RECE, 						/* 接收 */
    COMMU_TRAS,							/* 发送 */
    
    COMMU_ALARM, 						/* 报警 */
    COMMU_END,  						/* 结束 */
};


typedef struct tagUART
{
    U08  rece_cnt;   					/* 已接收的字节数 */
    U08  tras_cnt;	    				/* 已发送的字节数 */
    U08  tras_total;					/* 需发送的字节数 */
	U08  rece_total;					/* 需接收的字节数 */

    U08  rece_end_dly;					/* 结束接收延时 */
    U16  overtime_dly; 					/* 接收超时 */

	U08  LedFlashCnt;					/* 用于通讯指示灯 */

    U08  step;							/* 通讯状态 */
	u8	 addr;							/* 地址 */
    U08  err;							/* 异常码 */

	U16  fail_cnt;						/* 通讯失败重复累计 */
	
    U16  commu_error;					/* 最大连接16块板 */

    /* 通讯记录结构 诊断信息================= */
    U16  Bus_Message_Count;				/* 返回总线报文计数 CRC 不正确地报文，不在计数范围 */
	U16  Bus_Overtime_Conut;			/* 超时计数 */
	U16  Bus_CRC_Error_Count;			/* 返回总线通信错误计数 CRC错误 或 报文小于3个字节 */
	U16  Slave_No_Response_Count;  		/* 返回子节点无响应计数 即广播数据 */
	U16  Slave_Message_Count;			/* 返回子节点报文计数 包括广播 */
	U16  Slave_Except_Error_Count;		/* 返回子节点异常错误计数 */
	
	U08  rece_buf[256];					/* 接收缓冲区 */
    U08  tras_buf[256];					/* 发送缓冲区 */    

} UARTBUF;


/* Exported constants --------------------------------------------------------*/

#define MODBUS_01       	0x01		/* MODBUS功能码 */
#define MODBUS_02       	0x02
#define MODBUS_03       	0x03
#define MODBUS_04       	0x04
#define MODBUS_05       	0x05
#define MODBUS_06       	0x06
#define MODBUS_08       	0x08
#define MODBUS_0F       	0x0f
#define MODBUS_10       	0x10
#define MODBUS_17       	0x17


#define COMMU_RECE_END_DLY		6		/* 接收结束延时 */
#define COMMU_RECE_WAIT_DLY		20		/* 最长等待接收时间 */
#define COMMU_OVER_TIME			40		/* 最长通讯时间 */
#define COMMU_FAIL_CNT_MAX		3		/* 最大失败次数 */


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


/* 通讯数据信息，与应用层数据联系紧密(可做为队列节点中的数据域) */
typedef struct tagCOMMSG
{
    U08 driverNo;			/* 通讯设备的地址 */
    U08 cmd;				/* 命令 */
    U16 startAddr;			/* 通讯协议中读写起始地址 */
    U16 length;				/* 读写数据长度 */
    U16 *pData;				/* 发送或接收数据地址(位bit时，bit0为起始地址的数据) */
    U16 oneWrVal;			/* 写一个数据时，可使用该变量，不使用pData */
} COMMSG;
//#define oneWrVal length		/* 为节省空间，写一个数据时，使用length，不使用pData */

/* 队列节点 */
typedef struct tagQNODE
{
    U08 next;				/* 该节点后的下一个节点 */
    U08 driverType;			/* 通讯设备的类型 */
    COMMSG data;			/* 节点数据域 */
} QNODE;


#define DRIVER_DGUS		1	/* 迪文DGUS */

#define Q_PRIO_HIGH		0	/* 高优先级 */
#define Q_PRIO_LOW		1	/* 低优先级 */
//#define MAX_NODE_NUM 	10	/* 队列中最大的节点数目 */
#define MAX_NODE_NUM 	30	/* 队列中最大的节点数目(与迪文屏通讯节点数较多) */

#define MAX_MB_WORD_LEN	120	/* 最大的modbus字访问长度(如，后板串口缓冲区定义较少) */

/* 通信队列 */
typedef struct tagSENDQUEUE
{
	U08 head;				/* 队列首 */
	U08 end;				/* 队列尾 */
	U08 cnt;				/* 队列节点个数 */
    U08 insertPos;			/* 高优先优插入位置 */
    QNODE node[MAX_NODE_NUM];/* 队列节点数据 */
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

