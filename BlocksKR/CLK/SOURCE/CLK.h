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

#define  CLK_DLY_TICKS       OS_TICKS_PER_SEC    /* 1초 경과를 위한 클럭 틱의 수                       */
#define  CLK_TASK_PRIO        50                 /* ClkTask() 태스크의 우선순위                        */
#define  CLK_TASK_STK_SIZE   512                 /* ClkTask() 스택 크기                                */

#define  CLK_DATE_EN           1                 /* DATE 활성화(1일 경우)                              */
#define  CLK_TS_EN             1                 /* 타임스탬프 활성화 (1일 경우)                       */
#define  CLK_USE_DLY           1                 /* 태스크는 세마포어 대신 OSTimeDly()를 사용한다      */

#endif

#ifdef   CLK_GLOBALS
#define  CLK_EXT
#else
#define  CLK_EXT  extern
#endif

/*
*********************************************************************************************************
*                                               데이터 타입
*********************************************************************************************************
*/

typedef  INT32U  TS;                   /* 타임스탬프 정의                                              */


#if CLK_DATE_EN
typedef struct clk_month {             /* 달에 관계된 변수                                             */
    INT8U  MonthDays;                  /* 각 달의 날짜 수                                              */
    char  *MonthName;                  /* 달의 이름                                                    */
    INT8U  MonthVal;                   /* 요일을 계산하기 위한 값                                      */
} CLK_MONTH;
#endif

/*
*********************************************************************************************************
*                                                전역변수
*********************************************************************************************************
*/

CLK_EXT  INT8U   ClkHr;
CLK_EXT  INT8U   ClkMin;
CLK_EXT  INT8U   ClkSec;               /* 시간용 카운터                                                */

#if      CLK_DATE_EN
CLK_EXT  INT8U   ClkDay;               /* 날짜용 카운터                                                */
CLK_EXT  INT8U   ClkDOW;               /* 요일(0 은 일요일)                                            */
CLK_EXT  INT8U   ClkMonth;
CLK_EXT  INT16U  ClkYear;
#endif

#if      CLK_TS_EN
CLK_EXT  TS      ClkTS;                /* 현재의 타임스탬프                                            */
#endif

/*
*********************************************************************************************************
*                                                함수원형
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
