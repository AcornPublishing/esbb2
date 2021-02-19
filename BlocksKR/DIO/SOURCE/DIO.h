/*
*********************************************************************************************************
*                                   Embedded Systems Building Blocks
*                                Complete and Ready-to-Use Modules in C
*
*                                         Discrete I/O Module
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                          All Rights Reserved
*
* Filename   : DIO.H
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               ���� ���
*********************************************************************************************************
*/

#ifndef  CFG_H

#define  DIO_TASK_PRIO             40
#define  DIO_TASK_DLY_TICKS         1
#define  DIO_TASK_STK_SIZE        512

#define  DIO_MAX_DI                 8       /* ������ �Է� ä���� �ִ� �� (1..255)                     */
#define  DIO_MAX_DO                 8       /* ������ ��� ä���� �ִ� �� (1..255)                     */

#define  DI_EDGE_EN                 1       /* ���� ���� �ڵ� ����(1�� ��)                             */

#define  DO_BLINK_MODE_EN           1       /* ���� ��� �ڵ� ����(1�� ��)                             */

#endif

#ifdef   DIO_GLOBALS
#define  DIO_EXT
#else
#define  DIO_EXT  extern
#endif

/*
*********************************************************************************************************
*                                            ������ �Է� ���
*********************************************************************************************************
*/

                                            /* DI ��� ���� ��                                         */
#define  DI_MODE_LOW                0       /* �Է��� LOW�� �Ѵ�                                       */
#define  DI_MODE_HIGH               1       /* �Է��� HIGH�� �Ѵ�                                      */
#define  DI_MODE_DIRECT             2       /* ���� �������� �Է� ��                                   */
#define  DI_MODE_INV                3       /* ���� �������� �Է� ���� ����                            */
#define  DI_MODE_EDGE_LOW_GOING     4       /* �ϰ� ���� ���� ���                                     */
#define  DI_MODE_EDGE_HIGH_GOING    5       /* ��� ���� ���� ���                                     */
#define  DI_MODE_EDGE_BOTH          6       /* �ϰ�, ��� ���� ���� ���                               */
#define  DI_MODE_TOGGLE_LOW_GOING   7       /* �ϰ� ���� ���� ���                                     */
#define  DI_MODE_TOGGLE_HIGH_GOING  8       /* ��� ���� ���� ���                                     */

                                            /* DI ���� Ʈ���Ÿ� ��� ���� ��                           */
#define  DI_EDGE_LOW_GOING          0       /* �ϰ� ����                                               */
#define  DI_EDGE_HIGH_GOING         1       /* ��� ����                                               */
#define  DI_EDGE_BOTH               2       /* �ϰ�, ��� ����                                         */

/*$PAGE*/
/*
*********************************************************************************************************
*                                            ������ ��� ���
*********************************************************************************************************
*/

                                            /* DO ��� ���� ��                                         */
#define  DO_MODE_LOW                0       /* ����� LOW�� �Ѵ�                                       */
#define  DO_MODE_HIGH               1       /* ����� HIGH�� �Ѵ�                                      */
#define  DO_MODE_DIRECT             2       /* ����ڰ� �����ϴ� ���� ���� �޶�����                    */
#define  DO_MODE_BLINK_SYNC         3       /* ���� ���� ���                                          */
#define  DO_MODE_BLINK_ASYNC        4       /* �񵿱� ���� ���                                        */

                                            /* DO ���� ��� ���� ��                                    */
#define  DO_BLINK_EN                0       /* �׻� ����                                               */
#define  DO_BLINK_EN_NORMAL         1       /* ����ڰ� �����ϴ� ��� �����Ѵ�                         */
#define  DO_BLINK_EN_INV            2       /* ����ڰ� �����ϴ� �ݴ�� �����Ѵ�                       */

/*
*********************************************************************************************************
*                                              ������ Ÿ��
*********************************************************************************************************
*/

typedef struct dio_di {                /* ������ �Է� ä�� ����ü                                      */
    BOOLEAN   DIIn;                    /* �Է� ������ ���� ����                                        */
    INT16U    DIVal;                   /* ������ �Է� ä���� ����(�Ǵ� ������ ���º�ȭ Ƚ��)           */
    BOOLEAN   DIPrev;                  /* ���� ������ ���Ǵ� ������ DIIn ����                        */
    BOOLEAN   DIBypassEn;              /* �����н� Ȱ��ȭ ����ġ(TRUE�� �� �����н��Ѵ�)               */
    INT8U     DIModeSel;               /* ������ �Է� ä�� ��� ����                                   */
#if DI_EDGE_EN
    void    (*DITrigFnct)(void *);     /* ������ ������ �� ����� ����� ���� �Լ�                     */
    void     *DITrigFnctArg;           /* ���� �Լ��� ���޵Ǵ� ������ ������                           */
#endif
} DIO_DI;


typedef struct dio_do {                /* ������ ��� ä�� ����ü                                      */
    BOOLEAN   DOOut;                   /* ��� ä���� ���� ����                                        */
    BOOLEAN   DOCtrl;                  /* ������ ��� ���� �䱸                                        */
    BOOLEAN   DOBypass;                /* ������ ��� ���� �����н� ����                               */
    BOOLEAN   DOBypassEn;              /* �����н� Ȱ��ȭ ����ġ(TRUE�� �� �����н� �Ѵ�)              */
    INT8U     DOModeSel;               /* ������ ��� ä�� ��� ����                                   */
    INT8U     DOBlinkEnSel;            /* ���� Ȱ��ȭ ��� ����                                        */
    BOOLEAN   DOInv;                   /* ������ ��� ���� (TRUE�� �� �����Ѵ�)                        */
#if DO_BLINK_MODE_EN
    INT8U     DOA;                     /* ���� ��� ON Ÿ��                                            */
    INT8U     DOB;                     /* �񵿱� ���� ��� �ֱ�                                        */
    INT8U     DOBCtr;                  /* �񵿱� ���� ��� �ֱ� ī����                                 */
#endif
} DIO_DO;
/*$PAGE*/
/*
*********************************************************************************************************
*                                                ��������
*********************************************************************************************************
*/

DIO_EXT  DIO_DI      DITbl[DIO_MAX_DI];
DIO_EXT  DIO_DO      DOTbl[DIO_MAX_DO];

#if      DO_BLINK_MODE_EN
DIO_EXT  INT8U       DOSyncCtr;
DIO_EXT  INT8U       DOSyncCtrMax;
#endif

/*
*********************************************************************************************************
*                                                �Լ�����
*********************************************************************************************************
*/

void     DIOInit(void);

void     DICfgMode(INT8U n, INT8U mode);
INT16U   DIGet(INT8U n);
void     DISetBypassEn(INT8U n, BOOLEAN state);
void     DISetBypass(INT8U n, INT16U val);

#if      DI_EDGE_EN
void     DIClr(INT8U n);
void     DICfgEdgeDetectFnct(INT8U n, void (*fnct)(void *), void *arg);
#endif


void     DOCfgMode(INT8U n, INT8U mode, BOOLEAN inv);
BOOLEAN  DOGet(INT8U n);
void     DOSet(INT8U n, BOOLEAN state);
void     DOSetBypass(INT8U n, BOOLEAN state);
void     DOSetBypassEn(INT8U n, BOOLEAN state);

#if      DO_BLINK_MODE_EN
void     DOCfgBlink(INT8U n, INT8U mode, INT8U a, INT8U b);
void     DOSetSyncCtrMax(INT8U val);
#endif

/*
*********************************************************************************************************
*                                        �ϵ���� ���õ� �Լ� ����
*
*********************************************************************************************************
*/

void     DIOInitIO(void);
void     DIRd(void);
void     DOWr(void);
