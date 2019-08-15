/*******************************************************************************
 Copyright (c)  All rights reserved.
 �ļ�����: Dwin.h
 �ļ�����: ����DGUS��ͨѶģ��ͷ�ļ�����
 ������  : 
 ��������: 2017.06.12
*******************************************************************************/
#ifndef DWIN_H
#define DWIN_H	1

#ifdef	DWIN_GLOBAL
	#define	DWIN_EXT
#else	
	#define	DWIN_EXT 	extern
#endif

/* ����������ֵ�Ͳ������� */
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


/* ����Ϊ�����������Ŷ��� WDΪwindow��д��DLΪDIALOG��д */

#define DWIN_WD_POWERON					0
#define DWIN_WD_RUN						1			/* ���н��� */
#define DWIN_WD_MODE					2			/* ģʽѡ�� */
#define DWIN_WD_MODE_IMPACT				3			/* ����ѡ��Ч�� */
#define DWIN_WD_PLACE_SEL				4			/* ��λѡ����� */
#define DWIN_WD_BODY_SEL				10			/* ���岿λѡ�� */
#define DWIN_WD_FACE_SEL				5			/* �沿ѡ�� */
#define DWIN_WD_MAIN					9			/* ���˵� */

#define DWIN_WD_MODE_1					41			/* ģʽѡ��1 */
#define DWIN_WD_MODE_2					42			/* ģʽѡ��2 */
#define DWIN_WD_MODE_3					43			/* ģʽѡ��3 */
#define DWIN_WD_MODE_4					44			/* ģʽѡ��4 */
#define DWIN_WD_MODE_5					45			/* ģʽѡ��5 */
#define DWIN_WD_MODE_6					46			/* ģʽѡ��6 */
#define DWIN_WD_MODE_7					47			/* ģʽѡ��7 */
#define DWIN_WD_MODE_8					48			/* ģʽѡ��8 */

#define DWIN_WD_TERM_SET					74

#define DWIN_WD_DEBUG_PSWD				80

#define DWIN_WD_TERM_PSWD				82
#define DWIN_WD_SET_TERM_PSWD			83

/********************************************************************************/



/* ����ֵ���ض��� */
#define K_NULL				0		/* �ռ� */
#define MAX_LOCAL_KEY		0

#define K_DWIN_CLOSEWND		(0x0001+MAX_LOCAL_KEY) /* �رնԻ���"X" */
#define K_DWIN_PREPAGE	 	(0x0002+MAX_LOCAL_KEY) /* ��ҳ */
#define K_DWIN_NEXTPAGE 	(0x0003+MAX_LOCAL_KEY) /* ��ҳ */
#define K_DWIN_OK	 		(0x0005+MAX_LOCAL_KEY) /* ȷ�� */
#define K_DWIN_CANCEL	 	(0x0006+MAX_LOCAL_KEY) /* ȡ�� */
#define K_DWIN_BACKSPACE	(0x0008+MAX_LOCAL_KEY) /* ���������˸� */

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

                                                   
/* �����水�� */                                   
#define K_DWIN_ABOUT		(0x0020+MAX_LOCAL_KEY) /* ���� */
#define K_DWIN_ALARM 		(0x0021+MAX_LOCAL_KEY) /* ���� */
#define K_DWIN_MAINMENU 	(0x0022+MAX_LOCAL_KEY) /* ���˵� */
#define K_DWIN_TREATMENT 	(0x0023+MAX_LOCAL_KEY) /* ��� */

#define K_DWIN_CONFIRM		(0x0025+MAX_LOCAL_KEY) /* ���� */
#define K_DWIN_START		(0x0026+MAX_LOCAL_KEY) /* ���� */
#define K_DWIN_STOP			(0x0027+MAX_LOCAL_KEY) /* ͣ�� */


#define K_DWIN_FACE			(0x0029+MAX_LOCAL_KEY) /* �沿 */
#define K_DWIN_BODY			(0x0028+MAX_LOCAL_KEY) /* ���� */


/* ģʽѡ����� */
#define K_DWIN_SELECT_1 	(0x0030+MAX_LOCAL_KEY) /* ѡ��1 */
#define K_DWIN_SELECT_2		(0x0031+MAX_LOCAL_KEY) /* ѡ��1  */
#define K_DWIN_SELECT_3		(0x0032+MAX_LOCAL_KEY) /* ѡ��1  */
#define K_DWIN_SELECT_4		(0x0033+MAX_LOCAL_KEY) /* ѡ��1  */
#define K_DWIN_SELECT_5		(0x0034+MAX_LOCAL_KEY) /* ѡ��1  */
#define K_DWIN_SELECT_6		(0x0035+MAX_LOCAL_KEY) /* ѡ��1  */
#define K_DWIN_SELECT_7		(0x0036+MAX_LOCAL_KEY) /* ѡ��1  */
#define K_DWIN_SELECT_8		(0x0037+MAX_LOCAL_KEY) /* ѡ��8  */


#define K_DWIN_SELECT_CON			(0x0038+MAX_LOCAL_KEY) /* ���� */
#define K_DWIN_SELECT_CANCEL		(0x0039+MAX_LOCAL_KEY) /* ���� */

                                                   
                                                 
#define K_DWIN_FOREHEAD 	(0x0040+MAX_LOCAL_KEY) /* ѡ��1 */
#define K_DWIN_EYE			(0x0041+MAX_LOCAL_KEY) /* ѡ��1  */
#define K_DWIN_NOSE			(0x0042+MAX_LOCAL_KEY) /* ѡ��1  */
#define K_DWIN_CHEEK		(0x0043+MAX_LOCAL_KEY) /* ѡ��1  */
#define K_DWIN_LIPS			(0x0044+MAX_LOCAL_KEY) /* ѡ��1  */

#define K_DWIN_NECK			(0x0045+MAX_LOCAL_KEY) /* ѡ��1  */
#define K_DWIN_CHEST		(0x0046+MAX_LOCAL_KEY) /* ѡ��1  */
#define K_DWIN_NAVEL		(0x0047+MAX_LOCAL_KEY) /* ѡ��8  */
#define K_DWIN_VULVA		(0x0048+MAX_LOCAL_KEY) /* ѡ��8  */

#define K_DWIN_POS_CANCEL	(0x0049+MAX_LOCAL_KEY) /* ��λѡ�񷵻� */
#define K_DWIN_POS_ENTER	(0x0050+MAX_LOCAL_KEY) /* ��λselcetȷ�� */

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




/* RAM��ַ���� */
/* ��λ:0x0000~0x0020���ں����ڲ�ʹ�õ�ַ(��ʱû���ú궨��) */

/* ���н���ͨ�����ݣ�ʱ���¶�Һλ�ȣ� */
#define DWIN_ADDR_KEY				0x0001	/* ���� */			/* 0x0090->0x0001 */

/* GD008AX33005_006 �ռ���� */
#define DWIN_ADDR_DAY               0x0002		/* ��λ �� - day */
#define DWIN_ADDR_HOUR              0x0006		/* ��λ ʱ - hour */
#define DWIN_ADDR_MIN			    0x000B		/* ��λ �� - min */
#define DWIN_ADDR_SEC			    0x000F		/* ��λ �� - sec */
#define DWIN_ADDR_MM			    0x0013		/* ��λ ���� - mm */
#define DWIN_ADDR_PEICE_PER_HOUR	0x0016	    /* ��λ ��/ʱ - pcs/h */
#define DWIN_ADDR_PEICE             0x001C      /* ��λ �� - pcs */
#define DWIN_ADDR_MPERMIN			0x0020	    /* ��λ ��/�� - m/min */
#define DWIN_ADDR_YEAR			    0x0028		/* ��λ ��(ʱ����ʾ)-20���� */
#define DWIN_ADDR_COLON			    0x003C		/* ð�� */


/* �Ի��� */
#define DWIN_ADDR_BOX_TITLE			0x0080		/* ��ʾ����� */
#define DWIN_ADDR_BOX_TEXT			0x00A0		/* ��ʾ������ */
#define DWIN_ADDR_BOX_PICTURE		0x00C0		/* ��ʾ��-����ͼ����ʾ */

#define DWIN_ADDR_YES				0x0180		/* �� */
#define DWIN_ADDR_NO				0x0184		/* �� */
#define DWIN_ADDR_SURE				0x0187		/* ȷ�� */




/* �������� */
#define DWIN_ADDR_CNTDOWN				0x1000	/* ����ʱ */
#define DWIN_ADDR_CNTDOWN2				0x1001	/* ����ʱ2 */


/***************************** �������� ********************************************/
#define DWIN_ADDR_TEXS_PSWD_PROMPT 			0x1100			/* �ı�.������ʾ */
#define DWIN_ADDR_TEXT_PSWD					0x1130			/* �ı�.����������ʾ */

/***************************** ������ **********************************************/
#define DWIN_ADDR_KEY_NAME_MAINMENU			0x1200			/* ������.���� */


#define DWIN_ADDR_ICON_START                0x1300          /* ͼ��.������ */
#define DWIN_ADDR_ICON_STOP                 0x1301          /* ͼ��.ֹͣ�� */
#define DWIN_ADDR_ICON_SLIENT               0x1302          /* ͼ��.������ */


#define DWIN_ADDR_ICON_MODE               	0x1310          /* ͼ��.ģʽ */
#define DWIN_ADDR_ICON_PARTER            	0x1320          /* ͼ��.��λ */


#define DWIN_ADDR_DATA_MIN            		0x13B0			/* ����.ʱ����� */
#define DWIN_ADDR_DATA_SEC            		0x13C0			/* ����.ʱ������ */
#define DWIN_ADDR_DATA_TIME            		0x13D0			/* ����.���� */



#define DWIN_ADDR_ICON_MODE_1               0x1400          /* ͼ��.ģʽ1 */
#define DWIN_ADDR_ICON_MODE_2            	0x1401          /* ͼ��.ģʽ2 */
#define DWIN_ADDR_ICON_MODE_3            	0x1402          /* ͼ��.ģʽ2 */
#define DWIN_ADDR_ICON_MODE_4            	0x1403          /* ͼ��.ģʽ2 */
#define DWIN_ADDR_ICON_MODE_5            	0x1404          /* ͼ��.ģʽ2 */
#define DWIN_ADDR_ICON_MODE_6            	0x1405          /* ͼ��.ģʽ2 */
#define DWIN_ADDR_ICON_MODE_7            	0x1406          /* ͼ��.ģʽ2 */
#define DWIN_ADDR_ICON_MODE_8            	0x1407          /* ͼ��.ģʽ2 */


#define DWIN_ADDR_ICON_FACE            		0x1409          /* ͼ��.ģʽ2 */
#define DWIN_ADDR_ICON_BODY            		0x1408          /* ͼ��.ģʽ2 */






/***************************** ʹ������ **************************************************/

#define DWIN_ADDR_TERM_USE_NAME				0x1540			/* �Ƿ�ʹ��	*/

#define DWIN_ADDR_TERM_PSWD_NAME			0x1580			/* ��������	*/
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

