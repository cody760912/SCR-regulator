/******************** u-Sendy ********************
* File Name          : work.c
* Author             : Sendy
* Version            : V1.0
* Date               : 03/17/2011
* Description        : Work program body
********************************************************************************

V050  2017.03.06
1.初始版本

V051	2017.11.19	lgc
	1.更改在第一种模式的情况下,24V输出为开800MS,关200MS.

V051	2017.11.19	lgc
	1.更改在第一种模式的情况下,24V输出为开500MS,关200MS.

V052    2018.10.25  lgc
    1.开机时步进输出为5毫秒脉宽.正常工作时,步进输出为20毫秒脉宽.

V053    2018.10.27  lgc
    1.步进电机后退时按5MS脉冲运行.
	2.更改运行周期为480MS,以前为500毫秒.
	3.更改运行周期为460MS.
	3.更改运行周期为440MS.

V054    2018.12.26  lgc
    1.更改运行周期为470MS.

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stdio.h"
#include "math.h"
#include "work.h"
#include "commu.h"
#include "dwin.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
/* Private define ------------------------------------------------------------*/
#define FLASH_PAGE_SIZE         ((DWORD)0x00000400)   /* FLASH Page Size */
#define FLASH_USER_START_ADDR   ((DWORD)0x08007000)   /* Start @ of user Flash area */

#define FLASH_TERM_START_ADDR   ((DWORD)0x08007400)	/* Start @ of term Flash area */

#define FLASH_USER_END_ADDR     ((DWORD)0x08008000)   /* End @ of user Flash area */
#define DATA_32                 ((DWORD)0x12345678)


DWORD EraseCounter = 0x00;
DWORD Address = 0x00;
DWORD Data = 0x3210ABCD;
DWORD NbrOfPage = 0x00;
__IO FLASH_Status FLASHStatus = FLASH_COMPLETE;
__IO TestStatus MemoryProgramStatus = PASSED;

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#define MAX_M_OUT_NUM 	(7)

IOData MOutputIO[MAX_M_OUT_NUM] = 
{	
	{GPIOC, GPIO_Pin_15},	// MO1
	{GPIOC, GPIO_Pin_14},	// 
	{GPIOC, GPIO_Pin_13},
	{GPIOB, GPIO_Pin_9},
	{GPIOB, GPIO_Pin_8},
	{GPIOF, GPIO_Pin_0},
	{GPIOB, GPIO_Pin_3},	
};

#define MAX_INPUT_NUM 	(3)

IOData BAUDIO[MAX_INPUT_NUM] = 
{
	{GPIOA, GPIO_Pin_11},	// BUSY	
	{GPIOB, GPIO_Pin_10},	// IN1
	{GPIOB, GPIO_Pin_2},	// IN2
};


#define MAX_LED_NUM_COM 	(2)

IOData LEDIO[MAX_LED_NUM_COM] = 
{
	{GPIOA, GPIO_Pin_15},
	{GPIOB, GPIO_Pin_4},
};


#define MAX_STEP_NUM_COM 	(3)

IOData STEPIO[MAX_STEP_NUM_COM] = 
{
	{GPIOB, GPIO_Pin_12},	//ENABLE
	{GPIOB, GPIO_Pin_14},	//DIR
	{GPIOB, GPIO_Pin_13},	//STEP
};

#define COM_STEP_OUT		(2)

#define MAX_SPK_NUM_COM 	(1)

IOData SPKIO[MAX_LED_NUM_COM] = 
{
	{GPIOB, GPIO_Pin_4},
};

#define EEPROM_PARA_ADDR	10

///////////////////////////////////////////////////////////////////////////////



#define LED_LIGHT	0
#define LED_CLOSE   1
#define LED_CLOSE_ALL	0xff


///////////////////////////////////////////////////////////////////////////////

//#define MODBUS_READ_ONLY_NUM		200

//#define MODBUS_WRITE_NUM			200

BIT16 ROArray[MODBUS_READ_ONLY_NUM];

BIT16 WRArray[MODBUS_WRITE_NUM];


////////////////////////////////////////////////////////////////////////////////


#define VERSION_DATA	ROArray[1].word


/* read only area 2~9为开关量输入 */
#define INPUT_DATA_ADDR (2)
#define INPUTDATA(x)    (ROArray[INPUT_DATA_ADDR+(x)].word)	/* 开关量输入 */

#define IN_BUSY			(ROArray[INPUT_DATA_ADDR].word)		/* 忙 */
#define IN_FRONT_LIMIT	(ROArray[INPUT_DATA_ADDR+2].word)	/* 向上 */
#define IN_BACK_LIMIT	(ROArray[INPUT_DATA_ADDR+1].word)	/* 向下 */



#define InputBuffer		(ROArray[15].word)

/* 地址20做为AD值 
#define ADData(x)		(ROArray[20+x].word)
*/

#define MachineStatus	ROArray[50].word /* 系统状态 */

#define DisplayStatus	ROArray[60].word /* 显示状态 */


#define DWKey			ROArray[70].word /* 显示状态 */

#define FgStartFirst			ROArray[80].word /* 用于计数 */


//////////////////////////////////////////////////////////////////////////////////
//读写区

#define OUT_24V			(WRArray[0].word)
#define OUT_VALVE_A		(WRArray[1].word)
#define OUT_VALVE_B		(WRArray[2].word)
#define OUT_VALVE_A_BAK	(WRArray[3].word)
#define OUT_VALVE_B_BAK	(WRArray[4].word)
#define OUT_24V_BAK		(WRArray[5].word)
#define OUT_RELAY		(WRArray[6].word)


#define OUT_ENABLE		(WRArray[7].word)
#define OUT_FRONT		(WRArray[8].word)	/*  向前 */


#define RunTimeMin		WRArray[50].word
#define RunTimeSec		WRArray[51].word

#define SetMode		WRArray[60].word

#define SetParter		WRArray[61].word

#define CurSelectMode	WRArray[62].word		/*  current select mode ,if select < 8 display time ,relay work  */



/* write area 地址100开始用于存储保持数据 */


#define ADDR_FAC_START	150

#define SetMotorStep	WRArray[ADDR_FAC_START].word

#define RemainTimes		WRArray[ADDR_FAC_START+1].word



#define RunGroup		WRArray[ADDR_FAC_START+4].word		// use times group number

#define RunMode		WRArray[ADDR_FAC_START+5].word		// 1 work mode, 0 test mode working 10 minitue

U32 *SetUseTermPswd;

///////////////////////////////////////////////////////////////////////////////

static u32 SystemTime;

U16 TimeKey;				/* 按键消抖时间计时 */
U08 TimeKeyCnt;				/* 按键连续按下时间 */

BOOL FgLedFlash;			/* 数据管闪烁标志位 */
BOOL FgFlash500ms; 			/* 500ms闪烁标志 */

U08 TimePulse20ms = 0;
U08 FlagPulse20ms = 0;


KEYSTATUS g_Key;
					


BOOL FgKeySpeak = FALSE;


BYTE error;
BYTE SerialNumber_SHT2x[8];  //64bit serial number
	
BYTE userRegister;           //variable for user register

U16 DisplayStatusBak;		/* 界面显示备份 */

static U16 SetParterTmp = 0;

PASSData  UseTimesData[MAX_TIMES_DATA];

#define MAX_MODE_NUMB	8

/* Private function prototypes -----------------------------------------------*/
void ReadKey(void);
void DealAction(void);
void DealActStatus(void);
void DealTime(void);
void DealSetLed(void);
BOOL DealInputTermPwsd(void);
U16 LoadUseTimes(void);

U16 FlashWriteData(DWORD Address, U16 * pData, U16 Numb);
U16 FlashReadData(DWORD Address, U16 * pData, U16 Numb);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : GetSystemTime
* Description    : 获取系统时间,确保不被计时中断打断
* Input          : None
* Output         : None
* Return         : systemtime
*******************************************************************************/
U32 GetSystemTime(void)
{
	U32 TempData;
	/* TIM IT disable */
	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);

	TempData = SystemTime;

	/* TIM IT enable */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	return TempData;
}

/*******************************************************************************
* Function Name  : IT_Time
* Description    : call interrupt function
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IT_Time(void)
{
	static U08 Time5ms = 0;
	static U08 Time100ms = 0;
	static U08 Time500ms = 0;
	static U08 LedFlashTime = 0;


	SystemTime++;

	if (++Time5ms >= 5)					/* 5ms 时间 */
	{
		Time5ms = 0;

		UartTimer();

		if (TimeKey < 0xffff)
		{
			TimeKey++;
		}
		
		if (++Time100ms >= 20)		/* 100ms 时间 */
		{
			Time100ms = 0;
			if (TimeKeyCnt < 0xff)
			{
				TimeKeyCnt++;
			}

			if (g_Key.Cnt < 0xff)
			{
				g_Key.Cnt++;
			}

			LedFlashTime++;

			if (FgLedFlash && LedFlashTime >= 7)
			{
				FgLedFlash = FALSE;
				LedFlashTime = 0;
			}
			else if (!FgLedFlash && LedFlashTime >= 3)
			{
				FgLedFlash = TRUE;
				LedFlashTime = 0;
			}

			if (++Time500ms >= 5)
			{
				Time500ms = 0;
				
				if (FgFlash500ms)
				{
					FgFlash500ms = FALSE;
				}
				else
				{
					FgFlash500ms = TRUE;
				}
			}
		}
	}
}

/*******************************************************************************
* Function Name  : ReadInputSwitch
* Description    : read input and switch, 消抖处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ReadInput(void)
{
	const U16 FilterTime[MAX_INPUT_NUM] = {0,0,200};
	static U16 BAUDInputBak[MAX_INPUT_NUM];
	static U32 BAUDInputDataTime[MAX_INPUT_NUM];
	U16 BAUDInputData[MAX_INPUT_NUM];
	
	U16 i;

	InputBuffer = 0;

	for (i=0; i<MAX_INPUT_NUM; i++)
	{
		BAUDInputData[i] = 1 - GPIO_ReadInputDataBit(BAUDIO[i].GPIOx, BAUDIO[i].GPIO_Pin);

		if (BAUDInputBak[i] != BAUDInputData[i])
		{
			BAUDInputBak[i] = BAUDInputData[i];
			BAUDInputDataTime[i] = GetSystemTime();
		}

		if (GetSystemTime() - BAUDInputDataTime[i] > FilterTime[i])
		{
			ROArray[INPUT_DATA_ADDR+i].word = BAUDInputData[i];

			if (ROArray[INPUT_DATA_ADDR+i].word)
			{
				SETBIT(InputBuffer, i);
			}
		}
	}
}



/******************************************************************************************************
* Funtion    : ReadKey()
* Description: 读取按键
* Argument   : none
* Returns    : none
*******************************************************************************************************/
void ReadKey(void)
{
	static WORD KeyValueBak;
	U16 KeyValue;
	BIT16 ReadKeyValue;
	
	KeyValue = 0;
	ReadKeyValue.word = 0;

	ReadKeyValue.bit.b0 = GPIO_ReadInputDataBit(BAUDIO[1].GPIOx, BAUDIO[1].GPIO_Pin);
	ReadKeyValue.bit.b1 = GPIO_ReadInputDataBit(BAUDIO[2].GPIOx, BAUDIO[2].GPIO_Pin);
	ReadKeyValue.bit.b2 = GPIO_ReadInputDataBit(BAUDIO[0].GPIOx, BAUDIO[0].GPIO_Pin);

	KeyValue = ReadKeyValue.word;

	if (KeyValue)
	{
		if (KeyValueBak != KeyValue)
		{
			KeyValueBak = KeyValue;
			TimeKey = 0;
		}
		else if (TimeKey >= 5)	// 消抖时间50ms
		{
			if (g_Key.Value != KeyValue)
			{
				g_Key.Value = KeyValue;

				g_Key.Status = KEY_DOWN;

				FgKeySpeak = TRUE;
			}
			else if (g_Key.Status == KEY_DOWN)
			{
				g_Key.Status = KEY_CONTINUE;
				g_Key.Cnt = 0;
			}
		}
	}
	else
	{
		if (g_Key.Status == KEY_DOWN || g_Key.Status == KEY_CONTINUE)
		{
			g_Key.Status = KEY_UP;
		}
		else if (g_Key.Status == KEY_UP)
		{
			g_Key.Cnt = 0;
			g_Key.Status = KEY_NULL;
		}
		else 
		{
			g_Key.Status = KEY_NULL;
			g_Key.Value = KeyValue;
		}

		KeyValueBak = KeyValue;
	}
}


/******************************************************************************************************
* Funtion    : DealKeyBoard()
* Description: deal key 
* Argument   : none
* Returns    : none
*******************************************************************************************************/
void DealKeyBoard(void)
{
	if (g_Key.Status == KEY_DOWN)
	{		
		switch (g_Key.Value)
		{
			case KEY_UP_VALUE:
				break;
			case KEY_DOWN_VALUE:
				break;
			case KEY_SET_VALUE:
				break;
			default:
				break;
		}
	}
	else if (g_Key.Status == KEY_CONTINUE)
	{
		switch (g_Key.Value)
		{
			case KEY_ENTER_VALUE:

				break;
			case KEY_UP_VALUE:

				break;
			case KEY_SET_VALUE:
				if (g_Key.Cnt > 30)
				{
					;
				}
				break;
			default:
				break;
		}
	}
	else if (g_Key.Status == KEY_UP)
	{
		switch (g_Key.Value)
		{
			case KEY_ENTER_VALUE:

				break;
			case KEY_SET_VALUE:
				
				break;
			default:
				break;
		}
	}
}

/*******************************************************************************
* Function Name  : DealAlarm
* Description    : deal alarm in action
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DealAlarm(void)
{

}

/*******************************************************************************
* Function Name  : DealStepOutputCom
* Description    : 处理输出端口设置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DealStepOutputCom(void)
{
	static BOOL FgPulse = TRUE;
	TimePulse20ms++;
	
	if (SetMotorStep)
	{
		SetMotorStep--;
		
		
		if(FlagPulse20ms)
		{
		    if(TimePulse20ms >= 8L)
		    {
		        TimePulse20ms = 0;
		        GPIO_WriteBit(STEPIO[COM_STEP_OUT].GPIOx,STEPIO[COM_STEP_OUT].GPIO_Pin,Bit_SET);
		    }
		    else if(TimePulse20ms >= 4L)
		    {
		        GPIO_WriteBit(STEPIO[COM_STEP_OUT].GPIOx,STEPIO[COM_STEP_OUT].GPIO_Pin,Bit_RESET);
		    }
		}
		else
		{
    		if (FgPulse)
    		{
    			GPIO_WriteBit(STEPIO[COM_STEP_OUT].GPIOx,STEPIO[COM_STEP_OUT].GPIO_Pin,Bit_SET);
    			FgPulse = FALSE;
    		}
    		else
    		{
    			GPIO_WriteBit(STEPIO[COM_STEP_OUT].GPIOx,STEPIO[COM_STEP_OUT].GPIO_Pin,Bit_RESET);
    			FgPulse = TRUE;
    		}
        }
	}
	else
	{
		GPIO_WriteBit(STEPIO[COM_STEP_OUT].GPIOx,STEPIO[COM_STEP_OUT].GPIO_Pin,Bit_RESET);
		FgPulse = TRUE;
		//TimePulse20ms = 0;
		
	}
}

/*******************************************************************************
* Function Name  : DealSetOutputCom
* Description    : 处理输出端口设置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DealOutputCom(void)
{
	int i;

	
	for (i=0; i<MAX_M_OUT_NUM; i++)
	{
		if (i == MAX_M_OUT_NUM-1)
		{
			if (WRArray[i].word || (GETBIT((WRArray[30+((i)/16)].word),((i)%16))))
			{
				GPIO_WriteBit(MOutputIO[i].GPIOx,MOutputIO[i].GPIO_Pin,Bit_SET);
			}
			else
			{
				GPIO_WriteBit(MOutputIO[i].GPIOx,MOutputIO[i].GPIO_Pin,Bit_RESET);
			}
		}
		else
		{
			if (WRArray[i].word || (GETBIT((WRArray[30+((i)/16)].word),((i)%16))))
			{
				GPIO_WriteBit(MOutputIO[i].GPIOx,MOutputIO[i].GPIO_Pin,Bit_RESET);
			}
			else
			{
				GPIO_WriteBit(MOutputIO[i].GPIOx,MOutputIO[i].GPIO_Pin,Bit_SET);
			}
		}
	}

	for (i=0; i<MAX_STEP_NUM_COM-1; i++)
	{
		if (WRArray[MAX_M_OUT_NUM+i].word)
		{
			GPIO_WriteBit(STEPIO[i].GPIOx,STEPIO[i].GPIO_Pin,Bit_SET);
		}
		else
		{
			GPIO_WriteBit(STEPIO[i].GPIOx,STEPIO[i].GPIO_Pin,Bit_RESET);
		}
	}
	
}
/*******************************************************************************
* Function Name  : DealActionOutput
* Description    : deal output action
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DealActOutput(void)
{
	static U32 IntervalTime = 0;
	static U32 OutDelayTime = 0;
	static U32 CurSelectModeDelayTime = 0;
	static U32 FlashDelayTime = 0;
	static BOOL OutE = FALSE;

	OUT_VALVE_A = 0;
	OUT_VALVE_A_BAK = 0;
	OUT_VALVE_B = 0;
	OUT_VALVE_B_BAK = 0;

	OUT_24V = 0;
	OUT_24V_BAK = 0;
	OUT_RELAY = 0;

	if ((GetSystemTime() - IntervalTime)>6000)
	{
		IntervalTime = GetSystemTime();

		if (OutE)
		{
			OutE = FALSE;
		}
		else
		{
			OutE = TRUE;
		}
	}


	if  (CurSelectMode < MAX_MODE_NUMB)
	{
		if (OutE)
		{
			OUT_VALVE_A = 1;
			OUT_VALVE_A_BAK = 1;
			OUT_VALVE_B = 0;
			OUT_VALVE_B_BAK = 0;
		}
		else
		{
			OUT_VALVE_A = 0;
			OUT_VALVE_A_BAK = 0;
			OUT_VALVE_B = 1;
			OUT_VALVE_B_BAK = 1;
		}	

		if  ((GetSystemTime() - CurSelectModeDelayTime) < 3000)
		{
			OUT_RELAY = 0;

			if (MachineStatus == 0)
			{
				OUT_24V = 1;
			}
		}
		else
		{
			OUT_RELAY = 1;
		}
	}
	else
	{
		IntervalTime = GetSystemTime();
		CurSelectModeDelayTime = GetSystemTime();
		OutE = TRUE;
		OUT_VALVE_A = 0;
		OUT_VALVE_A_BAK = 0;
		OUT_VALVE_B = 0;
		OUT_VALVE_B_BAK = 0;

		OUT_RELAY = 0;
	}

	
	if  (MachineStatus == 1)
	{
		if (SetMode== 0 || SetMode == 4)
		{
			OUT_24V = 1;
		}
		else if (SetMode == 1)
		{
			if ((GetSystemTime() - FlashDelayTime)>700)
			{
				FlashDelayTime = GetSystemTime();
			}
			else if ((GetSystemTime() - FlashDelayTime)<500)
			{
				OUT_24V = 1;
			}
		}
		else if (SetMode == 5)
		{
			if ((GetSystemTime() - FlashDelayTime)>1000)
			{
				FlashDelayTime = GetSystemTime();
			}
			else if ((GetSystemTime() - FlashDelayTime)<500)
			{
				OUT_24V = 1;
			}
		}
		else if (SetMode == 2 || SetMode == 6)
		{
			if ((GetSystemTime() - FlashDelayTime)>600)
			{
				FlashDelayTime = GetSystemTime();
			}
			else if ((GetSystemTime() - FlashDelayTime)<300)
			{
				OUT_24V = 1;
			}
		}
		else if (SetMode == 3 || SetMode == 7)
		{
			if ((GetSystemTime() - FlashDelayTime)>400)
			{
				FlashDelayTime = GetSystemTime();
			}
			else if ((GetSystemTime() - FlashDelayTime)<200)
			{
				OUT_24V = 1;
			}
		}

		//OutDelayTime = GetSystemTime();
	}


	if  ((GetSystemTime() - OutDelayTime)<5000)
	{
		OUT_VALVE_B = 1;
		OUT_VALVE_B_BAK = 1;
	}

	if  (MachineStatus == 3)
	{
		OUT_24V = 1;
		
		OUT_VALVE_A = 1;
		OUT_VALVE_A_BAK = 1;
		OUT_VALVE_B = 0;
		OUT_VALVE_B_BAK = 0;

		OutDelayTime = GetSystemTime();
	}

	if (RunMode != 1)	/*  run test mode ,run 10 minitue  */
	{
		if (GetSystemTime()/1000 > 600)
		{
			OUT_VALVE_A = 0;
			OUT_VALVE_A_BAK = 0;
			OUT_VALVE_B = 0;
			OUT_VALVE_B_BAK = 0;

			OUT_24V = 0;
			OUT_24V_BAK = 0;
			OUT_RELAY = 0;

			OUT_ENABLE = 1;
			OUT_FRONT = 0;
		}
	}
}

/*******************************************************************************
* Function Name  : DealAction
* Description    : deal all action in this function
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DealAction(void)
{
	DealActStatus();

	DealActOutput();

	DealOutputCom();

	DealTime();
}

/*******************************************************************************
* Function Name  : DispRun
* Description    : main fuction 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DispRun(void)
{
	static U16 MachineStatusBak = 0;
	static U16 RunTimeMinBak = 0;
	static U16 RunTimeSecBak = 0;
	static U16 RemainTimesBak = 0;
	

	if (DisplayStatusBak != DisplayStatus)
	{
		DisplayStatusBak = DisplayStatus;

		DWIN_WriteOneRAM(DWIN_ADDR_DATA_MIN, RunTimeMin);

		DWIN_WriteOneRAM(DWIN_ADDR_DATA_SEC, RunTimeSec);

		DWIN_WriteOneRAM(DWIN_ADDR_DATA_TIME, RemainTimes);

		if (MachineStatus == 1)
		{
			DWIN_WriteOneRAM(DWIN_ADDR_ICON_START, 0);
		}
		else
		{
			DWIN_WriteOneRAM(DWIN_ADDR_ICON_START, 1);
		}

		DWIN_WriteOneRAM(DWIN_ADDR_ICON_MODE, SetMode);

		DWIN_WriteOneRAM(DWIN_ADDR_ICON_PARTER, SetParter);
	}

	DWKey = DWIN_ReadKey();

	switch (DWKey)
	{
		case K_DWIN_TREATMENT:

			//DWIN_WriteOneRAM(DWIN_ADDR_DATA_TIME, DWKey);
			
			if (MachineStatus == 0)
			{
				DWIN_WriteOneREG(0x03,  (DWIN_WD_MODE_1+SetMode));

				DWIN_WaitOver();

				DisplayStatus = 2;
			}
			break;
		case K_DWIN_START:
			if  (MachineStatus == 0 && CurSelectMode < MAX_MODE_NUMB)
			{
				if (RemainTimes == 0)	/* times is zero turn password menu */
				{
					if (LoadUseTimes())
					{
						MachineStatus = 1;

						FgStartFirst = TRUE;
					}
					else
					{
						DisplayStatus = 6;
					}
				}
				else
				{
					MachineStatus = 1;

					FgStartFirst = TRUE;
				}
			}
			else if (MachineStatus == 2)
			{
				MachineStatus = 1;
			}
			else if (MachineStatus == 1)
			{
				MachineStatus = 2;
			}
			break;
	}

	if (MachineStatusBak != MachineStatus)
	{
		MachineStatusBak = MachineStatus;

		if (MachineStatus == 1)
		{
			DWIN_WriteOneRAM(DWIN_ADDR_ICON_START, 0);
		}
		else
		{
			DWIN_WriteOneRAM(DWIN_ADDR_ICON_START, 1);
		}
	}

	if (RunTimeMinBak != RunTimeMin)
	{
		RunTimeMinBak = RunTimeMin;
		
		DWIN_WriteOneRAM(DWIN_ADDR_DATA_MIN, RunTimeMin);
	}

	if (RunTimeSecBak != RunTimeSec)
	{
		RunTimeSecBak = RunTimeSec;

		DWIN_WriteOneRAM(DWIN_ADDR_DATA_SEC, RunTimeSec);
	}

	if (RemainTimesBak != RemainTimes)
	{
		RemainTimesBak = RemainTimes;

		DWIN_WriteOneRAM(DWIN_ADDR_DATA_TIME, RemainTimes);
	}
}

/*******************************************************************************
* Function Name  : DispModeSeclet
* Description    : main fuction 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DispModeSeclet(void)
{
	static U16 SetModeBak = 0;

	U16 i;

	if (DisplayStatusBak != DisplayStatus)
	{
		DisplayStatusBak = DisplayStatus;

		CurSelectMode = SetMode;

		/*for (i=0; i<8; i++)
		{
			if (SetMode == i)
			{
				DWIN_WriteOneRAM(DWIN_ADDR_ICON_MODE_1+i, 1);
			}
			else
			{
				DWIN_WriteOneRAM(DWIN_ADDR_ICON_MODE_1+i, 0);
			}
		}*/
	}

	DWKey = DWIN_ReadKey();

	switch (DWKey)
	{
		case K_DWIN_SELECT_CON:
			
			DisplayStatus = 3;
			
			break;
		case K_DWIN_SELECT_CANCEL:
			
			DisplayStatus = 1;
			
			break;
		case K_DWIN_SELECT_1:
			SetMode = 0;
			break;
		case K_DWIN_SELECT_2:
			SetMode = 1;
			break;
		case K_DWIN_SELECT_3:
			SetMode = 2;
			break;
		case K_DWIN_SELECT_4:
			SetMode = 3;
			break;
		case K_DWIN_SELECT_5:
			SetMode = 4;
			break;
		case K_DWIN_SELECT_6:
			SetMode = 5;
			break;
		case K_DWIN_SELECT_7:
			SetMode = 6;
			break;
		case K_DWIN_SELECT_8:
			SetMode = 7;
			break;	
	}

	if (SetModeBak != SetMode)
	{
		SetModeBak = SetMode;

		DWIN_WriteOneREG(0x03,  (DWIN_WD_MODE_1+SetMode));

		DWIN_WaitOver();

		/*for (i=0; i<8; i++)
		{
			if (SetMode == i)
			{
				DWIN_WriteOneRAM(DWIN_ADDR_ICON_MODE_1+i, 1);
			}
			else
			{
				DWIN_WriteOneRAM(DWIN_ADDR_ICON_MODE_1+i, 0);
			}
		}*/
	}
}

/*******************************************************************************
* Function Name  : DispParterSeclet
* Description    : main fuction 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DispParterSeclet(void)
{
	static U16 DispPart = 0;
	static U16 DispPartBak = 0;
	static U16 DwinKeyBak = 0;

	if (DisplayStatusBak != DisplayStatus)
	{
		DisplayStatusBak = DisplayStatus;

		if (SetParter < 5)
		{
			DispPart = 0;
		}
		else
		{
			DispPart = 1;
		}

		
		if (DispPart)
		{
			DWIN_WriteOneRAM(DWIN_ADDR_ICON_FACE, 0);
			DWIN_WriteOneRAM(DWIN_ADDR_ICON_BODY, 1);
		}
		else
		{
			DWIN_WriteOneRAM(DWIN_ADDR_ICON_FACE, 1);
			DWIN_WriteOneRAM(DWIN_ADDR_ICON_BODY, 0);
		}

		DwinKeyBak = DWKey;
	}

	DWKey = DWIN_ReadKey();

	if (DwinKeyBak == 0 && DWKey)
	{

	switch (DWKey)
	{
		case K_DWIN_CONFIRM:

			if (DispPart)
			{
				DWIN_WriteOneREG(0x03,DWIN_WD_BODY_SEL);

				DWIN_WaitOver();
				
				DisplayStatus = 5;
			}
			else
			{

				DWIN_WriteOneREG(0x03,DWIN_WD_FACE_SEL);

				DWIN_WaitOver();
				
				DisplayStatus = 4;
			}

			SetParterTmp = 0;
			
			break;
		case K_DWIN_CANCEL:

			//DWIN_WriteOneREG(0x03,DWIN_WD_MODE);

			DWIN_WriteOneREG(0x03,  (DWIN_WD_MODE_1+SetMode));

			DWIN_WaitOver();
			
			DisplayStatus = 2;
			
			break;
		case K_DWIN_FACE:
			DispPart = 0;
			break;

		case K_DWIN_BODY:
			DispPart = 1;
			break;
	}
	}

	DwinKeyBak = DWKey;

	if (DispPartBak != DispPart)
	{
		DispPartBak = DispPart;
		
		if (DispPart)
		{
			DWIN_WriteOneRAM(DWIN_ADDR_ICON_FACE, 0);
			DWIN_WriteOneRAM(DWIN_ADDR_ICON_BODY, 1);
		}
		else
		{
			DWIN_WriteOneRAM(DWIN_ADDR_ICON_FACE, 1);
			DWIN_WriteOneRAM(DWIN_ADDR_ICON_BODY, 0);
		}
	}
}

/*******************************************************************************
* Function Name  : DealParterSeclet
* Description    : main fuction 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DispFaceSeclet(void)
{
	DWKey = DWIN_ReadKey();
	
	switch (DWKey)
	{
		case K_DWIN_POS_ENTER:

			DisplayStatus = 3;

			SetParter = SetParterTmp;
			
			break;
		case K_DWIN_POS_CANCEL:

			DWIN_WriteOneREG(0x03,  (DWIN_WD_MODE_1+SetMode));

			DWIN_WaitOver();
			
			DisplayStatus = 2;
			
			break;
		case K_DWIN_FOREHEAD:
			SetParterTmp = 0;
			break;
		case K_DWIN_EYE:
			SetParterTmp = 1;
			break;
		case K_DWIN_NOSE:
			SetParterTmp = 2;
			break;
		case K_DWIN_CHEEK:
			SetParterTmp = 3;
			break;
		case K_DWIN_LIPS:
			SetParterTmp = 4;
			break;	
	}
}

/*******************************************************************************
* Function Name  : DealParterSeclet
* Description    : main fuction 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DispBodySeclet(void)
{
	DWKey = DWIN_ReadKey();
	
	switch (DWKey)
	{
		case K_DWIN_POS_ENTER:

			DisplayStatus = 3;

			SetParter = SetParterTmp;
			
			break;
		case K_DWIN_POS_CANCEL:

			DWIN_WriteOneREG(0x03,  (DWIN_WD_MODE_1+SetMode));

			DWIN_WaitOver();
			
			DisplayStatus = 2;
			
			break;
		case K_DWIN_NECK:
			SetParterTmp = 5;
			break;
		case K_DWIN_CHEST:
			SetParterTmp = 6;
			break;
		case K_DWIN_NAVEL:
			SetParterTmp = 7;
			break;
		case K_DWIN_VULVA:
			SetParterTmp = 8;
			break;
	}
}


/*******************************************************************************
* Function Name  : DealDebug
* Description    : deal input debug password
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DispInputPassword(void)
{
	BOOL MenuStatus = TRUE;
	U32  InputPswd = 0L;

	OUT_ENABLE = 1;
	
	DWIN_WriteOneREG(0x03,DWIN_WD_TERM_PSWD);
	
	DWIN_WaitOver();

	DWIN_WriteRAM(DWIN_ADDR_CUR_PSWD, 2, (U16 *)&InputPswd);

	DWIN_WriteOneRAM(DWIN_ADDR_CUR_TERM_NUM, RunGroup);

	while (MenuStatus)
	{

		DWKey = DWIN_ReadKey();

		switch (DWKey)
		{
			case K_DWIN_TERM_ENTER:
				if (InputPswd == UseTimesData[RunGroup].PassWord)
				{
					 UseTimesData[RunGroup].FgUse = 0;
					 
					RunGroup++ ;
			
					RemainTimes =  UseTimesData[RunGroup].SetTimes;

					FlashWriteData(FLASH_USER_START_ADDR, &WRArray[ADDR_FAC_START].word , 20);
					FlashWriteData(FLASH_TERM_START_ADDR, &UseTimesData[0].FgUse, 210);
				}

				DisplayStatus = 1;

				DWIN_WriteOneREG(0x03,DWIN_WD_RUN);
	
				DWIN_WaitOver();

				MenuStatus = FALSE;
					
				break;

			default:
				break;
		}

		ReadInput();

		DWIN_CommuProg();

		COM_UartSlave(COM_UART2);

		DealOutputCom();

		if  (SetPswdData.update)
		{
			SetPswdData.update = FALSE;
			
			InputPswd = SetPswdData.val;
			
			DWIN_WriteRAM(DWIN_ADDR_CUR_PSWD, 2, (U16 *)&InputPswd);
		}

	}
}


/*******************************************************************************
* Function Name  : DealDisplay
* Description    : main fuction 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DealDisplay(void)
{
	switch (DisplayStatus)
	{
		case 1:
			DispRun();
			break;
		case 2:
			DispModeSeclet();
			break;
		case 3:
			DispParterSeclet();
			break;
		case 4:
			DispFaceSeclet();
			break;
		case 5:
			DispBodySeclet();
			break;
		case 6:
			DispInputPassword();
			break;
		default:
			DisplayStatus = 1;
			break;
	}
}

/*******************************************************************************
* Function Name  : DealInputTermPwsd
* Description    : deal input debug password
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
BOOL DealInputTermPwsd(void)
{
	BOOL MenuStatus = TRUE;

	U32  InputPswd = 0L;

	BOOL RetVal = FALSE;

	SetUseTermPswd = (U32*)(&WRArray[ADDR_FAC_START+6].word);

	OUT_ENABLE = 1;

	OUT_VALVE_A = 0;
	OUT_VALVE_A_BAK = 0;
	
	DWIN_WriteOneREG(0x03,DWIN_WD_SET_TERM_PSWD);
	
	DWIN_WaitOver();

	DWIN_WriteRAM(DWIN_ADDR_TERM_PSWD,2,  (U16 *)&InputPswd);

	while (MenuStatus)
	{

		DWKey = DWIN_ReadKey();

		switch (DWKey)
		{
			case K_DWIN_TERM_ENTER:

				MenuStatus = FALSE;
				
				if (InputPswd == *SetUseTermPswd)
				{
					RetVal = TRUE;
				}
				else
				{
					RetVal = FALSE;
				}
				break;

			default:
				break;
		}

		ReadInput();

		DWIN_CommuProg();

		COM_UartSlave(COM_UART2);

		DealOutputCom();

		if  (SetPswdData.update)
		{
			SetPswdData.update = FALSE;
			
			InputPswd = SetPswdData.val;
			
			DWIN_WriteRAM(DWIN_ADDR_TERM_PSWD, 2, (U16 *)&InputPswd);
		}

	}
	
	return RetVal;
}

/*******************************************************************************
* Function Name  : DealDebug
* Description    : deal input debug password
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitSetTerm(void)
{
	U08 i;
	
	SetUseTermPswd = (U32*)(&WRArray[ADDR_FAC_START+6].word);

	*SetUseTermPswd = 201707L;

	for (i=0; i<MAX_TIMES_DATA; i++)
	{
		UseTimesData[ i].FgUse = FALSE;
		UseTimesData[ i].SetTimes = 100;
		UseTimesData[ i].PassWord = 201700+i;
	}
}

/*******************************************************************************
* Function Name  : DealDebug
* Description    : deal input debug password
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DealSetTerm(void)
{
	#define MAX_PAGE_NUM 		10
	BOOL MenuStatus = TRUE;
	U32  InputPswd = 0L;

	U08 SetPage = 0;
	U08 i;

	BOOL FgReloadTimes = FALSE;

	PASSData  sUseTimesData[MAX_PAGE_NUM];

	PASSData  sUseTimesDataBak[MAX_PAGE_NUM];

	SetUseTermPswd = (U32*)(&WRArray[ADDR_FAC_START+6].word);

	for (i=0; i<MAX_PAGE_NUM; i++)
	{
		sUseTimesData[i] = UseTimesData[SetPage*MAX_PAGE_NUM + i];

		sUseTimesDataBak[i] = sUseTimesData[i];
	}
	

	if (DealInputTermPwsd())
	{
		DWIN_WriteOneREG(0x03,DWIN_WD_TERM_SET);
	
		DWIN_WaitOver();
		
		for (i=0; i<MAX_PAGE_NUM; i++)
		{
			DWIN_WriteOneRAM(DWIN_ADDR_USE_TERM_NUM+i, (SetPage*MAX_PAGE_NUM + i+1));

			DWIN_WriteOneRAM(DWIN_ADDR_USE_TIMES0_SEL+i*0x010, (sUseTimesData[i].FgUse));

			DWIN_WriteOneRAM(DWIN_ADDR_USE_TIMES0_V_N+i*0x010, (sUseTimesData[i].SetTimes));

			DWIN_WriteRAM(DWIN_ADDR_USE_TIMES0_PSWD+i*0x010, 2,(U16 *)(&sUseTimesData[i].PassWord));

			DWIN_WaitOver();
		}

		InputPswd = *SetUseTermPswd;

		DWIN_WriteRAM(DWIN_ADDR_TERM_PSWD,2,  (U16 *)&InputPswd);


		while (MenuStatus)
		{

			DWKey = DWIN_ReadKey();

			switch (DWKey)
			{
				case K_DWIN_TERM_ENTER:

					for (i=0; i<MAX_PAGE_NUM; i++)
					{
						UseTimesData[SetPage*MAX_PAGE_NUM + i] = sUseTimesData[i];
					}

					MenuStatus = FALSE;

					 *SetUseTermPswd = InputPswd;

					FlashWriteData(FLASH_USER_START_ADDR, &WRArray[ADDR_FAC_START].word , 20);
					FlashWriteData(FLASH_TERM_START_ADDR, &UseTimesData[0].FgUse, 210);
					
					DWIN_WriteOneREG(0x03,DWIN_WD_POWERON);
	
					DWIN_WaitOver();
					break;
				case K_DWIN_TERM_PAGE_UP:

					for (i=0; i<MAX_PAGE_NUM; i++)
					{
						UseTimesData[SetPage*MAX_PAGE_NUM + i] = sUseTimesData[i];
					}

					if (SetPage)
					{
						SetPage--;
					}

					for (i=0; i<MAX_PAGE_NUM; i++)
					{
						sUseTimesData[i] = UseTimesData[SetPage*MAX_PAGE_NUM + i];

						DWIN_WriteOneRAM(DWIN_ADDR_USE_TERM_NUM+i, (SetPage*MAX_PAGE_NUM + i+1));
					}

					
				
					break;
				case K_DWIN_TERM_PAGE_DW:

					for (i=0; i<MAX_PAGE_NUM; i++)
					{
						UseTimesData[SetPage*MAX_PAGE_NUM + i] = sUseTimesData[i];
					}

					if (SetPage<4)
					{
						SetPage++;
					}
					else
					{
						SetPage = 0;
					}

					for (i=0; i<MAX_PAGE_NUM; i++)
					{
						sUseTimesData[i] = UseTimesData[SetPage*MAX_PAGE_NUM + i];

						DWIN_WriteOneRAM(DWIN_ADDR_USE_TERM_NUM+i, (SetPage*MAX_PAGE_NUM + i+1));
					}	
					break;
				case K_DWIN_TERM0_SEL:
					if (sUseTimesData[0].FgUse)
					{
						sUseTimesData[0].FgUse = 0;
					}
					else
					{
						sUseTimesData[0].FgUse = 1;
					}
					break;
				case K_DWIN_TERM1_SEL:
					if (sUseTimesData[1].FgUse)
					{
						sUseTimesData[1].FgUse = 0;
					}
					else
					{
						sUseTimesData[1].FgUse = 1;
					}

					break;
				case K_DWIN_TERM2_SEL:
					if (sUseTimesData[2].FgUse)
					{
						sUseTimesData[2].FgUse = 0;
					}
					else
					{
						sUseTimesData[2].FgUse = 1;
					}
					break;
				case K_DWIN_TERM3_SEL:
					if (sUseTimesData[3].FgUse)
					{
						sUseTimesData[3].FgUse = 0;
					}
					else
					{
						sUseTimesData[3].FgUse = 1;
					}
					break;	
				case K_DWIN_TERM4_SEL:
					if (sUseTimesData[4].FgUse)
					{
						sUseTimesData[4].FgUse = 0;
					}
					else
					{
						sUseTimesData[4].FgUse = 1;
					}
					break;
				case K_DWIN_TERM5_SEL:
					if (sUseTimesData[5].FgUse)
					{
						sUseTimesData[5].FgUse = 0;
					}
					else
					{
						sUseTimesData[5].FgUse = 1;
					}
					break;
				case K_DWIN_TERM6_SEL:
					if (sUseTimesData[6].FgUse)
					{
						sUseTimesData[6].FgUse = 0;
					}
					else
					{
						sUseTimesData[6].FgUse = 1;
					}
					break;
				case K_DWIN_TERM7_SEL:
					if (sUseTimesData[7].FgUse)
					{
						sUseTimesData[7].FgUse = 0;
					}
					else
					{
						sUseTimesData[7].FgUse = 1;
					}
					break;
				case K_DWIN_TERM8_SEL:
					if (sUseTimesData[8].FgUse)
					{
						sUseTimesData[8].FgUse = 0;
					}
					else
					{
						sUseTimesData[8].FgUse = 1;
					}
					break;
				case K_DWIN_TERM9_SEL:
					if (sUseTimesData[9].FgUse)
					{
						sUseTimesData[9].FgUse = 0;
					}
					else
					{
						sUseTimesData[9].FgUse = 1;
					}
					break;
				default:
					break;
			}

			ReadInput();

			DWIN_CommuProg();

			COM_UartSlave(COM_UART2);

			DealOutputCom();

			if  (SetPswdData.update)
			{
				SetPswdData.update = FALSE;

				if  (SetPswdData.addr == DWIN_ADDR_TERM_PSWD)
				{
					InputPswd = SetPswdData.val;
			
					DWIN_WriteRAM(DWIN_ADDR_TERM_PSWD, 2, (U16 *)&InputPswd);
				}
				else
				{
					i = (U08) (SetPswdData.addr - DWIN_ADDR_USE_TIMES0_PSWD)/0x10;
					
					sUseTimesData[i].PassWord = SetPswdData.val;
				}
				
				//DWIN_WriteRAM(SetPswdData.addr, 4, sUseTimesData[i].PassWord);
			}

			
			if (SetNum.update)
			{
				SetNum.update = FALSE;
			
				i = (U08) (SetNum.addr - DWIN_ADDR_USE_TIMES0_V_N)/0x10;
				
				sUseTimesData[i].SetTimes = SetNum.val;
				
				//DWIN_WriteOneRAM(SetNum.addr,  sUseTimesData[i].SetTimes);
			}

			for (i=0; i<MAX_PAGE_NUM; i++)
			{
				if (sUseTimesDataBak[i].FgUse != sUseTimesData[i].FgUse)
				{
					DWIN_WriteOneRAM(DWIN_ADDR_USE_TIMES0_SEL+i*0x010, (sUseTimesData[i].FgUse));

					sUseTimesDataBak[i].FgUse = sUseTimesData[i].FgUse;
				}

				if (sUseTimesDataBak[i].SetTimes != sUseTimesData[i].SetTimes)
				{
					DWIN_WriteOneRAM(DWIN_ADDR_USE_TIMES0_V_N+i*0x010, (sUseTimesData[i].SetTimes));

					sUseTimesDataBak[i].SetTimes = sUseTimesData[i].SetTimes;

					FgReloadTimes = TRUE;
				}

				if (sUseTimesDataBak[i].PassWord != sUseTimesData[i].PassWord)
				{
					DWIN_WriteRAM(DWIN_ADDR_USE_TIMES0_PSWD+i*0x010, 2,(U16 *)(&sUseTimesData[i].PassWord));
					
					sUseTimesDataBak[i].PassWord = sUseTimesData[i].PassWord;
				}

				DWIN_WaitOver();
			}

		}
	}
	else
	{
		DWIN_WriteOneREG(0x03,DWIN_WD_POWERON);
	
		DWIN_WaitOver();
	}

	if (FgReloadTimes)
	{
		RemainTimes = UseTimesData[0].SetTimes;

		RunGroup = 0;

		FlashWriteData(FLASH_USER_START_ADDR, &WRArray[ADDR_FAC_START].word , 20);
	}
}

/*******************************************************************************
* Function Name  : DealDebug
* Description    : deal input debug password
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DealDebug(void)
{
	BOOL MenuStatus = TRUE;
	U32 const DEBUGPSWD = 20170714L;
	U32  InputPswd = 0L;

	OUT_ENABLE = 1;

	OUT_VALVE_A = 0;
	OUT_VALVE_A_BAK = 0;
	
	DWIN_WriteOneREG(0x03,DWIN_WD_DEBUG_PSWD);
	
	DWIN_WaitOver();

	DWIN_WriteRAM(DWIN_ADDR_DEBUG_PASSWORD, 2, (U16 *)&InputPswd);

	while (MenuStatus)
	{

		DWKey = DWIN_ReadKey();

		switch (DWKey)
		{
			case K_DWIN_TERM_ENTER:
				if (InputPswd == DEBUGPSWD)
				{
					RunMode = 1;

					DWIN_WriteOneREG(0x03,DWIN_WD_POWERON);
	
					DWIN_WaitOver();

					MenuStatus = FALSE;

					InitSetTerm();

					RemainTimes = UseTimesData[0].SetTimes;

					RunGroup = 0;

					FlashWriteData(FLASH_USER_START_ADDR, &WRArray[ADDR_FAC_START].word , 20);
					FlashWriteData(FLASH_TERM_START_ADDR, &UseTimesData[0].FgUse, 210);
				}
				break;

			default:
				break;
		}

		ReadInput();

		DWIN_CommuProg();

		COM_UartSlave(COM_UART2);

		DealOutputCom();

		if  (SetPswdData.update)
		{
			SetPswdData.update = FALSE;
			
			InputPswd = SetPswdData.val;
			
			DWIN_WriteRAM(DWIN_ADDR_DEBUG_PASSWORD, 2, (U16 *)&InputPswd);
		}

	}
}

/*******************************************************************************
* Function Name  : StartProgram
* Description    : main fuction 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void StartProgram(void)
{
	BOOL MenuStatus = TRUE;
	U16 StartStep = 0;
	U16 KeyDebug = 0;
	U16 KeyTerm = 0;
	U16 DWKeyBak = 0;	// up 

	while (MenuStatus)
	{
		switch (StartStep)
		{
			case 0:
				if (IN_FRONT_LIMIT)
				{
					StartStep = 1;
					SetMotorStep = 0;
					OUT_ENABLE = 1;
				}
				else
				{
					SetMotorStep = 10;
					OUT_ENABLE = 0;
					OUT_FRONT = 1;
				}

				OUT_VALVE_A = 1;
				OUT_VALVE_A_BAK = 1;
				break;
			case 1:
				if (IN_BACK_LIMIT)
				{
					StartStep = 2;
					SetMotorStep = 0;
					OUT_ENABLE = 1;

					OUT_VALVE_A = 0;
					OUT_VALVE_A_BAK = 0;
				}
				else
				{
					SetMotorStep = 10;
					OUT_ENABLE = 0;
					OUT_FRONT = 0;

					OUT_VALVE_A = 1;
					OUT_VALVE_A_BAK = 1;
				}
				
				break;
			case 2:
				MenuStatus = FALSE;
				break;
			default:
				StartStep = 0;
				break;
		}

		DWKey = DWIN_ReadKey();

		switch (DWKey)
		{
			case K_DWIN_DEBUG1:
				KeyTerm = 0;
				if (DWKeyBak == 0)
				{
					if (KeyDebug == 0 || KeyDebug == 3)
					{
						KeyDebug++;
					}
					else
					{
						KeyDebug = 0;
					}
				}
				break;

			case K_DWIN_DEBUG2:
				KeyDebug  = 0;
				if (DWKeyBak == 0)
				{
					KeyTerm++;
				}

				if (KeyTerm >= 5)	// enter term set menu
				{
					DealSetTerm();
					KeyTerm = 0;
				}
				break;
			case K_DWIN_DEBUG3:
				KeyTerm = 0;
				if (DWKeyBak == 0)
				{
					if (KeyDebug == 1 || KeyDebug == 2 || KeyDebug == 4)
					{
						KeyDebug++;
					}
					else
					{
						KeyDebug = 0;
					}
				}

				if (KeyDebug >= 5 && RunMode != 1)	// enter debug input pswd menu 
				{
					DealDebug();
					KeyDebug = 0;
				}

				if  (KeyDebug >= 5)
				{
					RemainTimes = 0;
				}
				break;
			default:
				break;
		}

		DWKeyBak = DWKey;

		ReadInput();

		DWIN_CommuProg();

		COM_UartSlave(COM_UART2);

		DealOutputCom();

		if (DWIN_QEmpty())
		{
			DWIN_WriteOneRAM(DWIN_ADDR_CNTDOWN2, (U16)(SystemTime/1000)%9);
		}
	}

	DWIN_WriteOneREG(0x03,DWIN_WD_RUN);

	DWIN_WaitOver();

	DisplayStatus = 1;

	RunTimeMin = 0;
	RunTimeSec = 0;

	CurSelectMode = MAX_MODE_NUMB;

	FgStartFirst = FALSE;

	MachineStatus = 0;

	if (RunGroup > 0x08fff)	/* initial term data */
	{
		InitSetTerm();

		RemainTimes = UseTimesData[0].SetTimes;

		RunGroup = 0;

		FlashWriteData(FLASH_USER_START_ADDR, &WRArray[ADDR_FAC_START].word , 20);
		FlashWriteData(FLASH_TERM_START_ADDR, &UseTimesData[0].FgUse, 210);
	}

	if (UseTimesData[RunGroup].FgUse)
	{
		if (RemainTimes > UseTimesData[RunGroup].SetTimes)
		{
			RemainTimes = UseTimesData[RunGroup].SetTimes;

			FlashWriteData(FLASH_USER_START_ADDR, &WRArray[ADDR_FAC_START].word , 20);
		}
	}
}


/*******************************************************************************
* Function Name  : WorkProgram
* Description    : main fuction 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WorkProgram(void)
{
	int i;

	for (i=0; i<MODBUS_READ_ONLY_NUM; i++)
	{
		ROArray[i].word = 0;
		WRArray[i].word = 0;
	}

	FlashReadData(FLASH_USER_START_ADDR, &WRArray[ADDR_FAC_START].word , 20);
	FlashReadData(FLASH_TERM_START_ADDR, &UseTimesData[0].FgUse, 210);

	CommuInitial();

	DWIN_Init();
	
	SystemTime = 0;

	VERSION_DATA = 101;
	
	StartProgram();

	while (1)
  	{	
		ReadInput();

		ReadKey();

		DealKeyBoard();

		DealSetLed();

		DealAction();

		DealDisplay();

		//COM_UartSlave(COM_UART1);
		DWIN_CommuProg();

		COM_UartSlave(COM_UART2);
  	}
}



/******************************************************************************************************
* Funtion    : DealSetLed()
* Description: deal set led light
* Argument   : none
* Returns    : none
*******************************************************************************************************/
void DealSetLed(void)
{
	if (FgFlash500ms)
	{
		GPIO_WriteBit(LEDIO[0].GPIOx,LEDIO[0].GPIO_Pin,Bit_SET);
	}
	else
	{
		GPIO_WriteBit(LEDIO[0].GPIOx,LEDIO[0].GPIO_Pin,Bit_RESET);
	}

	
}

/******************************************************************************************************
* Funtion    : DealAction()
* Description: deal action output
* Argument   : none
* Returns    : none
*******************************************************************************************************/
void DealActStatus(void)
{

	static U32 IntervalTime = 0;

	switch (MachineStatus)
	{
		case 0:
			if (CurSelectMode < MAX_MODE_NUMB)
			{
				RunTimeMin = 9;
				RunTimeSec = 30;
			}

			FgStartFirst = FALSE;
			
		case 2:
			OUT_ENABLE = 1;
			OUT_FRONT = 0;
			SetMotorStep = 0;

			break;
		case 1:
			OUT_ENABLE = 0;
			OUT_FRONT = 1;
			FlagPulse20ms = 1; 

			if (IN_FRONT_LIMIT)
			{
				SetMotorStep = 0;
				OUT_ENABLE = 1;
				OUT_FRONT = 0;

				RunTimeMin = 0;
				RunTimeSec = 0;

				MachineStatus = 3;
				CurSelectMode = MAX_MODE_NUMB;
				break;
			}
			//if (GetSystemTime() - IntervalTime >= 480)
			//if (GetSystemTime() - IntervalTime >= 440)
			if (GetSystemTime() - IntervalTime >= 470)
			{
				IntervalTime = GetSystemTime();
				SetMotorStep = 4;
				OUT_ENABLE = 0;
			}
			else if (GetSystemTime() - IntervalTime >= 120)
			{
				if (SetMotorStep == 0)
				{
					OUT_ENABLE = 1;
				}
			}
			
			break;
		case 3:
		    //返回时全速后退,5MS脉冲.
		    FlagPulse20ms = 0;  
			if (IN_BACK_LIMIT)
			{
				MachineStatus = 0;
				SetMotorStep = 0;
				OUT_ENABLE = 1;

				OUT_24V = 0;
			}
			else
			{
				SetMotorStep = 10000;
				OUT_ENABLE = 0;
				OUT_FRONT = 0;
			}
			break;
	}


	//if (DWIN_QEmpty())
	//{
	//	DWIN_WriteOneRAM(0x1005, (U16)SystemTime/1000);
	//}
	

	//OUT_24V = 1;
	//OUT_24V_BAK = 1;

	
}

/******************************************************************************************************
* Funtion    : DealTime()
* Description: deal time about force and times
* Argument   : none
* Returns    : none
*******************************************************************************************************/
void DealTime(void)
{
	static U32 SecTime = 0;

	if (GetSystemTime() - SecTime >= 1000)
	{
		SecTime = GetSystemTime();
	}
	else
	{
		return;
	}

	if (MachineStatus == 1)
	{
		if (RunTimeSec)
		{
			RunTimeSec--;
		}
		else
		{
			if (RunTimeMin)
			{
				RunTimeMin--;
				RunTimeSec = 59;
			}
			else
			{
				MachineStatus = 3;
				//RunTimeMin = 9;
				FgStartFirst = FALSE;
				CurSelectMode = MAX_MODE_NUMB;
			}

			if (FgStartFirst)
			{
				FgStartFirst = FALSE;
				RemainTimes--;
				FlashWriteData(FLASH_USER_START_ADDR, &WRArray[ADDR_FAC_START].word , 20);
			}
		}
	}
}

/******************************************************************************************************
* Funtion    : LoadUseTimes()
* Description: 加载使用次数
* Argument   : none
* Returns    : none
*******************************************************************************************************/
U16 LoadUseTimes(void)
{
	U16 i;
	
	if (RunGroup >= MAX_TIMES_DATA)
	{
		RemainTimes = 6000;
		RunGroup = 0;
	}
	else if (UseTimesData[RunGroup].FgUse == 0)
	{
		for (i=RunGroup; i<MAX_TIMES_DATA; i++)
		{
			RunGroup++;
			RemainTimes = UseTimesData[RunGroup].SetTimes;

			if (RemainTimes)break;
		}
	}

	FlashWriteData(FLASH_USER_START_ADDR, &WRArray[ADDR_FAC_START].word , 20);

	return RemainTimes;
}

/******************************************************************************************************
* Funtion    : LoadUseTimes()
* Description: 加载使用次数
* Argument   : none
* Returns    : none
*******************************************************************************************************/
U16 FlashWriteData(DWORD Address, U16 * pData, U16 Numb)
{
	U16 WriteNumb = 0;
	/* Unlock the Flash to enable the flash control register access *************/ 
  	FLASH_Unlock();
    
  	/* Erase the user Flash area
    	(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  	/* Clear pending flags (if any) */  
  	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 

  	/* Define the number of page to be erased */
  	//NbrOfPage = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;
  	NbrOfPage = 1;

  	/* Erase the FLASH pages */
  	for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  	{
    		if (FLASH_ErasePage(Address + (FLASH_PAGE_SIZE * EraseCounter))!= FLASH_COMPLETE)
	    	{
	     		/* Error occurred while sector erase. 
	         	User can add here some code to deal with this error  */
	      		//while (1)
	      		{
	      			return FALSE;
	      		}
	    	}
  	}
  	/* Program the user Flash area word by word
    	(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  	//Address = FLASH_USER_START_ADDR;
  	WriteNumb = 0;

  	while (WriteNumb < Numb)
  	{
    		if (FLASH_ProgramHalfWord(Address, *pData) == FLASH_COMPLETE)
    		{
      			Address = Address + 2;
      			pData++;

      			WriteNumb++;
   		 }
    		else
    		{ 
      			/* Error occurred while writing data in Flash memory. 
         		User can add here some code to deal with this error */
      			//while (1)
      			{
      				return FALSE;
      			}
    		}
  	}

  	/* Lock the Flash to disable the flash control register access (recommended
     	to protect the FLASH memory against possible unwanted operation) *********/
  	FLASH_Lock(); 

  	return TRUE;
}

/******************************************************************************************************
* Funtion    : LoadUseTimes()
* Description: 加载使用次数
* Argument   : none
* Returns    : none
*******************************************************************************************************/
U16 FlashReadData(DWORD Address, U16 * pData, U16 Numb)
{
	U16 ReadNumb = 0;
	//Address = FLASH_USER_START_ADDR;
	MemoryProgramStatus = PASSED;
	  
	 while (ReadNumb < Numb)
	 {
	    	* pData = *(__IO U16 *)Address;

	    	//if (Data != DATA_32)
	    	//{
	      	//	MemoryProgramStatus = FAILED;  
	    	//}

	    	Address = Address + 2;
		pData++;
	    	ReadNumb++;
	  }

	  return TRUE;
}

