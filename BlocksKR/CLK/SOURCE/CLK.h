/*
*********************************************************************************************************
*                                             Clock/Calendar
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* Filename   : CLK.H
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#ifndef  CFG_H

#define  CLK_DLY_TICKS       OS_TICKS_PER_SEC    /* 1�� ����� ���� Ŭ�� ƽ�� ��                       */
#define  CLK_TASK_PRIO        50                 /* ClkTask() �½�ũ�� �켱����                        */
#define  CLK_TASK_STK_SIZE   512                 /* ClkTask() ���� ũ��                                */

#define  CLK_DATE_EN           1                 /* DATE Ȱ��ȭ(1�� ���)                              */
#define  CLK_TS_EN             1                 /* Ÿ�ӽ����� Ȱ��ȭ (1�� ���)                       */
#define  CLK_USE_DLY           1                 /* �½�ũ�� �������� ��� OSTimeDly()�� ����Ѵ�      */

#endif

#ifdef   CLK_GLOBALS
#define  CLK_EXT
#else
#define  CLK_EXT  extern
#endif

/*
*********************************************************************************************************
*                                               ������ Ÿ��
*********************************************************************************************************
*/

typedef  INT32U  TS;                   /* Ÿ�ӽ����� ����                                              */


#if CLK_DATE_EN
typedef struct clk_month {             /* �޿� ����� ����                                             */
    INT8U  MonthDays;                  /* �� ���� ��¥ ��                                              */
    char  *MonthName;                  /* ���� �̸�                                                    */
    INT8U  MonthVal;                   /* ������ ����ϱ� ���� ��                                      */
} CLK_MONTH;
#endif

/*
*********************************************************************************************************
*                                                ��������
*********************************************************************************************************
*/

CLK_EXT  INT8U   ClkHr;
CLK_EXT  INT8U   ClkMin;
CLK_EXT  INT8U   ClkSec;               /* �ð��� ī����                                                */

#if      CLK_DATE_EN
CLK_EXT  INT8U   ClkDay;               /* ��¥�� ī����                                                */
CLK_EXT  INT8U   ClkDOW;               /* ����(0 �� �Ͽ���)                                            */
CLK_EXT  INT8U   ClkMonth;
CLK_EXT  INT16U  ClkYear;
#endif

#if      CLK_TS_EN
CLK_EXT  TS      ClkTS;                /* ������ Ÿ�ӽ�����                                            */
#endif

/*
*********************************************************************************************************
*                                                �Լ�����
*********************************************************************************************************
*/

void   ClkInit(void);

void   ClkFormatTime(INT8U n, char *s);
void   ClkSetTime(INT8U hr, INT8U min, INT8U sec);

void   ClkSignalClk(void);

#if    CLK_DATE_EN
void   ClkFormatDate(INT8U n, char *s);
void   ClkSetDate(INT8U month, INT8U day, INT16U year);
void   ClkSetDateTime(INT8U month, INT8U day, INT16U year, INT8U hr, INT8U min, INT8U sec);
#endif

#if    CLK_TS_EN
TS     ClkGetTS(void);
TS     ClkMakeTS(INT8U month, INT8U day, INT16U year, INT8U hr, INT8U min, INT8U sec);
void   ClkFormatTS(INT8U n, TS ts, char *s);
#endif
