/******************** (C) COPYRIGHT 2008 Sendy *********************************
* File Name          : work.h
* Author             : Sendy
* Version            : V1.0
* Date               : 04/01/2010
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
#ifndef __WORK_H
#define __WORK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_type.h"


/* Exported types ------------------------------------------------------------*/
typedef union tagBIT8
{
	unsigned char byte;
	struct{ 
		unsigned char b0:1;
		unsigned char b1:1;
		unsigned char b2:1;
		unsigned char b3:1;
		unsigned char b4:1;
		unsigned char b5:1;
		unsigned char b6:1;
		unsigned char b7:1;
	} bit;
} BIT8;

typedef union tagBIT16
{
	unsigned short word;
	struct {
		unsigned char low;
		unsigned char high; 
	} byte;
	struct{ 
		unsigned short b0:1;
		unsigned short b1:1;
		unsigned short b2:1;
		unsigned short b3:1;
		unsigned short b4:1;
		unsigned short b5:1;
		unsigned short b6:1;
		unsigned short b7:1;
		unsigned short b8:1;
		unsigned short b9:1;
		unsigned short b10:1;
		unsigned short b11:1;
		unsigned short b12:1;
		unsigned short b13:1;
		unsigned short b14:1;
		unsigned short b15:1;
	} bit;
} BIT16;

typedef union tagW16 {
	unsigned int word;
	struct {
		unsigned char low;
		unsigned char high; 
	} byte;
} W16;

typedef u8 U08;
typedef u16 U16;
typedef s16 S16;
typedef u32 U32;
typedef s32 S32;
typedef bool BOOL;

typedef unsigned long long U64;

typedef u8 BYTE;
typedef u16 WORD;
typedef u32 DWORD;

typedef struct tagIOGroup
{
	GPIO_TypeDef* GPIOx;
	u16 GPIO_Pin;
}IOData;

typedef struct tagPasswordGroup
{
	U16 FgUse;
	U16 SetTimes;
	U32 PassWord;
}PASSData;


/* Exported constants --------------------------------------------------------*/

#define NULL     		0
#define PNULL    		((PVOID)0)

//#define TRUE    		1
//#define FALSE   		0

#define ON				1
#define OFF				0

#define F_ON			0
#define F_OFF			1

#define BIT_HIGH		1
#define BIT_LOW			0
#define BYTE_HIGH		0xFF
#define BYTE_LOW		0x00

#define BIT_0 			0x1
#define BIT_1 			0x2
#define BIT_2 			0x4
#define BIT_3 			0x8
#define BIT_4 			0x10
#define BIT_5 			0x20
#define BIT_6 			0x40
#define BIT_7 			0x80
              			
#define BIT_8 			0x100
#define BIT_9 			0x200
#define BIT_10			0x400
#define BIT_11			0x800
#define BIT_12			0x1000
#define BIT_13			0x2000
#define BIT_14			0x4000
#define BIT_15			0x8000
              			
#define BIT_16			0x10000L
#define BIT_17			0x20000L
#define BIT_18			0x40000L
#define BIT_19			0x80000L
#define BIT_20			0x100000L
#define BIT_21			0x200000L
#define BIT_22			0x400000L
#define BIT_23			0x800000L
              			
#define BIT_24			0x1000000L
#define BIT_25			0x2000000L
#define BIT_26			0x4000000L
#define BIT_27			0x8000000L
#define BIT_28			0x10000000L
#define BIT_29			0x20000000L
#define BIT_30			0x40000000L
#define BIT_31			0x80000000L




/* Exported macro ------------------------------------------------------------*/
#define SETBIT(x, y)		(x |= (0x0001 << (y)))
#define CLRBIT(x, y)		(x &= ~(0x0001 << (y)))
#define GETBIT(x, y)		(((x) >> (y)) & 0x0001)

#define SETBITBYTE(x, y)	(x |= (0x01 << (y)))
#define CLRBITBYTE(x, y)	(x &= ~(0x01 << (y)))
#define GETBITBYTE(x, y)	(((x) >> (y)) & 0x01)

#define SWAPWORD(x)         (u16)((((x) & 0xFF) << 8) | (((x) >> 8) & 0xFF))
#define MAKEWORD(b, a)      ((u16) (((u8)(a)) | ((u16)((u8)(b))) << 8))

#define LO4BIT(x)			((u8)(x) & 0x0f)
#define HI4BIT(x)			((u8)(x) >> 4)

#define LOBYTE(x)           (u8)(x)
#define HIBYTE(x)           (u8)((u16)(x) >> 8)


#define ADData(x)		(ROArray[20+x].word)

#define CMD_START       WRArray[50].word
#define CMD_STOP		WRArray[51].word

#define MAX_TIMES_DATA	50

extern PASSData  UseTimesData[MAX_TIMES_DATA];


typedef struct tagKey
{
	WORD Value;
	BYTE Status;
	BYTE Cnt;
}KEYSTATUS;


#define KEY_ENTER  	1	// °´ÏÂ
#define KEY_RELAX  	0	// ËÉ¿ª

#define KEY_NULL 		0
#define KEY_DOWN		1
#define KEY_UP			2
#define KEY_CONTINUE	3

#define KEY_SET_VALUE     	0x0002
#define KEY_UP_VALUE		0x0001
#define KEY_DOWN_VALUE		0x0004

#define KEY_ENTER_VALUE     0x1000

#define KEY_EXIT_VALUE		0x0210



/* Exported functions ------------------------------------------------------- */


U32 GetSystemTime(void);
void DealStepOutputCom(void);

#endif /* __WORK_H */

/******************* (C) COPYRIGHT 2009 SENDY *****END OF FILE****/




