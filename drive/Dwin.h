/*******************************************************************************
 Copyright (c)  All rights reserved.
 文件名称: Dwin.h
 文件描述: 迪文DGUS屏通讯模块头文件声明
 创建人  : 
 创建日期: 2017.06.12
*******************************************************************************/
#ifndef DWIN_H
#define DWIN_H	1

#ifdef	DWIN_GLOBAL
	#define	DWIN_EXT
#else	
	#define	DWIN_EXT 	extern
#endif

/* 迪文设置数值型参数定义 */
typedef struct tagDWINSETNUM
{
	BOOL update;
	U16 addr;
	U16 val;
}DWINSETNUM;

typedef struct tagDWINSETPSWD
{
	BOOL update;
	U16 addr;
	U32 val;
}DWINSETPSWD;


/* 以下为迪文屏界面编号定义 WD为window缩写，DL为DIALOG缩写 */

#define DWIN_WD_POWERON					0
#define DWIN_WD_RUN						1			/* 运行界面 */
#define DWIN_WD_MODE					2			/* 模式选择 */
#define DWIN_WD_MODE_IMPACT				3			/* 运行选择效果 */
#define DWIN_WD_PLACE_SEL				4			/* 部位选择界面 */
#define DWIN_WD_BODY_SEL				10			/* 身体部位选择 */
#define DWIN_WD_FACE_SEL				5			/* 面部选择 */
#define DWIN_WD_MAIN					9			/* 主菜单 */

#define DWIN_WD_MODE_1					41			/* 模式选择1 */
#define DWIN_WD_MODE_2					42			/* 模式选择2 */
#define DWIN_WD_MODE_3					43			/* 模式选择3 */
#define DWIN_WD_MODE_4					44			/* 模式选择4 */
#define DWIN_WD_MODE_5					45			/* 模式选择5 */
#define DWIN_WD_MODE_6					46			/* 模式选择6 */
#define DWIN_WD_MODE_7					47			/* 模式选择7 */
#define DWIN_WD_MODE_8					48			/* 模式选择8 */

#define DWIN_WD_TERM_SET					74

#define DWIN_WD_DEBUG_PSWD				80

#define DWIN_WD_TERM_PSWD				82
#define DWIN_WD_SET_TERM_PSWD			83

/********************************************************************************/



/* 按键值返回定义 */
#define K_NULL				0		/* 空键 */
#define MAX_LOCAL_KEY		0

#define K_DWIN_CLOSEWND		(0x0001+MAX_LOCAL_KEY) /* 关闭对话框"X" */
#define K_DWIN_PREPAGE	 	(0x0002+MAX_LOCAL_KEY) /* 上页 */
#define K_DWIN_NEXTPAGE 	(0x0003+MAX_LOCAL_KEY) /* 下页 */
#define K_DWIN_OK	 		(0x0005+MAX_LOCAL_KEY) /* 确定 */
#define K_DWIN_CANCEL	 	(0x0006+MAX_LOCAL_KEY) /* 取消 */
#define K_DWIN_BACKSPACE	(0x0008+MAX_LOCAL_KEY) /* 密码输入退格 */

/***************************************/

#define K_DWIN_NUM0	 		(0x000A+MAX_LOCAL_KEY)
#define K_DWIN_NUM1	 		(0x000B+MAX_LOCAL_KEY)
#define K_DWIN_NUM2	 		(0x000C+MAX_LOCAL_KEY)
#define K_DWIN_NUM3	 		(0x000D+MAX_LOCAL_KEY)
#define K_DWIN_NUM4	 		(0x000E+MAX_LOCAL_KEY)
#define K_DWIN_NUM5	 		(0x000F+MAX_LOCAL_KEY)
#define K_DWIN_NUM6	 		(0x0010+MAX_LOCAL_KEY)
#define K_DWIN_NUM7	 		(0x0011+MAX_LOCAL_KEY)
#define K_DWIN_NUM8	 		(0x0012+MAX_LOCAL_KEY)
#define K_DWIN_NUM9	 		(0x0013+MAX_LOCAL_KEY)

                                                   
/* 主界面按键 */                                   
#define K_DWIN_ABOUT		(0x0020+MAX_LOCAL_KEY) /* 关于 */
#define K_DWIN_ALARM 		(0x0021+MAX_LOCAL_KEY) /* 报警 */
#define K_DWIN_MAINMENU 	(0x0022+MAX_LOCAL_KEY) /* 主菜单 */
#define K_DWIN_TREATMENT 	(0x0023+MAX_LOCAL_KEY) /* 监控 */

#define K_DWIN_CONFIRM		(0x0025+MAX_LOCAL_KEY) /* 配置 */
#define K_DWIN_START		(0x0026+MAX_LOCAL_KEY) /* 启动 */
#define K_DWIN_STOP			(0x0027+MAX_LOCAL_KEY) /* 停机 */


#define K_DWIN_FACE			(0x0029+MAX_LOCAL_KEY) /* 面部 */
#define K_DWIN_BODY			(0x0028+MAX_LOCAL_KEY) /* 身体 */


/* 模式选择界面 */
#define K_DWIN_SELECT_1 	(0x0030+MAX_LOCAL_KEY) /* 选择1 */
#define K_DWIN_SELECT_2		(0x0031+MAX_LOCAL_KEY) /* 选择1  */
#define K_DWIN_SELECT_3		(0x0032+MAX_LOCAL_KEY) /* 选择1  */
#define K_DWIN_SELECT_4		(0x0033+MAX_LOCAL_KEY) /* 选择1  */
#define K_DWIN_SELECT_5		(0x0034+MAX_LOCAL_KEY) /* 选择1  */
#define K_DWIN_SELECT_6		(0x0035+MAX_LOCAL_KEY) /* 选择1  */
#define K_DWIN_SELECT_7		(0x0036+MAX_LOCAL_KEY) /* 选择1  */
#define K_DWIN_SELECT_8		(0x0037+MAX_LOCAL_KEY) /* 选择8  */


#define K_DWIN_SELECT_CON			(0x0038+MAX_LOCAL_KEY) /* 配置 */
#define K_DWIN_SELECT_CANCEL		(0x0039+MAX_LOCAL_KEY) /* 返回 */

                                                   
                                                 
#define K_DWIN_FOREHEAD 	(0x0040+MAX_LOCAL_KEY) /* 选择1 */
#define K_DWIN_EYE			(0x0041+MAX_LOCAL_KEY) /* 选择1  */
#define K_DWIN_NOSE			(0x0042+MAX_LOCAL_KEY) /* 选择1  */
#define K_DWIN_CHEEK		(0x0043+MAX_LOCAL_KEY) /* 选择1  */
#define K_DWIN_LIPS			(0x0044+MAX_LOCAL_KEY) /* 选择1  */

#define K_DWIN_NECK			(0x0045+MAX_LOCAL_KEY) /* 选择1  */
#define K_DWIN_CHEST		(0x0046+MAX_LOCAL_KEY) /* 选择1  */
#define K_DWIN_NAVEL		(0x0047+MAX_LOCAL_KEY) /* 选择8  */
#define K_DWIN_VULVA		(0x0048+MAX_LOCAL_KEY) /* 选择8  */

#define K_DWIN_POS_CANCEL	(0x0049+MAX_LOCAL_KEY) /* 部位选择返回 */
#define K_DWIN_POS_ENTER	(0x0050+MAX_LOCAL_KEY) /* 部位selcet确认 */

#define K_DWIN_DEBUG1	(0x0060+MAX_LOCAL_KEY)
#define K_DWIN_DEBUG2	(0x0061+MAX_LOCAL_KEY)
#define K_DWIN_DEBUG3	(0x0062+MAX_LOCAL_KEY)


#define K_DWIN_TERM0_SEL	(0x0070+MAX_LOCAL_KEY)
#define K_DWIN_TERM1_SEL	(0x0071+MAX_LOCAL_KEY)
#define K_DWIN_TERM2_SEL	(0x0072+MAX_LOCAL_KEY)
#define K_DWIN_TERM3_SEL	(0x0073+MAX_LOCAL_KEY)
#define K_DWIN_TERM4_SEL	(0x0074+MAX_LOCAL_KEY)
#define K_DWIN_TERM5_SEL	(0x0075+MAX_LOCAL_KEY)
#define K_DWIN_TERM6_SEL	(0x0076+MAX_LOCAL_KEY)
#define K_DWIN_TERM7_SEL	(0x0077+MAX_LOCAL_KEY)
#define K_DWIN_TERM8_SEL	(0x0078+MAX_LOCAL_KEY)
#define K_DWIN_TERM9_SEL	(0x0079+MAX_LOCAL_KEY)

#define K_DWIN_TERM_ENTER	(0x0080+MAX_LOCAL_KEY)

#define K_DWIN_TERM_PAGE_UP	(0x0081+MAX_LOCAL_KEY)

#define K_DWIN_TERM_PAGE_DW	(0x0082+MAX_LOCAL_KEY)



#define DWIN_ADDR_DEBUG_PASSWORD    	0x0A00

#define DWIN_ADDR_TERM_PSWD    		0x0A10

#define DWIN_ADDR_USE_TERM_NUM    	         0x0A20

#define DWIN_ADDR_CUR_TERM_NUM    	         0x0A30

#define DWIN_ADDR_CUR_PSWD    			0x0A50


#define DWIN_ADDR_USE_TIMES0_SEL    	         0x0B00
#define DWIN_ADDR_USE_TIMES0_V_N    	0x0B01
#define DWIN_ADDR_USE_TIMES0_PSWD   	0x0B02

#define DWIN_ADDR_USE_TIMES1_SEL    	         0x0B10
#define DWIN_ADDR_USE_TIMES1_V_N    	0x0B11
#define DWIN_ADDR_USE_TIMES1_PSWD   	0x0B12

#define DWIN_ADDR_USE_TIMES2_SEL    	         0x0B20
#define DWIN_ADDR_USE_TIMES2_V_N    	0x0B21
#define DWIN_ADDR_USE_TIMES2_PSWD   	0x0B22


#define DWIN_ADDR_USE_TIMES3_SEL    	         0x0B30
#define DWIN_ADDR_USE_TIMES3_V_N    	0x0B31
#define DWIN_ADDR_USE_TIMES3_PSWD   	0x0B32

#define DWIN_ADDR_USE_TIMES4_SEL    	         0x0B40
#define DWIN_ADDR_USE_TIMES4_V_N    	0x0B41
#define DWIN_ADDR_USE_TIMES4_PSWD   	0x0B42

#define DWIN_ADDR_USE_TIMES5_SEL    	         0x0B50
#define DWIN_ADDR_USE_TIMES5_V_N    	0x0B51
#define DWIN_ADDR_USE_TIMES5_PSWD   	0x0B52

#define DWIN_ADDR_USE_TIMES6_SEL    	         0x0B60
#define DWIN_ADDR_USE_TIMES6_V_N    	0x0B61
#define DWIN_ADDR_USE_TIMES6_PSWD   	0x0B62

#define DWIN_ADDR_USE_TIMES7_SEL    	         0x0B70
#define DWIN_ADDR_USE_TIMES7_V_N    	0x0B71
#define DWIN_ADDR_USE_TIMES7_PSWD   	0x0B72

#define DWIN_ADDR_USE_TIMES8_SEL    	         0x0B80
#define DWIN_ADDR_USE_TIMES8_V_N    	0x0B81
#define DWIN_ADDR_USE_TIMES8_PSWD   	0x0B82

#define DWIN_ADDR_USE_TIMES9_SEL    	         0x0B90
#define DWIN_ADDR_USE_TIMES9_V_N    	0x0B91
#define DWIN_ADDR_USE_TIMES9_PSWD   	0x0B92




/* RAM地址定义 */
/* 单位:0x0000~0x0020，在函数内部使用地址(暂时没有用宏定义) */

/* 运行界面通用数据（时间温度液位等） */
#define DWIN_ADDR_KEY				0x0001	/* 按键 */			/* 0x0090->0x0001 */

/* GD008AX33005_006 空间紧张 */
#define DWIN_ADDR_DAY               0x0002		/* 单位 天 - day */
#define DWIN_ADDR_HOUR              0x0006		/* 单位 时 - hour */
#define DWIN_ADDR_MIN			    0x000B		/* 单位 分 - min */
#define DWIN_ADDR_SEC			    0x000F		/* 单位 秒 - sec */
#define DWIN_ADDR_MM			    0x0013		/* 单位 毫米 - mm */
#define DWIN_ADDR_PEICE_PER_HOUR	0x0016	    /* 单位 张/时 - pcs/h */
#define DWIN_ADDR_PEICE             0x001C      /* 单位 张 - pcs */
#define DWIN_ADDR_MPERMIN			0x0020	    /* 单位 米/分 - m/min */
#define DWIN_ADDR_YEAR			    0x0028		/* 单位 年(时间显示)-20个字 */
#define DWIN_ADDR_COLON			    0x003C		/* 冒号 */


/* 对话框 */
#define DWIN_ADDR_BOX_TITLE			0x0080		/* 提示框标题 */
#define DWIN_ADDR_BOX_TEXT			0x00A0		/* 提示框内容 */
#define DWIN_ADDR_BOX_PICTURE		0x00C0		/* 提示框-基本图形显示 */

#define DWIN_ADDR_YES				0x0180		/* 是 */
#define DWIN_ADDR_NO				0x0184		/* 否 */
#define DWIN_ADDR_SURE				0x0187		/* 确定 */




/* 开机界面 */
#define DWIN_ADDR_CNTDOWN				0x1000	/* 倒计时 */
#define DWIN_ADDR_CNTDOWN2				0x1001	/* 倒计时2 */


/***************************** 密码输入 ********************************************/
#define DWIN_ADDR_TEXS_PSWD_PROMPT 			0x1100			/* 文本.密码提示 */
#define DWIN_ADDR_TEXT_PSWD					0x1130			/* 文本.密码输入显示 */

/***************************** 主运行 **********************************************/
#define DWIN_ADDR_KEY_NAME_MAINMENU			0x1200			/* 按键名.启动 */


#define DWIN_ADDR_ICON_START                0x1300          /* 图标.启动键 */
#define DWIN_ADDR_ICON_STOP                 0x1301          /* 图标.停止键 */
#define DWIN_ADDR_ICON_SLIENT               0x1302          /* 图标.消音键 */


#define DWIN_ADDR_ICON_MODE               	0x1310          /* 图标.模式 */
#define DWIN_ADDR_ICON_PARTER            	0x1320          /* 图标.部位 */


#define DWIN_ADDR_DATA_MIN            		0x13B0			/* 数据.时间分钟 */
#define DWIN_ADDR_DATA_SEC            		0x13C0			/* 数据.时间秒钟 */
#define DWIN_ADDR_DATA_TIME            		0x13D0			/* 数据.次数 */



#define DWIN_ADDR_ICON_MODE_1               0x1400          /* 图标.模式1 */
#define DWIN_ADDR_ICON_MODE_2            	0x1401          /* 图标.模式2 */
#define DWIN_ADDR_ICON_MODE_3            	0x1402          /* 图标.模式2 */
#define DWIN_ADDR_ICON_MODE_4            	0x1403          /* 图标.模式2 */
#define DWIN_ADDR_ICON_MODE_5            	0x1404          /* 图标.模式2 */
#define DWIN_ADDR_ICON_MODE_6            	0x1405          /* 图标.模式2 */
#define DWIN_ADDR_ICON_MODE_7            	0x1406          /* 图标.模式2 */
#define DWIN_ADDR_ICON_MODE_8            	0x1407          /* 图标.模式2 */


#define DWIN_ADDR_ICON_FACE            		0x1409          /* 图标.模式2 */
#define DWIN_ADDR_ICON_BODY            		0x1408          /* 图标.模式2 */






/***************************** 使用期限 **************************************************/

#define DWIN_ADDR_TERM_USE_NAME				0x1540			/* 是否使用	*/

#define DWIN_ADDR_TERM_PSWD_NAME			0x1580			/* 解锁密码	*/
#define DWIN_ADDR_TERM1_USE					0x15A0			/*  */

extern DWINSETPSWD  SetPswdData;

extern DWINSETNUM SetNum;


void DWIN_UartRx(void);
void DWIN_UartTx(void);
void DWIN_ClrRecord(void);
void DWIN_CommuProg(void);
BOOL DWIN_WaitOver(void);
void DWIN_Init(void);
BOOL DWIN_QEmpty(void);
BOOL DWIN_QFull(void);
U08  DWIN_QGetNodeCnt(void);
BOOL DWIN_WriteRAM(U16 startAddr, U08 Length, U16 *pData );
BOOL DWIN_WriteOneRAM(U16 startAddr, U16 val);
BOOL DWIN_WriteOneREG(U16 startAddr, U16 val);
U16 DWIN_ReadKey(void);
void DWIN_SetVal(U16 address, U16 val);

#endif

