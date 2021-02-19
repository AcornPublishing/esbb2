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
* Filename   : DIO.C
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              인크루드 파일
*********************************************************************************************************
*/

#define  DIO_GLOBALS
#include "includes.h"

/*
*********************************************************************************************************
*                                                지역변수
*********************************************************************************************************
*/

static  OS_STK      DIOTaskStk[DIO_TASK_STK_SIZE];

/*
*********************************************************************************************************
*                                             지역함수 원형
*********************************************************************************************************
*/


static  void        DIIsTrig(DIO_DI *pdi);

static  void        DIOTask(void *data);

static  void        DIUpdate(void);

static  BOOLEAN     DOIsBlinkEn(DIO_DO *pdo);
static  void        DOUpdate(void);

/*$PAGE*/
/*
*********************************************************************************************************
*                                디지털 입력의 에지(EDGE) 감지 기능 설정
*
* 설명      : 이 함수는 디지털 입력 채널의 에지(edge) 감지 기능을 설정하기 위해 사용한다.
*
* 전달인자  : n     설정하고자 하는 디지털 입력 채널(0..DIO_MAX_DI-1).
*             fnct  원하는 에지가 감지될 때 호출하고자 하는 함수의 포인터
*
*             arg   함수로 전달될 인자의 포인터
* 리턴값    : 없음.
*********************************************************************************************************
*/

#if  DI_EDGE_EN
void  DICfgEdgeDetectFnct (INT8U n, void (*fnct)(void *), void *arg)
{
    DIO_DI  *pdi;


    if (n < DIO_MAX_DI) {
        pdi                = &DITbl[n];
        OS_ENTER_CRITICAL();
        pdi->DITrigFnct    = fnct;
        pdi->DITrigFnctArg = arg;
        OS_EXIT_CRITICAL();
    }
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                         디지털 입력의 모드 설정
*
* 설명      : 이 함수는 디지털 입력 채널의 모드를 설정할 때 호출된다.
* 전달인자  : n     설정하고자 하는 디지털 입력 채널(0..DIO_MAX_DI-1).
*             mode  설정하고자 하는 모드. 모드는 다음과 같다:
*                   DI_MODE_LOW               입력을 LOW로 한다
*                   DI_MODE_HIGH              입력을 HIGH로 한다
*                   DI_MODE_DIRECT            입력의 상태가 실제 물리적인 센서에 따라 달라진다(디폴트)
*                   DI_MODE_INV               입력의 상태가 물리직인 센서의 역전된 상태이다
*                   DI_MODE_EDGE_LOW_GOING    하강 에지 감지
*                   DI_MODE_EDGE_HIGH_GOING   상승 에지 감지
*                   DI_MODE_EDGE_BOTH         하강 상승 에지 감지
*                   DI_MODE_TOGGLE_LOW_GOING  토글 모드의 하강 에지 감지
*                   DI_MODE_TOGGLE_HIGH_GOING 토글 모드의 상승 에지 감지
* 리턴값    : 없음.
* 노트      : 에지 감지는 DI_EDGE_EN 이 1로 설정될 때만 가능하다.
*********************************************************************************************************
*/

void  DICfgMode (INT8U n, INT8U mode)
{
    if (n < DIO_MAX_DI) {
        OS_ENTER_CRITICAL();
        DITbl[n].DIModeSel = mode;
        OS_EXIT_CRITICAL();
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                        디지털 입력 채널 클리어
*
* 설명      : 이 함수는 디지털 입력 채널이 에지 감지용으로 설정된 경우 감지된 횟수를 클리어 한다.
*
* 전달인자  : n     클리어 하고자 하는 디지털 입력 채널(0..DIO_MAX_DI-1).
* 리턴값    : 없음
*********************************************************************************************************
*/

#if  DI_EDGE_EN
void  DIClr (INT8U n)
{
    DIO_DI  *pdi;


    if (n < DIO_MAX_DI) {
        pdi = &DITbl[n];
        OS_ENTER_CRITICAL();
        if (pdi->DIModeSel == DI_MODE_EDGE_LOW_GOING  ||        /* 에지 감지 모드인지 확인             */
            pdi->DIModeSel == DI_MODE_EDGE_HIGH_GOING ||
            pdi->DIModeSel == DI_MODE_EDGE_BOTH) {
            pdi->DIVal = 0;                                     /* 감지된 횟수 클리어                  */
        }
        OS_EXIT_CRITICAL();
    }
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                       디지털 입력 채널의 상태 얻기
*
* 설명      : 이 함수는 디지털 입력 채널의 현재 상태를 얻는데 사용한다.
*             채널이 에지 감지용으로 설정돼 있다면 감지된 횟수를 리턴한다.
* 전달인자  : n     디지털 입력 채널(0..DIO_MAX_DI-1).
* 리턴값    : 0     입력이 LOW인 경우 또는 에지가 감지되지 않은 경우
*             1     입력이 HIGH인 경우
*             > 0   에지가 감지된 경우
*********************************************************************************************************
*/

INT16U  DIGet (INT8U n)
{
    INT16U  val;


    if (n < DIO_MAX_DI) {
        OS_ENTER_CRITICAL();
        val = DITbl[n].DIVal;                              /* DI 채널의 상태를 얻는다                  */
        OS_EXIT_CRITICAL();
        return (val);
    } else {
        return (0);                                        /* 무효한 채널인 경우 무조건 0을 리턴한다   */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                                에지 감지
*
* 설명      : 이 함수는 선택된 채널에 대해 에지 감지를 위해서 호출된다(하강, 상승 또는 양자 모두)
*
* 전달인자  : pdi    디지털 입력 채널에 대한 구조체의 포인터.
* 리턴값    : 없음
*********************************************************************************************************
*/

#if DI_EDGE_EN
static  void  DIIsTrig (DIO_DI *pdi)
{
    BOOLEAN  trig;


    trig = FALSE;
    switch (pdi->DIModeSel) {
        case DI_MODE_EDGE_LOW_GOING:                       /* 하강 에지                                */
             if (pdi->DIPrev == 1 && pdi->DIIn == 0) {
                 trig = TRUE;
             }
             break;

        case DI_MODE_EDGE_HIGH_GOING:                      /* 상승 에지                                */
             if (pdi->DIPrev == 0 && pdi->DIIn == 1) {
                 trig = TRUE;
             }
             break;

        case DI_MODE_EDGE_BOTH:                            /* 양쪽 모두                                */
             if ((pdi->DIPrev == 1 && pdi->DIIn == 0) ||
                 (pdi->DIPrev == 0 && pdi->DIIn == 1)) {
                 trig = TRUE;
             }
             break;
    }
    if (trig == TRUE) {                                    /* 에지가 감지됐는가?                       */
        if (pdi->DITrigFnct != NULL) {                     /* 예, 사용자 정의 함수가 설정됐는가?       */
            (*pdi->DITrigFnct)(pdi->DITrigFnctArg);        /* 예, 함수를 실행한다                      */
        }
        if (pdi->DIVal < 255) {                            /* 에지 감지 횟수를 증가한다                */
            pdi->DIVal++;
        }
    }
    pdi->DIPrev = pdi->DIIn;                               /* 이전의 입력 상태를 저장한다              */
}
#endif

/*$PAGE*/
/*$PAGE*/
/*
*********************************************************************************************************
*                                         디지털 입력 채널 갱신
*
* 설명      : 이 함수는 모든 입력 채널의 상태를 갱신한다.
* 전달인자  : 없음.
* 리턴값    : 없음.
*********************************************************************************************************
*/

static  void  DIUpdate (void)
{
    INT8U    i;
    DIO_DI  *pdi;


    pdi = &DITbl[0];
    for (i = 0; i < DIO_MAX_DI; i++) {
        if (pdi->DIBypassEn == FALSE) {          /* 디지털 입력 채널이 바이패스 됐는가?                */
            switch (pdi->DIModeSel) {            /* 아니오, 채널을 처리한다                            */
                case DI_MODE_LOW:                /* 입력을 low로 한다                                  */
                     pdi->DIVal = 0;
                     break;

                case DI_MODE_HIGH:               /* 입력을 high로 한다                                 */
                     pdi->DIVal = 1;
                     break;

                case DI_MODE_DIRECT:                  /* 실제 물리적인 신호에 의존한다                 */
                     pdi->DIVal = (INT8U)pdi->DIIn;   /* 실제 입력값을 얻는다                          */
                     break;

                case DI_MODE_INV:                /* 실제 입력의 반전된 값을 얻는다                     */
                     pdi->DIVal = (INT8U)(pdi->DIIn ? 0 : 1);
                     break;

#if DI_EDGE_EN
                case DI_MODE_EDGE_LOW_GOING:
                case DI_MODE_EDGE_HIGH_GOING:
                case DI_MODE_EDGE_BOTH:
                     DIIsTrig(pdi);              /* 에지 감지 모드를 처리한다                          */
                     break;
#endif
/*$PAGE*/
                case DI_MODE_TOGGLE_LOW_GOING:
                     if (pdi->DIPrev == 1 && pdi->DIIn == 0) {
                         pdi->DIVal = pdi->DIVal ? 0 : 1;
                     }
                     pdi->DIPrev = pdi->DIIn;
                     break;

                case DI_MODE_TOGGLE_HIGH_GOING:
                     if (pdi->DIPrev == 0 && pdi->DIIn == 1) {
                         pdi->DIVal = pdi->DIVal ? 0 : 1;
                     }
                     pdi->DIPrev = pdi->DIIn;
                     break;
            }
        }
        pdi++;                                   /* 다음 DIO_DO요소를 지정한다                         */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         디지털 I/O 관리자 초기화
*
* 설명      : 이 함수는 디지털 I/O 관리자 모듈을 초기화한다.
* 전달인자  : 없음
* 리턴값    : 없음.
*********************************************************************************************************
*/

void  DIOInit (void)
{
    INT8U    err;
    INT8U    i;
    DIO_DI  *pdi;
    DIO_DO  *pdo;


    pdi = &DITbl[0];
    for (i = 0; i < DIO_MAX_DI; i++) {
        pdi->DIVal         = 0;
        pdi->DIBypassEn    = FALSE;
        pdi->DIModeSel     = DI_MODE_DIRECT;     /* 직접입력으로 디폴트 모드를 설정한다                */
#if DI_EDGE_EN
        pdi->DITrigFnct    = (void *)0;          /* 사용자 정의 함수 포인터 초기화                     */
        pdi->DITrigFnctArg = (void *)0;
#endif
        pdi++;
    }
    pdo = &DOTbl[0];
    for (i = 0; i < DIO_MAX_DO; i++) {
        pdo->DOOut        = 0;
        pdo->DOBypassEn   = FALSE;
        pdo->DOModeSel    = DO_MODE_DIRECT;      /* 직접출력으로 디폴트 모드를 설정한다                */
        pdo->DOInv        = FALSE;
#if DO_BLINK_MODE_EN
        pdo->DOBlinkEnSel = DO_BLINK_EN_NORMAL;  /* 직접 사용자 요구에 의해 점멸이 활성화된다          */
        pdo->DOA          = 1;
        pdo->DOB          = 2;
        pdo->DOBCtr       = 2;
#endif
        pdo++;
    }
#if DO_BLINK_MODE_EN
    DOSyncCtrMax = 100;
#endif
    DIOInitIO();
    OSTaskCreate(DIOTask, (void *)0, &DIOTaskStk[DIO_TASK_STK_SIZE], DIO_TASK_PRIO);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         디지털 I/O 관리자 태스크
*
* 설명      : 이 태스크는 에 의해 생성되며 디지털 출력과 입력을 갱신하는 일을 한다.
*             DIOTask() DIO_TASK_DLY_TICKS 마다 실행된다.
*
* 전달인자  : 없음.
* 리턴값    : 없음.
*********************************************************************************************************
*/

static  void  DIOTask (void *data)
{
    data = data;                                      /* 컴파일러 경고를 피한다(uC/OS 요구사항)        */
    for (;;) {
        OSTimeDly(DIO_TASK_DLY_TICKS);                /* DIO 관리자 시간지연                           */
        DIRd();                                       /* 물리적인 입력신호를 읽고 DI채널로 매핑한다    */
        DIUpdate();                                   /* 모든 DI 채널을 갱신한다                       */
        DOUpdate();                                   /* 모든 DO 채널을 갱신한다                       */
        DOWr();                                       /* DO 채널을 물리적인 출력으로 매핑한다          */
    }
}
/*$PAGE*/

/*
*********************************************************************************************************
*                                        바이패스된 입력 상태 설정
*
* 설명      : 이 함수는 바이패스된 입력의 상태를 설정하는데 사용한다.
*             이 함수는 하드웨어 입력이 있는 것처럼 동작한다.
*             이 함수는 바이패스 스위치가 열려 있는 경우만 의미가 있다.
* 전달인자  : n     디지털 입력 채널(0..DIO_MAX_DI-1).
*             val   바이패스 된 입력의 상태:
*                   0     LOW로 설정함을 명시
*                   1     HIGH로 설정함을 명시
*                   > 0   에지 모드에서의 감지 횟수 명시
* 리턴값    : 없음.
*********************************************************************************************************
*/

void  DISetBypass (INT8U n, INT16U val)
{
    DIO_DI  *pdi;


    if (n < DIO_MAX_DI) {
        pdi = &DITbl[n];
        OS_ENTER_CRITICAL();
        if (pdi->DIBypassEn == TRUE) {           /* 입력이 바이패스 됐는지 확인                        */
            pdi->DIVal = val;                    /* 예, DI 채널의 새 값을 설정한다                     */
        }
        OS_EXIT_CRITICAL();
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        입력 바이패스 스위치 설정
*
* 설명      : 이 함수는 입력 바이패스 스위치의 상태를 설정한다.
*             스위치가 열려 있을 때(.DIBypassEn이 TRUE) 입력이 바이패스 된다
* 전달인자  : n     디지털 입력 채널(0..DIO_MAX_DI-1).
*             state 바이패스 스위치의 상태:
*                         FALSE 바이패스 비활성화  (바이패스 스위치가 닫힘)
*                         TRUE  바이패스 활성화    (바이패스 스위치가 열림)
* 리턴값    : 없음.
*********************************************************************************************************
*/

void  DISetBypassEn (INT8U n, BOOLEAN state)
{
    if (n < DIO_MAX_DI) {
        OS_ENTER_CRITICAL();
        DITbl[n].DIBypassEn = state;
        OS_EXIT_CRITICAL();
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         디지털 출력 점멸모드 설정
*
* 설명      : 이 함수는 디지털 출력 채널의 점멸모드를 설정하기 위해 사용한다.
* 전달인자  : n     디지털 출력 채널(0..DIO_MAX_DO-1).
*             mode  원하는 점멸모드:
*                     DO_BLINK_EN         항상 점멸모드가 활성화됨
*                     DO_BLINK_EN_NORMAL  점멸모드가 사용자의 요청에 따라 달라짐
*                     DO_BLINK_EN_INV     점멸모드가 사용자의 요청에 반대로 동작함
*               a     DIO 태스크의 실행빈도에 상대적인 ON 타임 (1..250)
*               b     DO_MODE_BLINK_ASYNC 모드에서의 주기 (1..250)
* 리턴값    : 없음.
*********************************************************************************************************
*/

#if  DO_BLINK_MODE_EN
void  DOCfgBlink (INT8U n, INT8U mode, INT8U a, INT8U b)
{
    DIO_DO  *pdo;


    if (n < DIO_MAX_DO) {
        pdo               = &DOTbl[n];
        OS_ENTER_CRITICAL();
        pdo->DOBlinkEnSel = mode;
        pdo->DOA          = a;
        pdo->DOB          = b;
        pdo->DOBCtr       = 0;
        OS_EXIT_CRITICAL();
    }
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                         디지털 출력 모드 설정
*
* 설명      : 이 함수는 디지털 출력 채널의 모드를 설정하는데 사용한다.
* 전달인자  : n     설정하고자 하는 디지털 출력 채널(0..DIO_MAX_DO-1).
*             mode  원하는 모드:
*                   DO_MODE_LOW              출력을 LOW로 한다
*                   DO_MODE_HIGH             출력을 HIGH로 한다
*                   DO_MODE_DIRECT           출력이 DOBypass에 의존한다
*                   DO_MODE_BLINK_SYNC       DOSyncCtr에 동기되어 점멸한다
*                   DO_MODE_BLINK_ASYNC      DOA 와 DOB에 따라 점멸한다
*             inv   출력을 반전할 것이지를 알려준다:
*                   TRUE   출력 반전
*                   FALSE  정상 출력
* 리턴값    : 없음.
*********************************************************************************************************
*/

void  DOCfgMode (INT8U n, INT8U mode, BOOLEAN inv)
{
    DIO_DO  *pdo;
    
    
    if (n < DIO_MAX_DO) {
        pdo            = &DOTbl[n];
        OS_ENTER_CRITICAL();
        pdo->DOModeSel = mode;
        pdo->DOInv     = inv;
        OS_EXIT_CRITICAL();
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                 GET THE STATE OF THE DISCRETE OUTPUT
*
* 설명      : 이 함수는 디지털 출력의 상태를 얻기 위해 사용한다.
* 전달인자  : n     디지털 출력 채널(0..DIO_MAX_DO-1).
* 리턴값    : TRUE  출력이 HIGH인 경우.
*             FALSE 출력이 LOW인 경우.
*********************************************************************************************************
*/

BOOLEAN  DOGet (INT8U n)
{
    BOOLEAN  out;


    if (n < DIO_MAX_DO) {
        OS_ENTER_CRITICAL();
        out = DOTbl[n].DOOut;
        OS_EXIT_CRITICAL();
        return (out);
    } else {
        return (FALSE);
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             점멸 모드 확인
*
* 설명      : 점멸모드가 활성화돼 있는지 확인한다.
* 전달인자  : pdo    디지털 출력의 데이터 구조체 포인터.
* 리턴값    : TRUE   점멸 활성화
*             FALSE  그렇지 않은 경우
*********************************************************************************************************
*/

#if DO_BLINK_MODE_EN
static  BOOLEAN  DOIsBlinkEn (DIO_DO *pdo)
{
    BOOLEAN  en;


    en = FALSE;
    switch (pdo->DOBlinkEnSel) {
        case DO_BLINK_EN:                   /* 상시 점멸 모드                                          */
             en = TRUE;
             break;

        case DO_BLINK_EN_NORMAL:            /* 사용자 요구 상태에 따라 활성화 또는 비활성화            */
             en = pdo->DOBypass;
             break;

        case DO_BLINK_EN_INV:               /* 사용자 요구상태의 반전에 따라 활성화 또는 비활성화      */
             en = pdo->DOBypass ? FALSE : TRUE;
             break;
    }
    return (en);
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                         디지털 출력 상태 설정
*
* 설명      : 이 함수는 디지털 출력의 상태를 설정하기 위해 사용한다.
* 전달인자  : n     디지털 출력 채널(0..DIO_MAX_DO-1).
*             state 원하는 출력 상태:
*                         FALSE 출력상태를 LOW로 한다
*                         TRUE  출력상태를 HIGH로 한다
* 리턴값    : 없음.
* 노트      : 'DIInv'가 TRUE로 설정돼 있다면 실제 출력은 반전된다.
*********************************************************************************************************
*/

void  DOSet (INT8U n, BOOLEAN state)
{
    if (n < DIO_MAX_DO) {
        OS_ENTER_CRITICAL();
        DOTbl[n].DOCtrl = state;
        OS_EXIT_CRITICAL();
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        바이패스 출력 상태 설정
*
* 설명      : 이 함수는 바이패스 출력의 상태를 설정하기 위해 사용한다.
*             이 함수는 응용프로그램을 바이패스 해서 출력을 직접 제어할 수 있게 해준다.
*             이 함수는 바이패스 스위치가 열려 있는 경우만 유효하다
* 전달인자  : n     디지털 출력 채널(0..DIO_MAX_DO-1).
*             state 원하는 출력 상태:
*                         FALSE 출력상태를 LOW로 한다
*                         TRUE  출력상태를 HIGH로 한다
* 리턴값    : 없음.
* 노트      : 1) 'DIInv'가 TRUE로 설정돼 있다면 실제 출력은 반전된다.
*             2) 점멸 모드에서는, 점멸을 시작 또는 정지 시킨다.
*********************************************************************************************************
*/

void  DOSetBypass (INT8U n, BOOLEAN state)
{
    DIO_DO  *pdo;
    
    
    if (n < DIO_MAX_DO) {
        pdo = &DOTbl[n];
        OS_ENTER_CRITICAL();
        if (pdo->DOBypassEn == TRUE) {
            pdo->DOBypass = state;
        }
        OS_EXIT_CRITICAL();
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           바이패스 출력 설정
*
* 설명      : 이 함수는 바이패스 스위치의 상태를 설정하기 위해 사용한다.
*             스위치가 열리면 출력은 바이패스 된다(DOBypassEn이 TRUE일 때).
* 전달인자  : n     디지털 출력 채널(0..DIO_MAX_DO-1).
*             state 바이패스 스위치의 상태:
*                       FALSE 바이패스 출력을 비활성화한다(스위치가 닫힘)
*                       TRUE  바이패스 출력을 활성화한다(스위치가 열림)
* 리턴값    : 없음.
*********************************************************************************************************
*/

void  DOSetBypassEn (INT8U n, BOOLEAN state)
{
    if (n < DIO_MAX_DO) {
        OS_ENTER_CRITICAL();
        DOTbl[n].DOBypassEn = state;
        OS_EXIT_CRITICAL();
    }
}

/*$PAGE*/

/*
*********************************************************************************************************
*                                         동기 카운터 최대값 설정
*
* 설명      : 이 함수는 동기 점멸 모드에서 사용되는 동기 카운터의 최대값을 설정하기 위해서 사용한다.
*
* 전달인자  : val   카운터의 최대 값 (1..255)
* 리턴값    : 없음.
*********************************************************************************************************
*/

#if  DO_BLINK_MODE_EN
void  DOSetSyncCtrMax (INT8U val)
{
    OS_ENTER_CRITICAL();
    DOSyncCtrMax = val;
    DOSyncCtr    = val;
    OS_EXIT_CRITICAL();
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                           디지털 출력 갱신
*
* 설명      : 이 함수는 모든 디지털 출력 채널을 처리하기 위해 호출된다.
* 전달인자  : 없음.
* 리턴값    : 없음.
*********************************************************************************************************
*/

static  void  DOUpdate (void)
{
    INT8U     i;
    BOOLEAN   out;
    DIO_DO   *pdo;


    pdo = &DOTbl[0];
    for (i = 0; i < DIO_MAX_DO; i++) {           /* 모든 출력을 처리한다                               */
        if (pdo->DOBypassEn == FALSE) {          /* DO 채널이 활성화돼 있는지 확인한다                 */
            pdo->DOBypass = pdo->DOCtrl;         /* 제어 상태를 얻는다                                 */
        }
        out = FALSE;                             /* 변경되지 않는 한 출력을 LOW로 가정한다             */
        switch (pdo->DOModeSel) {
            case DO_MODE_LOW:                    /* 실제 LOW가 출력될 것이다                           */
                 break;

            case DO_MODE_HIGH:                   /* HIGH를 출력한다                                    */
                 out = TRUE;
                 break;

            case DO_MODE_DIRECT:                 /* 사용자가 설정한 값으로 출력된다                    */
                 out = pdo->DOBypass;
                 break;
/*$PAGE*/
#if DO_BLINK_MODE_EN
            case DO_MODE_BLINK_SYNC:                       /* 동기 점멸 모드                           */
                 if (DOIsBlinkEn(pdo)) {                   /* 점멸이 활성화돼 있는지 확인...           */
                     if (pdo->DOA >= DOSyncCtr) {          /* ... 예, 임계치 이하이면 HIGH로 한다      */
                         out = TRUE;
                     }
                 }
                 break;

            case DO_MODE_BLINK_ASYNC:                      /* 비동기 점멸 모드                         */
                 if (DOIsBlinkEn(pdo)) {                   /* 점멸이 활성화돼 있는지 확인...           */
                     if (pdo->DOA >= pdo->DOBCtr) {        /* ... 예, 임계치 이하이면 HIGH로 한다      */
                         out = TRUE;
                     }
                 }
                 if (pdo->DOBCtr < pdo->DOB) {             /* 임계치 카운터 갱신                       */
                     pdo->DOBCtr++;
                 } else {
                     pdo->DOBCtr = 0;
                 }
                 break;
#endif
        }
        if (pdo->DOInv == TRUE) {                          /* 출력 반전 확인 ...                       */
            pdo->DOOut = out ? FALSE : TRUE;               /* ... 예, 출력을 반전한다                  */
        } else {
            pdo->DOOut = out;                              /* ... 아니오, 반전하지 않는다              */
        }
        pdo++;                                             /* 다음 DIO_DO 블럭으로 넘어간다            */
    }
#if DO_BLINK_MODE_EN
    if (DOSyncCtr < DOSyncCtrMax) {                        /* 동기 카운터를 갱신한다                   */
        DOSyncCtr++;
    } else {
        DOSyncCtr = 0;
    }
#endif
}
/*$PAGE*/
#ifndef CFG_C
/*
*********************************************************************************************************
*                                            물리적 I/O 초기화
*
* 설명      : 이 함수는 DIO드라이버에서 사용하는 물리적인 I/O를 초기화 하기 위해서 DIOInit()에서 호출된다
* 전달인자  : 없음.
* 리턴값    : 없음.
* 노트      : 물리적 I/O 는 다음과 같이 초기화된 82C55 칩이라고 가정한다.
*                   포트 A = 출력 (디지털 출력)
*                   포트 B = 입력 (디지털 입력)
*                   포트 C = 출력 (사용 안함)
*********************************************************************************************************
*/

void  DIOInitIO (void)
{
    outp(0x0303, 0x82);                          /* 포트 A = 출력, 포트 B = 입력, 포트 C = 출력        */
}

/*
*********************************************************************************************************
*                                            물리적 입력 읽기
*
* 설명      : 이 함수는 물리적 포트로 부터 입력 상태를 읽고 이 값을 대응되는 입력 데이터 구조체로
*             매핑하기 위해 호출된다.
* 전달인자  : 없음.
* 리턴값    : 없음.
*********************************************************************************************************
*/

void  DIRd (void)
{
    DIO_DI  *pdi;
    INT8U    i;
    INT8U    in;
    INT8U    msk;


    pdi = &DITbl[0];                                       /* 입력 데이터 구조체의 시작점을 지시한다   */
    msk = 0x01;                                            /* 비트 0을 얻어내기 위한 마스크 설정       */
    in  = inp(0x0301);                                     /* 물리적 포트를 읽는다(8비트)              */
    for (i = 0; i < 8; i++) {                              /* 8개의 DI채널에 모든 비트를 매핑한다      */
        pdi->DIIn   = (BOOLEAN)(in & msk) ? 1 : 0;
        msk       <<= 1;
        pdi++;
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                           물리적 출력 갱신
* 설명      : 이 함수는 모든 디지털 출력 채널의 값을 대응하는 물리적 출력으로 매핑하기 위해 호출된다
*
* 전달인자  : 없음.
* 리턴값    : 없음.
*********************************************************************************************************
*/

void  DOWr (void)
{
    DIO_DO  *pdo;
    INT8U    i;
    INT8U    out;
    INT8U    msk;


    pdo = &DOTbl[0];                        /* 출력 데이터 구조체의 시작점을 지시한다                  */
    msk = 0x01;                             /* 첫 번째 DO는 비트 0에 매핑된다                          */
    out = 0x00;                             /* 임시 출력 포트 이미지                                   */
    for (i = 0; i < 8; i++) {               /* 8개 DO의 값을 8비트 포트 이미지로 매핑한다              */
        if (pdo->DOOut == TRUE) {
            out |= msk;
        }
        msk <<= 1;
        pdo++;
    }
    outp(0x0300, out);                      /* 포트 이미지를 물리적 포트로 출력한다                    */
}
#endif
