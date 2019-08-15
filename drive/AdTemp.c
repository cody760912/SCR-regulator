/******************** u-Sendy ********************
* File Name          : work.c
* Author             : Sendy
* Version            : V1.0
* Date               : 09/20/2014
* Description        : Adtemp program body
********************************************************************************

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stdio.h"
#include "uet_gpio.h"
#include "work.h"
#include "math.h"
#include "adtemp.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

#define VOL_CONTRAST_R		(4990L+1000L)	/* 并联分压电阻值 */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

U16 TempValue[MAX_AD_NUM];	/* 电压值 */
U16 FgTempErr[MAX_AD_NUM];	/* 故障 */

extern BIT16 ROArray[];
extern BIT16 WRArray[];
/* Private function prototypes -----------------------------------------------*/


/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : DealGetVoltage
* Description    : deal get voltage from ad 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DealGetVoltage(void)
{
	static U16 RunTime = 0;
	static U32 SunAdData[MAX_AD_NUM] = {0};

	static U16 FilterTemp[MAX_AD_NUM][10];
	static U16 FilterNumb = 0;

	static U32 FilterTime = 0;

	U16 i;
	U16 j;

	if (GetSystemTime()- FilterTime >= 10)
	{
		FilterTime = GetSystemTime();

		for (i=0; i<MAX_AD_NUM; i++)
		{
			SunAdData[i] += ADData(i);		/* 进行数据累加 */

			if (ADData(i) > 4000)			/* 故障判断 */
			{
				FgTempErr[i] = 1;
			}
			else if (ADData(i) <= 2000)
			{
				FgTempErr[i] = 2;
			}
			else
			{
				FgTempErr[i] = 0;
			}
		}

		if (++RunTime >= 16)
		{
			U32 TempR = 0;
			
			for (i=0; i<MAX_AD_NUM; i++)
			{
				if (i==0)
				{
					TempR = ((((U32)SunAdData[i]*VOL_CONTRAST_R/0x0ffff)*33)/100);

					FilterTemp[i][FilterNumb] = (U16)TempR;

					TempValue[i] = 0;

					for (j=0; j<10; j++)
					{
						TempValue[i] += FilterTemp[i][j];
					}

					TempValue[i] = TempValue[i]/100+PARA_Vol;
				}
				else
				{
					TempR = ((U32)SunAdData[i]*33000/0x0fff0);

					if (i == 1)
					{
						FilterTemp[i][FilterNumb] = (U16)TempR/331;
					}
					else
					{
						FilterTemp[i][FilterNumb] = (U16)TempR/11;
					}

					TempValue[i] = 0;

					for (j=0; j<10; j++)
					{
						TempValue[i] += FilterTemp[i][j];
					}

					TempValue[i] = TempValue[i]/100;
				}

				SunAdData[i] = 0;
			}

			RunTime = 0;

			if (++FilterNumb >= 10)
			{
				FilterNumb = 0;
			}
		}
	}
}


