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
*                                               설정 상수
*********************************************************************************************************
*/

#ifndef  CFG_H

#define  DIO_TASK_PRIO             40
#define  DIO_TASK_DLY_TICKS         1
#define  DIO_TASK_STK_SIZE        512

#define  DIO_MAX_DI                 8       /* 디지털 입력 채널의 최대 수 (1..255)                     */
#define  DIO_MAX_DO                 8       /* 디지털 출력 채널의 최대 수 (1..255)                     */

#define  DI_EDGE_EN                 1       /* 에지 감지 코드 생성(1일 때)                             */

#define  DO_BLINK_MODE_EN           1       /* 점멸 모드 코드 생성(1일 때)                             */

#endif

#ifdef   DIO_GLOBALS
#define  DIO_EXT
#else
#define  DIO_EXT  extern
#endif

/*
*********************************************************************************************************
*                                            디지털 입력 상수
*********************************************************************************************************
*/

                                            /* DI 모드 선택 값                                         */
#define  DI_MODE_LOW                0       /* 입력을 LOW로 한다                                       */
#define  DI_MODE_HIGH               1       /* 입력을 HIGH로 한다                                      */
#define  DI_MODE_DIRECT             2       /* 실제 물리적인 입력 값                                   */
#define  DI_MODE_INV                3       /* 실제 물리적인 입력 값의 반전                            */
#define  DI_MODE_EDGE_LOW_GOING     4       /* 하강 에지 감지 모드                                     */
#define  DI_MODE_EDGE_HIGH_GOING    5       /* 상승 에지 감지 모드                                     */
#define  DI_MODE_EDGE_BOTH          6       /* 하강, 상승 에지 감지 모드                               */
#define  DI_MODE_TOGGLE_LOW_GOING   7       /* 하강 에지 감지 모드                                     */
#define  DI_MODE_TOGGLE_HIGH_GOING  8       /* 상승 에지 감지 모드                                     */

                                            /* DI 에지 트리거링 모드 선택 값                           */
#define  DI_EDGE_LOW_GOING          0       /* 하강 에지                                               */
#define  DI_EDGE_HIGH_GOING         1       /* 상승 에지                                               */
#define  DI_EDGE_BOTH               2       /* 하강, 상승 에지                                         */

/*$PAGE*/
/*
*********************************************************************************************************
*                                            디지털 출력 상수
*********************************************************************************************************
*/

                                            /* DO 모드 선택 값                                         */
#define  DO_MODE_LOW                0       /* 출력을 LOW로 한다                                       */
#define  DO_MODE_HIGH               1       /* 출력을 HIGH로 한다                                      */
#define  DO_MODE_DIRECT             2       /* 사용자가 설정하는 값에 따라 달라진다                    */
#define  DO_MODE_BLINK_SYNC         3       /* 동기 점멸 모드                                          */
#define  DO_MODE_BLINK_ASYNC        4       /* 비동기 점멸 모드                                        */

                                            /* DO 점멸 모드 선택 값                                    */
#define  DO_BLINK_EN                0       /* 항상 점멸                                               */
#define  DO_BLINK_EN_NORMAL         1       /* 사용자가 설정하는 대로 점멸한다                         */
#define  DO_BLINK_EN_INV            2       /* 사용자가 설정하는 반대로 점멸한다                       */

/*
*********************************************************************************************************
*                                              데이터 타입
*********************************************************************************************************
*/

typedef struct dio_di {                /* 디지털 입력 채널 구조체                                      */
    BOOLEAN   DIIn;                    /* 입력 센서의 현재 상태                                        */
    INT16U    DIVal;                   /* 디지털 입력 채널의 상태(또는 감지된 상태변화 횟수)           */
    BOOLEAN   DIPrev;                  /* 에지 감지에 사용되는 이전의 DIIn 상태                        */
    BOOLEAN   DIBypassEn;              /* 바이패스 활성화 스위치(TRUE일 때 바이패스한다)               */
    INT8U     DIModeSel;               /* 디지털 입력 채널 모드 선택                                   */
#if DI_EDGE_EN
    void    (*DITrigFnct)(void *);     /* 에지가 감지될 때 수행될 사용자 정의 함수                     */
    void     *DITrigFnctArg;           /* 위의 함수로 전달되는 인자의 포인터                           */
#endif
} DIO_DI;


typedef struct dio_do {                /* 디지털 출력 채널 구조체                                      */
    BOOLEAN   DOOut;                   /* 출력 채널의 현재 상태                                        */
    BOOLEAN   DOCtrl;                  /* 디지털 출력 제어 요구                                        */
    BOOLEAN   DOBypass;                /* 디지털 출력 제어 바이패스 상태                               */
    BOOLEAN   DOBypassEn;              /* 바이패스 활성화 스위치(TRUE일 때 바이패스 한다)              */
    INT8U     DOModeSel;               /* 디지털 출력 채널 모드 선택                                   */
    INT8U     DOBlinkEnSel;            /* 점멸 활성화 모드 선택                                        */
    BOOLEAN   DOInv;                   /* 디지털 출력 반전 (TRUE일 때 반전한다)                        */
#if DO_BLINK_MODE_EN
    INT8U     DOA;                     /* 점멸 모드 ON 타임                                            */
    INT8U     DOB;                     /* 비동기 점멸 모드 주기                                        */
    INT8U     DOBCtr;                  /* 비동기 점멸 모드 주기 카운터                                 */
#endif
} DIO_DO;
/*$PAGE*/
/*
*********************************************************************************************************
*                                                전역변수
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
*                                                함수원형
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
*                                        하드웨어 관련된 함수 원형
*
*********************************************************************************************************
*/

void     DIOInitIO(void);
void     DIRd(void);
void     DOWr(void);
