/*
*********************************************************************************************************
*                                          Analog I/O Module
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                          All Rights Reserved
*
* Filename   : AIO.H
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*/

#ifdef   AIO_GLOBALS
#define  AIO_EXT
#else
#define  AIO_EXT extern
#endif

/*
*********************************************************************************************************
*                                               ���� ���
*********************************************************************************************************
*/

#ifndef  CFG_H

#define  AIO_TASK_PRIO             40
#define  AIO_TASK_DLY             100       /* �и� �� ������ �½�ũ ���� �ֱ�                         */
#define  AIO_TASK_STK_SIZE        512

#define  AIO_MAX_AI                 8       /* �Ƴ��α� �Է� ä���� �ִ� ��(1..250)                    */
#define  AIO_MAX_AO                 8       /* �Ƴ��α� ��� ä���� ��� ��(1..250)                    */

#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                               ������ Ÿ��
*********************************************************************************************************
*/

typedef struct aio {                        /* �Ƴ��α� I/O ä�� ������ ����ü                         */
    BOOLEAN   AIOBypassEn;                  /* �����н� Ȱ��ȭ ����ġ(TRUE�� �� �����н�)              */
    INT16S    AIORaw;                       /* ADC �Ǵ� DAC�� ��                                       */
    FP32      AIOEU;                        /* AI ä���� ���д��� ��                                   */
    FP32      AIOGain;                      /* ��ü �̵�   (AIOCalGain   * AIOConvGain)                */
    FP32      AIOOffset;                    /* ��ü ������ (AIOCalOffset + AIOConvOffset)              */
    INT16S    AIOLim;                       /* �Ƴ��α� ��� ä���� �ִ� ��                            */
    INT8U     AIOPassCnts;                  /* �н� ī��Ʈ                                             */
    INT8U     AIOPassCtr;                   /* �н� ī���� (PassCnts�κ��� �ε�ȴ�)                   */
    FP32      AIOCalGain;                   /* ���� �̵�                                               */
    FP32      AIOCalOffset;                 /* ���� ������                                             */
    FP32      AIOConvGain;                  /* ��ȯ �̵�                                               */
    FP32      AIOConvOffset;                /* ��ȯ ������                                             */
    FP32      AIOScaleIn;                   /* �����Լ��� �Է�                                         */
    FP32      AIOScaleOut;                  /* �����Լ��� ���                                         */
    void    (*AIOScaleFnct)(struct aio *paio);   /* �����Լ� ������                                    */
    void     *AIOScaleFnctArg;                   /* �����Լ� AIOScaleFnct�� ��������                   */
} AIO;

/*
*********************************************************************************************************
*                                               ���� ����
*********************************************************************************************************
*/

AIO_EXT   AIO      AITbl[AIO_MAX_AI];
AIO_EXT   AIO      AOTbl[AIO_MAX_AO];

/*$PAGE*/
/*
*********************************************************************************************************
*                                               �Լ� ����
*********************************************************************************************************
*/

void     AIOInit(void);

INT8U    AICfgCal(INT8U n, FP32 gain, FP32 offset);
INT8U    AICfgConv(INT8U n, FP32 gain, FP32 offset, INT8U pass);
INT8U    AICfgScaling(INT8U n, void (*fnct)(AIO *paio), void *arg);
INT8U    AISetBypass(INT8U n, FP32 val);
INT8U    AISetBypassEn(INT8U n, BOOLEAN state);
INT8U    AIGet(INT8U n, FP32 *pval);

INT8U    AOCfgCal(INT8U n, FP32 gain, FP32 offset);
INT8U    AOCfgConv(INT8U n, FP32 gain, FP32 offset, INT16S lim, INT8U pass);
INT8U    AOCfgScaling(INT8U n, void (*fnct)(AIO *paio), void *arg);
INT8U    AOSet(INT8U n, FP32 val);
INT8U    AOSetBypass(INT8U n, FP32 val);
INT8U    AOSetBypassEn(INT8U n, BOOLEAN state);

void     AIOInitIO(void);                        /* �ϵ���� �������� �Լ�                             */
INT16S   AIRd(INT8U ch);
void     AOWr(INT8U ch, INT16S cnts);
