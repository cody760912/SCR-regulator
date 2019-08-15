/******************** (C) COPYRIGHT 2008 Sendy *********************************
* File Name          : AdTemp.h
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
#ifndef __ADTEMP_H
#define __ADTEMP_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_type.h"


/* Exported types ------------------------------------------------------------*/

#define MAX_AD_NUM  1

extern U16 TempValue[MAX_AD_NUM];	/* ÎÂ¶ÈÖµ */
extern U16 FgTempErr[MAX_AD_NUM];

#define  CurVoltage		TempValue[0]

/* Exported constants --------------------------------------------------------*/


/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void DealGetVoltage(void);

#endif /* __ADTEMP_H */

/******************* (C) COPYRIGHT 2009 SENDY *****END OF FILE****/





