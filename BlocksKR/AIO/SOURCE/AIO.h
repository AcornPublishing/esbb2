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
*                                               설정 상수
*********************************************************************************************************
*/

#ifndef  CFG_H

#define  AIO_TASK_PRIO             40
#define  AIO_TASK_DLY             100       /* 밀리 초 단위의 태스크 실행 주기                         */
#define  AIO_TASK_STK_SIZE        512

#define  AIO_MAX_AI                 8       /* 아날로그 입력 채널의 최대 수(1..250)                    */
#define  AIO_MAX_AO                 8       /* 아날로그 출력 채널의 쇠대 수(1..250)                    */

#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                               데이터 타잎
*********************************************************************************************************
*/

typedef struct aio {                        /* 아날로그 I/O 채널 데이터 구조체                         */
    BOOLEAN   AIOBypassEn;                  /* 바이패스 활성화 스위치(TRUE일 때 바이패스)              */
    INT16S    AIORaw;                       /* ADC 또는 DAC의 값                                       */
    FP32      AIOEU;                        /* AI 채널의 공학단위 값                                   */
    FP32      AIOGain;                      /* 전체 이득   (AIOCalGain   * AIOConvGain)                */
    FP32      AIOOffset;                    /* 전체 오프셋 (AIOCalOffset + AIOConvOffset)              */
    INT16S    AIOLim;                       /* 아날로그 출력 채널의 최대 값                            */
    INT8U     AIOPassCnts;                  /* 패스 카운트                                             */
    INT8U     AIOPassCtr;                   /* 패스 카운터 (PassCnts로부터 로드된다)                   */
    FP32      AIOCalGain;                   /* 교정 이득                                               */
    FP32      AIOCalOffset;                 /* 교정 오프셋                                             */
    FP32      AIOConvGain;                  /* 변환 이득                                               */
    FP32      AIOConvOffset;                /* 변환 오프셋                                             */
    FP32      AIOScaleIn;                   /* 배율함수의 입력                                         */
    FP32      AIOScaleOut;                  /* 배율함수의 출력                                         */
    void    (*AIOScaleFnct)(struct aio *paio);   /* 배율함수 포인터                                    */
    void     *AIOScaleFnctArg;                   /* 배율함수 AIOScaleFnct의 전달인자                   */
} AIO;

/*
*********************************************************************************************************
*                                               전역 변수
*********************************************************************************************************
*/

AIO_EXT   AIO      AITbl[AIO_MAX_AI];
AIO_EXT   AIO      AOTbl[AIO_MAX_AO];

/*$PAGE*/
/*
*********************************************************************************************************
*                                               함수 원형
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

void     AIOInitIO(void);                        /* 하드웨어 의존적인 함수                             */
INT16S   AIRd(INT8U ch);
void     AOWr(INT8U ch, INT16S cnts);
