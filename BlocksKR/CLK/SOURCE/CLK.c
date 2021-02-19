/*
*********************************************************************************************************
*                                             Clock/Calendar
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* Filename   : CLK.C
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              ��ũ��� ����
*********************************************************************************************************
*/

#define  CLK_GLOBALS                   /* CLK.H�� ���������� ���念���� Ȯ���Ѵ�                       */
#include "includes.h"

/*
*********************************************************************************************************
*                                               �������
*********************************************************************************************************
*/

#define  CLK_TS_BASE_YEAR    2000      /* Ÿ�ӽ����� ���۳⵵                                          */

/*
*********************************************************************************************************
*                                               ��������
*********************************************************************************************************
*/

static  OS_EVENT   *ClkSem;            /* Ŭ���� �׼����ϱ� ���� ��������                              */
static  OS_EVENT   *ClkSemSec;         /* �ʸ� �����ϱ� ���� ī���� ��������                           */

static  OS_STK      ClkTaskStk[CLK_TASK_STK_SIZE];

static  INT8U       ClkTickCtr;        /* �ý��� Ŭ�� ƽ�� �����ϱ� ���� ī����                        */


/*$PAGE*/
/*
*********************************************************************************************************
*                                             ���� ���̺�
*********************************************************************************************************
*/

#if CLK_DATE_EN
static  char *ClkDOWTbl[] = {          /* ���� �̸�                                                    */
    "Sunday ",
    "Monday ",
    "Tuesday ",
    "Wednesday ",
    "Thursday ",
    "Friday ",
    "Saturday "
};


static  CLK_MONTH ClkMonthTbl[] = {    /* ���� ���̺�                                                  */
    {0,  "",           0},             /* ���� ��                                                      */
    {31, "January ",   6},             /* 1��                                                          */
    {28, "February ",  2},             /* 2�� (������ �ڵ忡 ���� ó����)                              */
    {31, "March ",     2},             /* 3��                                                          */
    {30, "April ",     5},             /* 4��                                                          */
    {31, "May ",       0},             /* 5��                                                          */
    {30, "June ",      3},             /* 6��                                                          */
    {31, "July ",      5},             /* 7��                                                          */
    {31, "August ",    1},             /* 8��                                                          */
    {30, "September ", 4},             /* 9��                                                          */
    {31, "October ",   6},             /* 10��                                                         */
    {30, "November ",  2},             /* 11��                                                         */
    {31, "December ",  4}              /* 12��                                                         */
};
#endif

/*
*********************************************************************************************************
*                                             �����Լ� ����
*********************************************************************************************************
*/

        void          ClkTask(void *data);
static  BOOLEAN       ClkUpdateTime(void);

#if     CLK_DATE_EN
static  BOOLEAN       ClkIsLeapYear(INT16U year);
static  void          ClkUpdateDate(void);
static  void          ClkUpdateDOW(void);
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                     ������ ��¥�� ���ڿ��� ������
*
* ���� : ���� ��¥�� ASCII ���ڿ��� �����Ѵ�.
* ��������   : n ���� ����:
*                 1   ��¥�� "MM-DD-YY" �������� �����Ѵ�           (�ּ��� 9���� �ʿ�)
*                 2   ��¥�� "Day Month DD, YYYY" �������� �����Ѵ� (�ּ��� 30���� �ʿ�)
*                 3   ��¥�� "YYYY-MM-DD" �������� �����Ѵ�         (�ּ��� 11���� �ʿ�)
*              s ������ ����� �����ϰ��� �ϴ� ���ڿ��� ������.
*                ���ڿ��� ������ ����� ������ �� �ִ� ����� �޸𸮰� �Ҵ�Ǿ�� �Ѵ�.
*
* ���ϰ�     : ����.
* ��Ʈ       : - ����� ������ �����ϰ� �߰��� �� �ֵ��� switch���� ����Ѵ�.
*                ���� ���, �� ���Ŀ� �ٸ� �������� ��ȣ�� �߰������ν� ��������, �����ξ�, ���Ͼ� ��
*                ����� �����ǿ� ��ȯ����� ������ �� �ִ�.
*              - �� �Լ��� strcpy(), strcat(), itoa() ���� �Լ��� ������ �����Լ���� �����Ѵ�.
*********************************************************************************************************
*/

#if  CLK_DATE_EN
void  ClkFormatDate (INT8U n, char *s)
{
    INT8U   err;
    INT16U  year;
    char    str[5];


    OSSemPend(ClkSem, 0, &err);                  /* TOD Ŭ���� ��Ÿ�� �׼����� ��´�                  */
    switch (n) {
        case  1:
              strcpy(s, "MM-DD-YY");             /* ���õ� ������ ���ø��� �����Ѵ�                    */
              s[0] = ClkMonth / 10 + '0';        /* ��¥�� ASCII�� ��ȯ�Ѵ�                            */
              s[1] = ClkMonth % 10 + '0';
              s[3] = ClkDay   / 10 + '0';
              s[4] = ClkDay   % 10 + '0';
              year = ClkYear % 100;
              s[6] = year / 10 + '0';
              s[7] = year % 10 + '0';
              break;

        case  2:
              strcpy(s, ClkDOWTbl[ClkDOW]);                  /* ������ ��´�                          */
              strcat(s, ClkMonthTbl[ClkMonth].MonthName);    /* ���̸��� ��´�                        */
              if (ClkDay < 10) {
                 str[0] = ClkDay + '0';
                 str[1] = 0;
              } else {
                 str[0] = ClkDay / 10 + '0';
                 str[1] = ClkDay % 10 + '0';
                 str[2] = 0;
              }
              strcat(s, str);
              strcat(s, ", ");
              itoa(ClkYear, str, 10);
              strcat(s, str);
              break;

        case  3:
              strcpy(s, "YYYY-MM-DD");           /* ���õ� ������ ���ø��� �����Ѵ�                    */
              s[0] = year / 1000 + '0';
              year = year % 1000;
              s[1] = year /  100 + '0';
              year = year %  100;
              s[2] = year /   10 + '0';
              s[3] = year %   10 + '0';
              s[5] = ClkMonth / 10 + '0';        /* ��¥�� ASCII�� ��ȯ�Ѵ�                            */
              s[6] = ClkMonth % 10 + '0';
              s[8] = ClkDay   / 10 + '0';
              s[9] = ClkDay   % 10 + '0';
              break;

        default:
              strcpy(s, "?");
              break;
    }
    OSSemPost(ClkSem);                           /* �׼����� �絵�Ѵ�                                  */
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                        ���� �ð��� ���ڿ��� �����Ѵ�
*
* ���� : ����ð��� ASCII ���ڿ��� �����Ѵ�.
* ��������   : n ��������:
*                 1  �ð��� "HH:MM:SS" �������� �����Ѵ�     (24 Hour format)
*                                                            (�ּ��� 9���� �ʿ�)
*                 2  �ð��� "HH:MM:SS AM" �������� �����Ѵ�  (AM/PM ǥ��)
*                                                            (�ּ��� 13���� �ʿ�)
*              s ������ ����� �����ϰ��� �ϴ� ���ڿ��� ������.
*                ���ڿ��� ������ ����� ������ �� �ִ� ����� �޸𸮰� �Ҵ�Ǿ�� �Ѵ�.
*
* ���ϰ�     : ����.
* ��Ʈ       : - ����� ������ �����ϰ� �߰��� �� �ֵ��� switch���� ����Ѵ�.
*              - �� �Լ��� strcpy() �Լ��� ������ �����Լ���� �����Ѵ�.
*********************************************************************************************************
*/

void  ClkFormatTime (INT8U n, char *s)
{
    INT8U err;
    INT8U hr;


    OSSemPend(ClkSem, 0, &err);                       /* TOD Ŭ���� ��Ÿ�� �׼����� ��´�             */
    switch (n) {
        case  1:
              strcpy(s, "HH:MM:SS");                  /* ���õ� ������ ���ø��� �����Ѵ�               */
              s[0] = ClkHr  / 10 + '0';               /* �ð��� ASCII�� ��ȯ�Ѵ�                       */
              s[1] = ClkHr  % 10 + '0';
              s[3] = ClkMin / 10 + '0';
              s[4] = ClkMin % 10 + '0';
              s[6] = ClkSec / 10 + '0';
              s[7] = ClkSec % 10 + '0';
              break;

        case  2:
              strcpy(s, "HH:MM:SS AM");               /* ���õ� ������ ���ø��� �����Ѵ�               */
              s[9] = (ClkHr >= 12) ? 'P' : 'A';       /* AM �Ǵ� PM �� �����Ѵ�                        */
              if (ClkHr > 12) {                       /* ǥ�õ� �ð��� �����Ѵ�                        */
                  hr   = ClkHr - 12;
              } else {
                  hr = ClkHr;
              }
              s[0] = hr     / 10 + '0';               /* �ð��� ASCII�� ��ȯ�Ѵ�                       */
              s[1] = hr     % 10 + '0';
              s[3] = ClkMin / 10 + '0';
              s[4] = ClkMin % 10 + '0';
              s[6] = ClkSec / 10 + '0';
              s[7] = ClkSec % 10 + '0';
              break;

        default:
              strcpy(s, "?");
              break;
    }
    OSSemPost(ClkSem);                                /* �׼����� �絵�Ѵ�                             */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         Ÿ�ӽ������� �����Ѵ�
*
* ���� : �� �Լ��� Ÿ�ӽ������� ASCII ���ڿ��� �����Ѵ�.
* ��������   : n   ��������:
*                   1 : "MM-DD-YY HH:MM:SS"         (�ּ��� 18���� �ʿ�)
*                   2 : "YYYY-MM-DD HH:MM:SS"       (�ּ��� 20���� �ʿ�)
*              ts  �����ϰ��� �ϴ� Ÿ�ӽ����� ��
*              s   ������ ����� �����ϰ��� �ϴ� ���ڿ��� ������.
* ���ϰ�     : ����
* ��Ʈ       : - Ÿ�ӽ������� ������ ���� ���ǵ� 32��Ʈ ����ȣ �����̴�:
*
*      �ʵ�     : -------Year------ ---Month--- ------Day----- ----Hours----- ---Minutes--- --Seconds--
*      ��Ʈ��ȣ : 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
*
*               - ������ CLK_TS_BASE_YEAR���� ���۵ȴ�.  ����, ���� ��Ʈ 31..26�� 0�� ���� �ƴٸ�
*                 ���� ������ CLK_TS_BASE_YEAR�̴�.  ���� ��Ʈ 31..26�� 13�� ���� �ƴٸ�
*                 ���� ������ CLK_TS_BASE_YEAR + 13 �̴�.
*********************************************************************************************************
*/

#if CLK_TS_EN && CLK_DATE_EN
void  ClkFormatTS (INT8U n, TS ts, char *s)
{
    INT16U yr;
    INT8U month;
    INT8U day;
    INT8U hr;
    INT8U min;
    INT8U sec;


    yr    = CLK_TS_BASE_YEAR + (ts >> 26);       /* Ÿ�ӽ������� ������ Ǭ��                           */
    month = (ts >> 22) & 0x0F;
    day   = (ts >> 17) & 0x1F;
    hr    = (ts >> 12) & 0x1F;
    min   = (ts >>  6) & 0x3F;
    sec   = (ts & 0x3F);
    switch (n) {
        case  1:
              strcpy(s, "MM-DD-YY HH:MM:SS");    /* ���õ� ������ ���ø��� �����Ѵ�                    */
              yr    = yr % 100;
              s[ 0] = month / 10 + '0';          /* ��¥�� ASCII�� ��ȯ�Ѵ�                            */
              s[ 1] = month % 10 + '0';
              s[ 3] = day   / 10 + '0';
              s[ 4] = day   % 10 + '0';
              s[ 6] = yr    / 10 + '0';
              s[ 7] = yr    % 10 + '0';
              s[ 9] = hr    / 10 + '0';           /* �ð��� ASCII�� ��ȯ�Ѵ�                            */
              s[10] = hr    % 10 + '0';
              s[12] = min   / 10 + '0';
              s[13] = min   % 10 + '0';
              s[15] = sec   / 10 + '0';
              s[16] = sec   % 10 + '0';
              break;

        case  2:
              strcpy(s, "YYYY-MM-DD HH:MM:SS");  /* ���õ� ������ ���ø��� �����Ѵ�                    */
              s[ 0] = yr    / 1000 + '0';        /* ��¥�� ASCII�� ��ȯ�Ѵ�                            */
              yr    = yr % 1000;
              s[ 1] = yr    /  100 + '0';
              yr    = yr %  100;
              s[ 2] = yr    /   10 + '0';
              s[ 3] = yr    %   10 + '0';
              s[ 5] = month / 10 + '0';          
              s[ 6] = month % 10 + '0';
              s[ 8] = day   / 10 + '0';
              s[ 9] = day   % 10 + '0';
              s[11] = hr    / 10 + '0';          /* �ð��� ASCII�� ��ȯ�Ѵ�                            */
              s[12] = hr    % 10 + '0';
              s[14] = min   / 10 + '0';
              s[15] = min   % 10 + '0';
              s[17] = sec   / 10 + '0';
              s[18] = sec   % 10 + '0';
              break;

        default:
              strcpy(s, "?");
              break;
    }
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                           Ÿ�ӽ������� ��´�
*
* ���� : �� �Լ��� Ÿ�ӽ������� �������α׷����� �����ش�.
*        Ÿ�ӽ������� ������ ������ ���� ���ǵȴ�:
*
*        �ʵ�     : -------Year------ ---Month--- ------Day----- ----Hours----- ---Minutes--- --Seconds--
*        ��Ʈ��ȣ : 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
*
* ��������   : ����.
* ���ϰ�     : ����.
* ��Ʈ       : - ������ CLK_TS_BASE_YEAR���� ���۵ȴ�.  ����, ���� ��Ʈ 31..26�� 0�� ���� �ƴٸ�
*                ���� ������ CLK_TS_BASE_YEAR�̴�.  ���� ��Ʈ 31..26�� 13�� ���� �ƴٸ�
*                ���� ������ CLK_TS_BASE_YEAR + 13 �̴�.
*********************************************************************************************************
*/

#if CLK_TS_EN && CLK_DATE_EN
TS  ClkGetTS (void)
{
    TS ts;


    OS_ENTER_CRITICAL();
    ts = ClkTS;
    OS_EXIT_CRITICAL();
    return (ts);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                         �ð�/�޷� ��� �ʱ�ȭ
*                                           TOD Ŭ�� �ʱ�ȭ
*
* ���� : �� �Լ��� �ð� ����� �ʱ�ȭ �Ѵ�.  
*        TOD �½�ũ�� �� �Լ��� ���� �����ȴ�.
* ��������   : ����
* ���ϰ�     : ����.
*********************************************************************************************************
*/

void  ClkInit (void)
{
    ClkSem     = OSSemCreate(1);       /* TOD Ŭ�� ������� �����Ѵ�                                 */
    ClkSemSec  = OSSemCreate(0);       /* 1�ʰ� �������� ��ȣ�ϱ� ���� ī���� ������� �����Ѵ�      */
    ClkTickCtr =    0;
    ClkSec     =    0;
    ClkMin     =    0;
    ClkHr      =    0;
#if CLK_DATE_EN
    ClkDay     =    1;
    ClkMonth   =    1;
    ClkYear    = 1999;
#endif
#if CLK_TS_EN && CLK_DATE_EN
    ClkTS      = ClkMakeTS(ClkMonth, ClkDay, ClkYear, ClkHr, ClkMin, ClkSec);
#endif
    OSTaskCreate(ClkTask, (void *)0, &ClkTaskStk[CLK_TASK_STK_SIZE], CLK_TASK_PRIO);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           ���� ������ �����Ѵ�
*
* ���� : �� �Լ��� �������ڷ� �Ѿ�� ������ ���� ������ �����Ѵ�.
* ��������   : year    ����.
* ���ϰ�     : TRUE    'year' �� ������ ���.
*              FALSE   'year' �� ������ �ƴ� ���.
*********************************************************************************************************
*/
#if CLK_DATE_EN
static  BOOLEAN  ClkIsLeapYear(INT16U year)
{
    if (!(year % 4) && (year % 100) || !(year % 400)) {
        return TRUE;
    } else {
        return (FALSE);
    }
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                           Ÿ�ӽ������� �����
*
* ����      : �� �Լ��� ����ڰ� ������ ��¥�� �ð��� Ÿ�ӽ�������� �ϴ� 32��Ʈ ������
*             �����Ѵ�.
* ��������  : month     ���ϴ� ��           (1..12)
*             day       ���ϴ� ��           (1..31)
*             year      ���ϴ� �⵵         (CLK_TS_BASE_YEAR .. CLK_TS_BASE_YEAR+63)
*             hr        ���ϴ� �ð�         (0..23)
*             min       ���ϴ� ��           (0..59)
*             sec       ���ϴ� ��           (0..59)
* ���ϰ�    : �� �Լ��� ���޵� ���ڰ� ��ȯ�� Ÿ�ӽ�����.
* ��Ʈ      : - The time stamp is formatted as follows using a 32 bit unsigned integer:
* ��Ʈ      : - Ÿ�ӽ������� ������ 32��Ʈ ������ �̿��ؼ� ������ ���� ���ǵȴ�:
*
*        �ʵ�     : -------Year------ ---Month--- ------Day----- ----Hours----- ---Minutes--- --Seconds--
*        ��Ʈ��ȣ : 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
*
*             - ������ CLK_TS_BASE_YEAR���� ���۵ȴ�.  ����, ���� ��Ʈ 31..26�� 0�� ���� �ƴٸ�
*               ���� ������ CLK_TS_BASE_YEAR�̴�.  ���� ��Ʈ 31..26�� 13�� ���� �ƴٸ�
*               ���� ������ CLK_TS_BASE_YEAR + 13 �̴�.
*********************************************************************************************************
*/

#if CLK_TS_EN && CLK_DATE_EN
TS  ClkMakeTS (INT8U month, INT8U day, INT16U yr, INT8U hr, INT8U min, INT8U sec)
{
    TS ts;


    yr -= CLK_TS_BASE_YEAR;
    ts  = ((INT32U)yr << 26) | ((INT32U)month << 22) | ((INT32U)day << 17);
    ts |= ((INT32U)hr << 12) | ((INT32U)min   <<  6) |  (INT32U)sec;
    return (ts);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                             ��¥�� ��Ʈ�Ѵ�
*
* ����       : TOD Ŭ���� ��¥�� �����Ѵ�
* ��������   : month   ���ϴ� ��              (1..12)
*              day     ���ϴ� ��              (1..31)
*              year    ���ϴ� �⵵            (CLK_TS_BASE_YEAR .. CLK_TS_BASE_YEAR+63)
* ���ϰ�     : ����.
* ��Ʈ       : �� �Լ��� ����ڰ� ��¥�� ��Ȯ�� �Է��ϴ� ������ �����Ѵ�.
*              ��, �� �Լ������� ������ �˻����� �ʴ´�.
*********************************************************************************************************
*/

#if  CLK_DATE_EN
void  ClkSetDate (INT8U month, INT8U day, INT16U year)
{
    INT8U err;


    OSSemPend(ClkSem, 0, &err);                  /* TOD Ŭ���� ��Ÿ�� �׼����� ��´�                  */
    ClkMonth = month;
    ClkDay   = day;
    ClkYear  = year;
    ClkUpdateDOW();                              /* ������ ����Ѵ�(Sunday ...)                        */
    OSSemPost(ClkSem);                           /* �׼����� �絵�Ѵ�                                  */
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                           ��¥�� �ð� ����
*
* ����       : TOD Ŭ���� ��¥�� �ð��� �����Ѵ�
* ��������   : month     ���ϴ� ��              (1..12)
*              day       ���ϴ� ��              (1..31)
*              year      ���ϴ� ����            (2xxx)
*              hr        ���ϴ� �ð�            (0..23)
*              min       ���ϴ� ��              (0..59)
*              sec       ���ϴ� ��              (0..59)
* ���ϰ�     : ����.
* ��Ʈ       : �� �Լ��� ����ڰ� ��¥�� ��Ȯ�� �Է��ϴ� ������ �����Ѵ�.
*              ��, �� �Լ������� ������ �˻����� �ʴ´�.
*********************************************************************************************************
*/

#if  CLK_DATE_EN
void  ClkSetDateTime (INT8U month, INT8U day, INT16U year, INT8U hr, INT8U min, INT8U sec)
{
    INT8U err;


    OSSemPend(ClkSem, 0, &err);                  /* TOD Ŭ���� ��Ÿ�� �׼����� ��´�                  */
    ClkMonth = month;
    ClkDay   = day;
    ClkYear  = year;
    ClkHr    = hr;
    ClkMin   = min;
    ClkSec   = sec;
    ClkUpdateDOW();                              /* ������ ����Ѵ�(Sunday ...)                        */
    OSSemPost(ClkSem);                           /* �׼����� �絵�Ѵ�                                  */
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                              �ð� ����
*
* ����       : TOD Ŭ���� �ð��� ����
* ��������   : hr        ���ϴ� �ð�    (0..23)
*              min       ���ϴ� ��      (0..59)
*              sec       ���ϴ� ��      (0..59)
* ���ϰ�     : ����.
* ��Ʈ       : �� �Լ��� ����ڰ� ��¥�� ��Ȯ�� �Է��ϴ� ������ �����Ѵ�.
*              ��, �� �Լ������� ������ �˻����� �ʴ´�.
*********************************************************************************************************
*/

void  ClkSetTime (INT8U hr, INT8U min, INT8U sec)
{
    OS_ENTER_CRITICAL();                         /* TOD Ŭ���� ��Ÿ�� �׼����� ��´�                  */
    ClkHr  = hr;
    ClkMin = min;
    ClkSec = sec;
    OS_EXIT_CRITICAL();                          /* �׼����� �絵�Ѵ�                                  */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                            'Ŭ�� ƽ'�� �߻��� ���� Ŭ�� ��⿡�� ��ȣ�� �ش�
*
* ����       : �� �Լ��� �� ƽ���� ƽ ISR�� ���ؼ� ȣ��ȴ�.
*              �� �Լ��� ���� �� Ŭ�� ƽ�� ���� ���� �Ѵ�.
*              1�ʰ� ������ TOD Ŭ�� �½�ũ���� ��ȣ�� �����ش�.
* ��������   : ����.
* ���ϰ�     : ����.
* ��Ʈ       : CLK_DLY_TICKS�� ���� �� �߻��ϴ� Ŭ��ƽ�� ������ �����Ѵ�.
*              ���� ���� uC/OS-II�� ����Ѵٸ�  �� ���� OS_TICKS_PER_SEC�� �� ���̴�.
*********************************************************************************************************
*/

void  ClkSignalClk (void)
{
    ClkTickCtr++;                           /* 1�� ���� Ŭ�� ƽ�� ī��Ʈ �Ѵ�                          */
    if (ClkTickCtr >= CLK_DLY_TICKS) {
        ClkTickCtr = 0;
        OSSemPost(ClkSemSec);               /* 1�ʰ� ���� ���� �˷��ش�                                */
    }
}

/*
*********************************************************************************************************
*                                        TIME-OF-DAY Ŭ�� �½�ũ
*
* ����       : �� �Լ��� ClkInit()�� ���ؼ� ȣ��Ǹ� �ð��� ��¥�� �����ϴ� ���� �Ѵ�.
*              ClkTask()�� 1�ʸ��� ����ȴ�.
* ��������   : ����.
* ���ϰ�     : ����.
* ��Ʈ       : CLK_DLY_TICKS�� 1�ʵ��� �����ǵ��� �����Ǿ�� �Ѵ�.
*********************************************************************************************************
*/

void  ClkTask (void *data)
{
    INT8U err;


    data = data;                            /* �����Ϸ� ����� ���Ѵ� (uC/OS �䱸����)                 */
    for (;;) {

#if CLK_USE_DLY
        OSTimeDlyHMSM(0, 0, 1, 0);          /* 1�ʰ� �����ȴ�                                          */
#else
        OSSemPend(ClkSemSec, 0, &err);      /* 1�ʰ� �����⸦ ��ٸ���                                 */
#endif

        OSSemPend(ClkSem, 0, &err);         /* TOD Ŭ���� ��Ÿ�� �׼����� ��´�                       */
        if (ClkUpdateTime() == TRUE) {      /* �ð��� �����Ѵ�(HH:MM:SS)                               */
#if CLK_DATE_EN
            ClkUpdateDate();                /* ���ο� ���̸� ��¥�� �����Ѵ�(MM-DD-YY)                 */
#endif
        }
#if CLK_TS_EN && CLK_DATE_EN
        ClkTS = ClkMakeTS(ClkMonth, ClkDay, ClkYear, ClkHr, ClkMin, ClkSec);
#endif
        OSSemPost(ClkSem);                  /* �׼����� �絵�Ѵ�                                       */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                               ��¥ ����
*
* ����       : �� �Լ��� ��¥�� �����ϱ� ���� ȣ��ȴ�
* ��������   : ����.
* ���ϰ�     : ����.
* ��Ʈ       : �� �Լ��� ClkDay, ClkMonth, ClkYear, ClkDOW�� �����Ѵ�.
*********************************************************************************************************
*/

#if CLK_DATE_EN
static  void  ClkUpdateDate (void)
{
    BOOLEAN newmonth;


    newmonth = TRUE;
    if (ClkDay >= ClkMonthTbl[ClkMonth].MonthDays) {  /* ���� ������ ���ΰ�?                           */
        if (ClkMonth == 2) {                          /* 2�� �ΰ�?                                     */
            if (ClkIsLeapYear(ClkYear) == TRUE) {     /* ��, �����ΰ�?                                 */
                if (ClkDay >= 29) {                   /* ��, 2���� ������ ���ΰ�?                      */
                    ClkDay = 1;                       /* ��, 3�� 1�Ϸ� ��Ʈ�Ѵ�                        */
                } else {
                    ClkDay++;
                    newmonth = FALSE;
                }
            } else {
                ClkDay = 1;
            }
        } else {
            ClkDay = 1;
        }
    } else {
        ClkDay++;
        newmonth = FALSE;
    }
    if (newmonth == TRUE) {                      /* ���� ���ŵƴ°�?                                   */
        if (ClkMonth >= 12) {                    /* ��, 12���ΰ�?                                      */
            ClkMonth = 1;                        /* ��, 1���� ��Ʈ�Ѵ� ...                             */
            ClkYear++;                           /*     ... ���ظ� �´´�!                             */
        } else {
            ClkMonth++;                          /* �ƴϿ�,  ���� �����Ѵ�                             */
        }
    }
    ClkUpdateDOW();                              /* ������ ����Ѵ�(Sunday ...)                        */
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                               ���� ���
*
* ����       : �� �Լ��� ������ ��, ��, ������ ������� ����(�Ͽ���==0)�� ����Ѵ�
*
* ��������   : ����.
* ���ϰ�     : ����.
* ��Ʈ       : - �� �Լ��� ClkDOW�� �����Ѵ�
*              - �� �Լ��� ClkUpdateDate()�� ���ؼ� ȣ��ȴ�.
*********************************************************************************************************
*/
#if CLK_DATE_EN
static  void  ClkUpdateDOW (void)
{
    INT16U dow;


    dow = ClkDay + ClkMonthTbl[ClkMonth].MonthVal;
    if (ClkMonth < 3) {
        if (ClkIsLeapYear(ClkYear)) {
            dow--;
        }
    }
    dow    += ClkYear + (ClkYear / 4);
    dow    += (ClkYear / 400) - (ClkYear / 100);
    dow    %= 7;
    ClkDOW  = dow;
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                               �ð� ����
*
* ����       : �� �Լ��� �ð��� �����ϱ� ���ؼ� ȣ��ȴ�(��, ��, ��)
* ��������   : ����.
* ���ϰ�     : TRUE     1���� ��� ����.
*              FALSE    �׷��� ���� ���.
* ��Ʈ       : �� �Լ��� ClkSec, ClkMin, ClkHr�� �����Ѵ�.
*********************************************************************************************************
*/

static  BOOLEAN  ClkUpdateTime (void)
{
    BOOLEAN newday;


    newday = FALSE;                         /* ���� �Ϸ簡 ������ �ʾҴٰ� �����Ѵ�                    */
    if (ClkSec >= 59) {                     /* 1���� ����ߴ��� Ȯ���Ѵ�                               */
        ClkSec = 0;                         /* ��, �ʸ� Ŭ���� �Ѵ�                                    */
        if (ClkMin >= 59) {                 /*    1�ð��� ����ߴ��� Ȯ���Ѵ�                          */
            ClkMin = 0;                     /*    ��, ���� Ŭ���� �Ѵ�                                 */
            if (ClkHr >= 23) {              /*        �Ϸ簡 ����ߴ��� Ȯ���Ѵ�                       */
                ClkHr = 0;                  /*        ��, �ð��� Ŭ���� �Ѵ�...                        */
                newday    = TRUE;           /*        ... �Ϸ簡 �������� �˸��� �����׸� ��Ʈ�Ѵ�     */
            } else {
                ClkHr++;                    /*        �ƴϿ�, �ð��� �����Ѵ�                          */
            }
        } else {
            ClkMin++;                       /*    �ƴϿ�, ���� �����Ѵ�                                */
        }
    } else {
        ClkSec++;                           /* �ƴϿ�, �ʸ� �����Ѵ�                                   */
    }
    return (newday);
}
