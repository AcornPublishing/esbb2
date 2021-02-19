/*
*********************************************************************************************************
*                                    Embedded Systems Building Blocks
*                                 Complete and Ready-to-Use Modules in C
*
*                                         Matrix Keyboard Driver
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* Filename   : KEY.C
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*                                                  설명 
*
*    키보드 모듈은 4개의 행과 6개의 열로 구성된 매트릭스로 가정하고 있다. 그러나 이 코드는 8 x 8까지의 
*    어떤 형태의 매트릭스도 지원한다. 또한 이 드라이버는 열(column)입력 중 하나부터 세 개까지의 입력을
*    사용하면 3개까지의 쉬프트키를 지원한다. 이 키는 SHIFT1, SHIFT2 그리고 SHIFT3이다.
*
*    응용 프로그램은 다음을 반드시 설정해야 한다(KEY.H를 보라):
*
*    KEY_BUF_SIZE            키보드 버퍼의 크기
*
*    KEY_MAX_ROWS            키보드의 최대 행(row) 수
*    KEY_MAX_COLS            키보드의 최대 열(column) 수
*
*    KEY_RPT_DLY             자동 반복기능이 다시 수행되기 전의 스캔타임 값
*    KEY_RPT_START_DLY       자동 반복기능이 수행되기까지의 스캔타임 값
*
*    KEY_SCAN_TASK_DLY       키를 스캔하는 시간 간격
*    KEY_SCAN_TASK_PRIO      키보드 스캐닝 태스크의 우선순위
*    KEY_SCAN_TASK_STK_SIZE  키보드 스캐닝 태스크의 스택 크기
*
*    KEY_SHIFT1_MSK          SHIFT1키가 열입력 중 어떤 비트에 해당하는 가를 결정하는데 사용되는 마스크
*                                (0x00은 SHIFT1키가 없음을 말한다)
*    KEY_SHIFT1_OFFSET       SHIFT1키가 눌려졌을 때 더해질 오프셋 값
*
*    KEY_SHIFT2_MSK          SHIFT2키가 열입력 중 어떤 비트에 해당하는 가를 결정하는데 사용되는 마스크
*                                (0x00은 SHIFT2키가 없음을 말한다)
*    KEY_SHIFT2_OFFSET       SHIFT2키가 눌려졌을 때 더해질 오프셋 값
*
*    KEY_SHIFT3_MSK          SHIFT3키가 열입력 중 어떤 비트에 해당하는 가를 결정하는데 사용되는 마스크
*                                (0x00은 SHIFT3키가 없음을 말한다)
*    KEY_SHIFT3_OFFSET       SHIFT3키가 눌려졌을 때 더해질 오프셋 값
*
*
*    KEY_PORT_ROW            키보드 매트릭스의 행에 해당하는 포트의 어드레스
*    KEY_PORT_COL            키보드 매트릭스의 열에 해당하는 포트의 어드레스
*    KEY_PORT_CW             키보드 I/O포트 제어용 워드의 어드레스
*
*    KeyInitPort, KeySelRow() 그리고 KeyGetCol()이 하드웨어와 관련된 3개의 함수다.
*    이것은 하드웨어로의 인터페이스를 이들 3개의 함수로 지역화 함으로써 사용자 고유의 시스템으로
*    이식하는 것을 편리하도록 해준다.
*********************************************************************************************************
*/

/*$PAGE*/
/*
*********************************************************************************************************
*                                              인크루드 파일
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                                 지역상수
*********************************************************************************************************
*/

#define KEY_STATE_UP                 1      /* KeyScan()에서 사용되는 키 스캐닝 태스크의  상태         */
#define KEY_STATE_DEBOUNCE           2
#define KEY_STATE_RPT_START_DLY      3
#define KEY_STATE_RPT_DLY            4

/*
*********************************************************************************************************
*                                                 전역변수
*********************************************************************************************************
*/

static  INT8U     KeyBuf[KEY_BUF_SIZE];     /* 키보드 버퍼                                             */
static  INT8U     KeyBufInIx;               /* 키보드 버퍼에서 다음 번에 저장될 스캔코드의 인덱스      */
static  INT8U     KeyBufOutIx;              /* 키보드 버퍼에서 다음 번에 읽힐 스캔코드의 인덱스        */
static  INT16U    KeyDownTmr;               /* 키가 얼마나 오랫동안 눌려 있는 지를 카운트한다          */
static  INT8U     KeyNRead;                 /* 키보드로부터 읽힌 키의 개수                             */

static  INT8U     KeyRptStartDlyCtr;        /* 자동반복 기능이 시작되기 전의 스캔타임                  */
static  INT8U     KeyRptDlyCtr;             /* 자동반복 기능이 다시 시작되기 전의 스캔타임             */

static  INT8U     KeyScanState;             /* 키보드 스캐닝 함수의 현재 상태                          */

static  OS_STK    KeyScanTaskStk[KEY_SCAN_TASK_STK_SIZE];  /* 키보드 스캐닝 태스크의 스택              */

static  OS_EVENT *KeySemPtr;                               /* 키보드 세마포어의 포인터                 */

/*
*********************************************************************************************************
*                                             지역함수 원형
*********************************************************************************************************
*/

static  void      KeyBufIn(INT8U code);     /* 스캔코드를 키보드 버퍼에 저장한다                       */
static  INT8U     KeyDecode(void);          /* 현재 눌려진 키보드의 스캔코드를 얻는다                  */
static  BOOLEAN   KeyIsKeyDown(void);       /* 키가 눌려져 있는 지를 확인한다                          */
static  void      KeyScanTask(void *data);  /* 키보드 스캐닝 태스크                                    */

/*$PAGE*/
/*
*********************************************************************************************************
*                                      키 문자를 키보드 버퍼에 저장
*
* 설명     : 이 함수는 키 문자를 키보드 버퍼에 저장한다
* 전달인자 : code 는 키보드 버퍼에 저장될 스캔코드다
* 리턴값   : 없음
*********************************************************************************************************
*/

static  void  KeyBufIn (INT8U code)
{
    OS_ENTER_CRITICAL();                         /* 크리티컬 섹션을 시작함. 인터럽트가 디세이블 된다   */
    if (KeyNRead < KEY_BUF_SIZE) {               /* 버퍼에 오버플로우가 없는지 확인한다                */
        KeyNRead++;                              /* 읽혀진 키의 숫자를 증가한다                        */
        KeyBuf[KeyBufInIx++] = code;             /* 스캔코드를 버퍼에 저장한다                         */
        if (KeyBufInIx >= KEY_BUF_SIZE) {        /* 다음 번에 저장될 버퍼 인덱스를 조절한다            */
            KeyBufInIx = 0;
        }
        OS_EXIT_CRITICAL();                      /* 크리티컬 섹션을 종료함                             */
        OSSemPost(KeySemPtr);                    /* 버퍼에 스캔코드가 저장된 경우 세마포어를 시그널링함*/
    } else {                                     /* 버퍼가 꽉 찼음. 스캔코드를 잃어버림                */
        OS_EXIT_CRITICAL();                      /* 크리티컬 섹션을 종료함                             */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           키보드 디코드
*
* 설명     : 이 함수는 눌려진 키의 스캔코드를 결정하기 위해 호출된다
* 전달인자 : 없음
* 리턴값   : 키 스캔코드
*********************************************************************************************************
*/

static  INT8U  KeyDecode (void)
{
    INT8U   col;
    INT8U   row;
    INT8U   offset;
    BOOLEAN done;
    INT8U   col_id;
    INT8U   msk;


    done = FALSE;
    row  = 0;
    while (row < KEY_MAX_ROWS && !done) {             /* 몇 번째 행의 키가 눌렸는 지를 알아낸다        */
        KeySelRow(row);                               /* 행을 선택한다                                 */
        if (KeyIsKeyDown()) {                         /* 선택된 행의 키가 눌렸는 지를 확인한다         */
            done = TRUE;                              /* 행 찾기를 종료한다                            */
        } else {
            row++;                                    /* 다음 행을 선택한다                            */
        }
    }
    col    = KeyGetCol();                             /* 열을 읽는다                                   */
    offset = 0;                                       /* 아무런 시프트 키도 눌리지 않는 것으로 한다    */
    if (col & KEY_SHIFT1_MSK) {                       /* SHIFT1 또한 눌려 있는지 확인                  */
        offset += KEY_SHIFT1_OFFSET;
    }
    if (col & KEY_SHIFT2_MSK) {                       /* SHIFT2 또한 눌려 있는지 확인                  */
        offset += KEY_SHIFT2_OFFSET;
    }
    if (col & KEY_SHIFT3_MSK) {                       /* SHIFT3 또한 눌려 있는지 확인                  */
        offset += KEY_SHIFT3_OFFSET;
    }
    msk    = 0x01;                                    /* 열을 스캔하기 위해서 비트 마스크 함           */
    col_id =    0;                                    /* 열의 값을 세트한다 (0..7)                     */
    done   = FALSE;
    while (col_id < KEY_MAX_COLS && !done) {          /* 모든 열을 조사한다                            */
        if (col & msk) {                              /* 이번 열이 눌려 있는 지를 확인한다             */
            done  = TRUE;                             /* 종료, i에 키의 열의 값이 저장되어 있음(0..7)  */
        } else {
            col_id++;
            msk <<= 1;
        }
    }
    return (row * KEY_MAX_COLS + offset + col_id);    /* 스캔코드를 리턴한다                           */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                              키보드 버퍼 플러쉬
*
* 설명     : 이 함수는 키보드 버퍼를 클리어 한다
* 전달인자 : 없음
* 리턴값   : 없음
*********************************************************************************************************
*/

void  KeyFlush (void)
{
    while (KeyHit()) {                           /* 키보드 버퍼에 스캔코드가 있는 동안 반복...         */
        KeyGetKey(0);                            /* ... 버퍼로 부터 다음 스캔코드를 읽어온다           */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             스캔코드를 구한다
*
* 설명     : 키보드 드라이버로부터 스캔코드를 읽어온다
* 전달인자 : 'to' 는 키가 눌려질 때까지 KeyGetKey()가 기다려야 하는 시간을 나타낸다
*            0의 값은 키가 눌려질 때까지 영원히 기다려야 함을 의미한다
* 리턴값   : != 0xFF 인 경우는 눌려진 키의 스캔코드 임
*            == 0xFF 인 경우는 'to'에 의해 설정된 타임아웃 때까지 키가 눌려지지 않았음을 나타낸다
*********************************************************************************************************
*/

INT8U  KeyGetKey (INT16U to)
{
    INT8U code;
    INT8U err;


    OSSemPend(KeySemPtr, to, &err);              /* 키가 눌려질 때를 기다린다                          */
    OS_ENTER_CRITICAL();                         /* 크리티컬 섹션을 시작한다. 인터럽트는 디세이블 된다 */
    if (KeyNRead > 0) {                          /* 키보드 버퍼에 스캔코드가 있는 지를 확인한다        */
        KeyNRead--;                              /* 읽혀진 키의 개수를 감소시킨다                      */
        code = KeyBuf[KeyBufOutIx];              /* 버퍼로 부터 스캔코드를 읽어온다                    */
        KeyBufOutIx++;
        if (KeyBufOutIx >= KEY_BUF_SIZE) {       /* 키보드 버퍼의 인덱스를 조절한다                    */
            KeyBufOutIx = 0;
        }
        OS_EXIT_CRITICAL();                      /* 크리티컬 섹션을 종료한다                           */
        return (code);                           /* 눌려진 키의 스캔코드를 리턴한다                    */
    } else {
        OS_EXIT_CRITICAL();                      /* 크리티컬 섹션을 종료한다                           */
        return (0xFF);                           /* 버퍼에 스캔코드가 없음. -1을 리턴한다              */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                  키가 얼마나 오래 눌려져 있었는 지를 알아냄
*
* 설명     : 이 함수는 키가 눌려져 있는 시간을 리턴한다
* 전달인자 : 없음
* 리턴값   : 키가 눌려진 시간(밀리초, milliseconds)
*********************************************************************************************************
*/

INT32U  KeyGetKeyDownTime (void)
{
    INT16U tmr;


    OS_ENTER_CRITICAL();
    tmr = KeyDownTmr;
    OS_EXIT_CRITICAL();
    return (tmr * KEY_SCAN_TASK_DLY);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                 버퍼에 스캔코드가 저장되어 있는 지 알아냄
*
* 설명     : 이 함수는 키가 눌려져 있었는 지를 확인한다
* 전달인자 : 없음
* 리턴값   : TRUE   키가 눌려졌던 경우
*            FALSE  키가 눌려지지 않았던 경우
*********************************************************************************************************
*/

BOOLEAN  KeyHit (void)
{
    BOOLEAN hit;


    OS_ENTER_CRITICAL();
    hit = (BOOLEAN)(KeyNRead > 0) ? TRUE : FALSE;
    OS_EXIT_CRITICAL();
    return (hit);
}

/*
*********************************************************************************************************
*                                             키보드 초기화
*
* 설명     : 키보드 초기화 함수. KeyInit()는 키보드 관련된 사용자 함수를 호출하기 전에 반드시 최초로 
*            호출되어야 한다
* 전달인자 : 없음
* 리턴값   : 없음
*********************************************************************************************************
*/

void  KeyInit (void)
{
    KeySelRow(KEY_ALL_ROWS);                     /* 모든 행을 선택한다                                 */
    KeyScanState = KEY_STATE_UP;                 /* 키보드 스캐닝 태스크의 초기상태를 세트한다         */
    KeyNRead     = 0;                            /* 읽혀진 키의 수를 저장하는 변수 초기화              */
    KeyDownTmr   = 0;
    KeyBufInIx   = 0;                            /* 저장될 스캔코드는 버퍼의 처음부터 시작됨           */
    KeyBufOutIx  = 0;                            /* 읽혀질 스캔코드는 버퍼의 처음부터 시작됨           */
    KeySemPtr    = OSSemCreate(0);               /* 키보드 세마포어 초기화                             */
    KeyInitPort();                               /* 키보드 드라이버에서 사용되는 I/O포트 초기화        */
    OSTaskCreate(KeyScanTask, (void *)0, &KeyScanTaskStk[KEY_SCAN_TASK_STK_SIZE], KEY_SCAN_TASK_PRIO);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           SEE IF KEY PRESSED
*
* 설명     : 이 함수는 현재 키가 눌려져 있는 지를 알아낸다
* 전달인자 : 없음
* 리턴값   : TRUE   키가 눌려진 경우
*            FALSE  키가 눌려지지 않은 경우
* 노트     : (1 << KEY_MAX_COLS) - 1  는 열입력을 분리하기 위해 사용되는 마스크 값이다
*            (i.e. mask off the SHIFT keys).
*********************************************************************************************************
*/

static  BOOLEAN  KeyIsKeyDown (void)
{
    if (KeyGetCol() & ((1 << KEY_MAX_COLS) - 1)) {         /* 0이면 키가 눌려지지 않음을 의미          */
        OS_ENTER_CRITICAL();
        KeyDownTmr++;                                      /* 키다운 카운터를 증가한다                 */
        OS_EXIT_CRITICAL();
        return (TRUE);
    } else {
        return (FALSE);
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                          키보드 스캐닝 태스크
*
* 설명     : 이 함수는 키보드 스캐닝 태스크다. 이 태스크의 우선순위는 낮게 할당돼야 하며
*            스캐닝 주기는 KEY_SCAN_TASK_DLY에 의해 결정된다
* 전달인자 : 'data'  태스크가 생성될 때 전달되는 인자의 포인터(사용되지는 않음)
* 리턴값   : KeyScanTask()는 복귀되지 않는다
* 노트     : KEY_RPT_START_DLY보다 오랫동안 키가 눌려져 있는 경우 자동반복 기능이 동작한다
*            일단 자동반복 기능이 시작되면 키가 눌려져 있는 한 KEY_RPT_DLY마다 반복된다
*            예를 들면, 키보드 스캐닝이 50msec마다 수행되고 KEY_RPT_START_DLY는 40, KEY_RPT_DLY는 2로
*            설정되어 있다면, 자동반복은 키를 누른 후 2초후에 시작되고 100msec마다 키가 반복될 것이다
*
*
*********************************************************************************************************
*/

/*$PAGE*/
static  void  KeyScanTask (void *data)
{
    INT8U code;


    data = data;                                           /* 컴파일러 경고를 피한다 (uC/OS-II req.)   */
    for (;;) {
        OSTimeDlyHMSM(0, 0, 0, KEY_SCAN_TASK_DLY);         /* 키보드 스캐닝 딜레이                     */
        switch (KeyScanState) {
            case KEY_STATE_UP:                             /* 키가 눌려져 있는 지를 확인할 필요가 있음 */
                 if (KeyIsKeyDown()) {                     /* 키눌림을 확인한다                        */
                     KeyScanState = KEY_STATE_DEBOUNCE;    /* 디바운스 스테이트로 상태 변경            */
                     KeyDownTmr   = 0;                     /* 키다운 타이머를 리셋                     */
                 }
                 break;

            case KEY_STATE_DEBOUNCE:                       /* 키 눌림, 스캔코드를 확인                 */
                 if (KeyIsKeyDown()) {                     /* 키가 눌렸는가?                           */
                     code              = KeyDecode();      /* 눌린 키의 스캔코드를 알아낸다            */
                     KeyBufIn(code);                       /* 스캔코드를 버퍼에 저장                   */
                     KeyRptStartDlyCtr = KEY_RPT_START_DLY;/* 자동반복 기능을 시작하기 위해 딜레이 시작*/
                     KeyScanState      = KEY_STATE_RPT_START_DLY;
                 } else {
                     KeySelRow(KEY_ALL_ROWS);              /* 모든 행을 선택                           */
                     KeyScanState      = KEY_STATE_UP;     /* 키가 눌려져 있지 않다                    */
                 }
                 break;

            case KEY_STATE_RPT_START_DLY:
                 if (KeyIsKeyDown()) {                     /* 아직도 키가 눌려져 있는가?               */
                     if (KeyRptStartDlyCtr > 0) {          /* 자동반복 기능 시작전에 딜레이가 필요한가?*/
                         KeyRptStartDlyCtr--;              /* 예, 자동반복 시작을 위해 카운터를 감소   */
                         if (KeyRptStartDlyCtr == 0) {     /* 자동반복을 위한 딜레이가 완료되었는가?...*/
                             code         = KeyDecode();   /* 스캔코드를 알아낸다                      */
                             KeyBufIn(code);               /* 버퍼에 스캔코드를 저장한다               */
                             KeyRptDlyCtr = KEY_RPT_DLY;   /* 다음 반복을 위한 딜레이 값을 로드        */
                             KeyScanState = KEY_STATE_RPT_DLY;
                         }
                     }
                 } else {
                     KeyScanState = KEY_STATE_DEBOUNCE;    /* 키가 눌려져 있지 않다                    */
                 }
                 break;

            case KEY_STATE_RPT_DLY:
                 if (KeyIsKeyDown()) {                     /* 아직도 키가 눌려져 있는가?               */
                     if (KeyRptDlyCtr > 0) {               /* 키 반복전에 기다릴 필요가 있는가?        */
                         KeyRptDlyCtr--;                   /* 예, 감소하고 다음 키반복을 기다린다      */
                         if (KeyRptDlyCtr == 0) {          /* 키를 반복할 때인가                       */
                             code         = KeyDecode();   /* 스캔코드를 알아낸다                      */
                             KeyBufIn(code);               /* 스캔코드를 저장한다                      */
                             KeyRptDlyCtr = KEY_RPT_DLY;   /* 자동반복에 필요한 딜레이 값을 리로드한다 */
                         }
                     }
                 } else {
                     KeyScanState = KEY_STATE_DEBOUNCE;    /* 키가 눌려져 있지 않음                    */
                 }
                 break;
        }
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                                 열 읽기
*
* 설명     : 이 함수는 열 포트를 읽기위해 호출된다
* 전달인자 : 없음
* 리턴값   : 열 포트의 컴플리먼트 이므로 1의 값을 갖는 비트에 해당하는 열이 눌려짐
*********************************************************************************************************
*/

#ifndef CFG_C
INT8U  KeyGetCol (void)
{
    return (~inp(KEY_PORT_COL));                 /* Complement columns (1이 키가 눌렸음을 의미)        */
}
#endif


/*
*********************************************************************************************************
*                                             I/O 포트 초기화
*********************************************************************************************************
*/

#ifndef CFG_C
void  KeyInitPort (void)
{
    outp(KEY_PORT_CW, 0x82);                     /* 82C55초기화 : A=출력, B=입력(열), C=출력(행)       */
}
#endif


/*
*********************************************************************************************************
*                                                 행 선택
*
* 설명     : 이 함수는 키보드의 행을 선택하기 위해 호출된다
* 전달인자 : row 는 행의 수를 말함(0..7) 또는 KEY_ALL_ROWS
* 리턴값   : 없음
* Note     : 각 행은 0을 출력함으로써 선택됨
*********************************************************************************************************
*/

#ifndef CFG_C
void  KeySelRow (INT8U row)
{
    if (row == KEY_ALL_ROWS) {
        outp(KEY_PORT_ROW, 0x00);                /* 모든 행을 0으로 출력함                             */
    } else {
        outp(KEY_PORT_ROW, ~(1 << row));         /* 원하는 행만 0으로 출력함                           */
    }
}
#endif
