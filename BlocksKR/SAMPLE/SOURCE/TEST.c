/*
*********************************************************************************************************
*                                    Embedded Systems Building Blocks
*                                 Complete and Ready-to-Use Modules in C
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* Filename   : TEST.C
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                                  상수
*********************************************************************************************************
*/

#define          TASK_STK_SIZE            512    /* 각 태스크의 스택 크기(16비트 워드 수)              */

/*
*********************************************************************************************************
*                                                  변수
*********************************************************************************************************
*/

OS_STK           TestStatTaskStk[TASK_STK_SIZE];

OS_STK           TestAIOTaskStk[TASK_STK_SIZE];
OS_STK           TestClkTaskStk[TASK_STK_SIZE];
OS_STK           TestDIOTaskStk[TASK_STK_SIZE];
OS_STK           TestRxTaskStk[TASK_STK_SIZE];
OS_STK           TestTxTaskStk[TASK_STK_SIZE];
OS_STK           TestTmrTaskStk[TASK_STK_SIZE];


/*
*********************************************************************************************************
*                                               함수 원형
*********************************************************************************************************
*/

static void       TestDispLit(void);
static void       TestDispMap(void);

       void       TestStatTask(void *data);
static void       TestInitModules(void);

       void       TestAIOTask(void *data);
       
       void       TestClkTask(void *data);
       
       void       TestDIOTask(void *data);
       
       void       TestRxTask(void *data);
       void       TestTxTask(void *data);
       
       void       TestTmrTask(void *data);
static void       TestTmr0TO(void *arg);
static void       TestTmr1TO(void *arg);

/*$PAGE*/
/*
*********************************************************************************************************
*                                                  MAIN
*
* 설명      : 이 곳은 프로그램의 시작 점이다. 만약 여러 분이 DOS 프롬프트에서 아무 인자도 지정하지 않고
*             다음 명령을 입력할 경우 임베디드 시스템 빌딩 블록 예제 코드가 실행될 것이다.
*
*                 TEST
*
*                   
*             만약 여러 분이 프롬프트 상에서 TEST에 어떤 인자를 지정한다면 PC의 문자 매핑이 표시될 것이다
*             다른 말로 여러 분은 다음과 같이 입력할 수 있다:
*
*                 TEST display
*
* 전달인자  : argc     표준 C 명령어 라인의 인자 수
*             argv     명령어 라인에서 전달된 인자를 포함하는 배열
*
* 리턴값    : 없음
*********************************************************************************************************
*/

void  main (int argc, char *argv[])
{
    if (argc > 1) {
        if (strcmp(argv[1], "display") == 0 || strcmp(argv[1], "DISPLAY") == 0) {
            TestDispMap();                                 /* PC의 문자 매핑을 표시한다                */
        }
        exit(0);                                           /* DOS로 돌아간다                           */
    }
    
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* 화면을 지운다                            */
    OSInit();                                              /* uC/OS-II를 초기화 한다                   */
    OSFPInit();                                            /* 부동소수연산을 초기화 한다               */
    PC_DOSSaveReturn();                                    /* DOS로 복귀하기 위한 환경을 저장한다      */   
    PC_VectSet(uCOS, OSCtxSw);                             /* uC/OS-II의 컨텍스트 스위치 벡터를 세트   */
    OSTaskCreateExt(TestStatTask, 
                   (void *)0, 
                   &TestStatTaskStk[TASK_STK_SIZE], 
                   STAT_TASK_PRIO,
                   STAT_TASK_PRIO, 
                   &TestStatTaskStk[0], 
                   TASK_STK_SIZE, 
                   (void *)0, 
                   OS_TASK_OPT_SAVE_FP);
    OSStart();                                             /* 멀티태스킹을 시작한다                    */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                          아날로그 I/O 태스크
*********************************************************************************************************
*/

void  TestAIOTask (void *data)
{
    char    s[81];
    FP32    value;
    INT16S  temp;
    INT8U   err;
    

    data = data;                                           /* 컴파일러 경고를 피한다                   */
    
    AICfgConv(0, 0.01220740, -4095.875, 10);               /* 아날로그 입력 채널 #0을 설정             */
    AICfgCal(0,  1.00,           0.00);
    
    for (;;) {
        err  = AIGet(0, &value);                           /* 아날로그 입력 채널 #0을 읽는다           */
        temp = (INT16S)value;                              /* 소수점 이하는 무시한다                   */
        sprintf(s, "%5d", temp);                           
        PC_DispStr(49, 11, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

        OSTimeDlyHMSM(0, 0, 0, 10);                        /* 초당 100회 반복                          */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                               클럭 태스크
*********************************************************************************************************
*/

void  TestClkTask (void *data)
{
    char    s[81];
    INT16U  time;
    TS      ts;


    data  = data;                                          /* 컴파일러 경고를 피한다                   */

    ClkSetDateTime(12, 31, 1999, 23, 58, 0);               /* 시계/달력을 세트한다                     */

    for (;;) {
        PC_ElapsedStart();
        ClkFormatDate(2, s);                               /* 시계/달력으로부터 포맷된 날짜를 얻는다   */
        time = PC_ElapsedStop();
        PC_DispStr( 8, 11, "                   ", DISP_FGND_WHITE);
        PC_DispStr( 8, 11, s, DISP_FGND_BLUE + DISP_BGND_CYAN);
        sprintf(s, "%3d uS", time);
        PC_DispStr( 8, 14, s, DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);

        PC_ElapsedStart();
        ClkFormatTime(1, s);                               /* 시계/달력으로부터 포맷된 시간을 얻는다   */
        time = PC_ElapsedStop();
        PC_DispStr( 8, 12, s, DISP_FGND_BLUE + DISP_BGND_CYAN);
        sprintf(s, "%3d uS", time);
        PC_DispStr(22, 14, s, DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);

        ts = ClkGetTS();                                   /* 현재의 타임스탬프를 얻는다               */
        ClkFormatTS(2, ts, s);                             /* 타임스탬프를 포맷한다                    */
        PC_DispStr( 8, 13, s, DISP_FGND_BLUE + DISP_BGND_CYAN);

        OSTimeDlyHMSM(0, 0, 0, 100);
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                            디지털 I/O 태스크
*********************************************************************************************************
*/

void  TestDIOTask (void *data)
{
    BOOLEAN state;


    data = data;                                           /* 컴파일러 경고를 피한다                   */
    
    DOSetSyncCtrMax(100);
    
    DOCfgBlink(0, DO_BLINK_EN,  5, 10);                    /* 디지털 출력 #0, 50%듀티 점멸             */
    DOCfgMode(0,  DO_MODE_BLINK_ASYNC, FALSE);
    
    DOCfgBlink(1, DO_BLINK_EN, 10, 20);                    /* 디지털 출력 #1, 50%듀티 점멸             */
    DOCfgMode(1,  DO_MODE_BLINK_ASYNC, FALSE);
    
    DOCfgBlink(2, DO_BLINK_EN, 25,  0);                    /* 디지털 출력 #2, 25%듀티 점멸             */
    DOCfgMode(2,  DO_MODE_BLINK_SYNC, FALSE);

    for (;;) {
        state = DOGet(0);
        if (state == TRUE) {
            PC_DispStr(49,  6, "TRUE ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        } else {
            PC_DispStr(49,  6, "FALSE", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        }
        state = DOGet(1);
        if (state == TRUE) {
            PC_DispStr(49,  7, "HIGH", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        } else {
            PC_DispStr(49,  7, "LOW ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        }
        state = DOGet(2);
        if (state == TRUE) {
            PC_DispStr(49,  8, "ON ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        } else {
            PC_DispStr(49,  8, "OFF", DISP_FGND_YELLOW + DISP_BGND_BLUE);
        }
        OSTimeDlyHMSM(0, 0, 0, 100);                       /* 초당 10회 반복한다                       */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                               화면 배치
*********************************************************************************************************
*/

static  void  TestDispLit (void)
{
                     /*           1111111111222222222233333333334444444444555555555566666666667777777777 */
                     /* 01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
    PC_DispStr( 0,  0, "                        EMBEDDED SYSTEMS BUILDING BLOCKS                        ", DISP_FGND_WHITE + DISP_BGND_RED + DISP_BLINK);
    PC_DispStr( 0,  1, "                     Complete and Ready-to-Use Modules in C                     ", DISP_FGND_WHITE);
    PC_DispStr( 0,  2, "                                Jean J. Labrosse                                ", DISP_FGND_WHITE);
    PC_DispStr( 0,  3, "                                  SAMPLE  CODE                                  ", DISP_FGND_WHITE);
    PC_DispStr( 0,  4, "                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0,  5, "Chapter 3, Keyboards                    Chapter  8, Discrete I/Os               ", DISP_FGND_WHITE);
    PC_DispStr( 0,  6, "Chapter 4, Multiplexed LED Displays       DO #0:        50% Duty Cycle (Async)  ", DISP_FGND_WHITE);
    PC_DispStr( 0,  7, "Chapter 5, Character LCD Modules          DO #1:        50% Duty Cycle (Async)  ", DISP_FGND_WHITE);
    PC_DispStr( 0,  8, "  -No Sample Code-                        DO #2:        25% Duty Cycle (Sync)   ", DISP_FGND_WHITE);
    PC_DispStr( 0,  9, "                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 10, "Chapter 6, Time-Of-Day Clock            Chapter 10, Analog I/Os                 ", DISP_FGND_WHITE);
    PC_DispStr( 0, 11, "  Date:                                   AI #0:                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 12, "  Time:                                                                         ", DISP_FGND_WHITE);
    PC_DispStr( 0, 13, "  TS  :                                                                         ", DISP_FGND_WHITE);
    PC_DispStr( 0, 14, "  Date:         Time:                                                           ", DISP_FGND_WHITE);
    PC_DispStr( 0, 15, "Chapter 7, Timer Manager                Chapter 11, Async. Serial Comm.         ", DISP_FGND_WHITE);
    PC_DispStr( 0, 16, "  Tmr0:                                   Tx   :                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 17, "                                          Rx   :                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 18, "  Tmr1:                                                                         ", DISP_FGND_WHITE);
    PC_DispStr( 0, 19, "                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 20, "                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 21, "                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 22, "                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 23, "MicroC/OS-II Vx.yy    #Tasks: xxxxx   #Task switch/sec: xxxxx   CPU Usage: xxx %", DISP_FGND_YELLOW + DISP_BGND_BLUE);
    PC_DispStr( 0, 24, "                            <-PRESS 'ESC' TO QUIT->                             ", DISP_FGND_WHITE);
                     /*           1111111111222222222233333333334444444444555555555566666666667777777777 */
                     /* 01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                              문자매핑 표시
*
* 설명      : 이 함수는 문자 맵을 표시한다.
*             테이블의 왼쪽은 0x00에서 0x7F의 값을 가지는 ASCII 문자에 해당하고
*             오른쪽은 0x80에서 0xFF의 값을 가지는 특수문자에 해당한다.
*
* 전달인자  : 없음
*
* 리턴값    : 없음
*********************************************************************************************************
*/

static  void  TestDispMap (void)
{
    INT8U  x;
    INT8U  y;
    INT16S key;
    
    
    PC_DispClrScr(DISP_FGND_WHITE);
    PC_DispStr( 0,  0, "                        EMBEDDED SYSTEMS BUILDING BLOCKS                        ", DISP_FGND_WHITE + DISP_BGND_RED + DISP_BLINK);
    PC_DispStr( 0,  1, "                     Complete and Ready-to-Use Modules in C                     ", DISP_FGND_WHITE);
    PC_DispStr( 0,  2, "                                Jean J. Labrosse                                ", DISP_FGND_WHITE);
    PC_DispStr( 0,  3, "                                 PC DISPLAY MAP                                 ", DISP_FGND_WHITE);
    PC_DispStr( 0,  4, "                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0,  5, "     ------ ASCII  Characters ------            ----- Special  Characters ----- ", DISP_FGND_WHITE);
    PC_DispStr( 0,  6, "     0 1 2 3 4 5 6 7 8 9 A B C D E F            0 1 2 3 4 5 6 7 8 9 A B C D E F ", DISP_FGND_WHITE);
    PC_DispStr( 0,  7, "                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0,  8, "0x00                                       0x80                                 ", DISP_FGND_WHITE);
    PC_DispStr( 0,  9, "                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 10, "0x10                                       0x90                                 ", DISP_FGND_WHITE);
    PC_DispStr( 0, 11, "                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 12, "0x20                                       0xA0                                 ", DISP_FGND_WHITE);
    PC_DispStr( 0, 13, "                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 14, "0x30                                       0xB0                                 ", DISP_FGND_WHITE);
    PC_DispStr( 0, 15, "                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 16, "0x40                                       0xC0                                 ", DISP_FGND_WHITE);
    PC_DispStr( 0, 17, "                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 18, "0x50                                       0xD0                                 ", DISP_FGND_WHITE);
    PC_DispStr( 0, 19, "                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 20, "0x60                                       0xE0                                 ", DISP_FGND_WHITE);
    PC_DispStr( 0, 21, "                                                                                                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 22, "0x70                                       0xF0                                                                                                                 ", DISP_FGND_WHITE);
    PC_DispStr( 0, 23, "                                                                                ", DISP_FGND_WHITE);
    PC_DispStr( 0, 24, "                            <PRESS ANY KEY TO QUIT>                             ", DISP_FGND_YELLOW + DISP_BGND_BLUE + DISP_BLINK);
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 16; x++) {
            PC_DispChar(x * 2 + 5, y * 2 + 8, y * 16 + x, DISP_FGND_YELLOW + DISP_BGND_BLUE);
        }
    }
    for (y = 8; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            PC_DispChar(x * 2 + 48, (y - 8) * 2 + 8, y * 16 + x, DISP_FGND_YELLOW + DISP_BGND_BLUE);
        }
    }
    while (PC_GetKey(&key) == FALSE) {
        ;
    }
    PC_DispClrScr(DISP_FGND_WHITE);
}

/*$PAGE*/    
/*
*********************************************************************************************************
*                                       임베디드 시스템 빌딩 블록
*                                           각 모듈의 초기화
*********************************************************************************************************
*/

static  void  TestInitModules (void)
{
    PC_ElapsedInit();

#if MODULE_KEY_MN
    KeyInit();                                             /* 키보드 스캐닝 모듈 초기화                */
#endif

#if MODULE_LED
    DispInit();                                            /* LED 모듈 초기화                          */
#endif

#if MODULE_LCD
    DispInit(4, 20);                                       /* LCD 모듈 초기화(4 x 20)                  */
#endif

#if MODULE_CLK
    ClkInit();                                             /* 시계/달력 모듈 초기화                    */
#endif

#if MODULE_TMR
    TmrInit();                                             /* 타이머 관리자 초기화                     */
#endif

#if MODULE_DIO
    DIOInit();                                             /* 디지털 I/O 모듈 초기화                   */
#endif

#if MODULE_AIO
    AIOInit();                                             /* 아날로그 I/O 모듈 초기화                 */
#endif

#if MODULE_COMM_BGND
    CommInit();                                            /* 시리얼 I/O 모듈 초기화                   */
#endif

#if MODULE_COMM_RTOS
    CommInit();                                            /* 시리얼 I/O 모듈 초기화                   */
#endif

#if MODULE_COMM_PC
    CommCfgPort(COMM1, 9600, 8, COMM_PARITY_NONE, 1);      /* PC의 COM1을 초기화 한다                  */
    CommSetIntVect(COMM1);                                 /* 인터럽트 벡터를 설정한다                 */
    CommRxIntEn(COMM1);                                    /* 수신 인터럽트 활성화                     */
#endif
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                       비동기 시리얼 통신 태스크
*                                           데이터 수신 태스크
*********************************************************************************************************
*/

void  TestRxTask (void *data)
{
    INT8U  err;
    INT8U  nbytes;
    INT8U  c;
    char   s[81];
    char  *ps;
    
    
    data   = data;                                                   /* 컴파일러 경고를 피한다         */
    for (;;) {
        ps     = s;
        nbytes = 0;
        do {
            c     = CommGetChar(COMM1, OS_TICKS_PER_SEC, &err);
            *ps++ = c;
            nbytes++;
        } while (c != '\n' && nbytes < 20);
        *ps = NUL;                                                   /* NUL로 끝나는 문자열            */
        PC_DispStr(49, 17, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                               통계 태스크
*
* 노트    : 1) 이 함수는 예제 코드에서 사용되는 모든 빌딩 블록을 초기화 하는 함수인 TestInitModules()를
*              호출한다. 몇몇 빌딩 블록은 실제적인 태스크를 생성하므로 우선순위를 할당해야 함을 유의한다 
*              태스크의 우선순위는 CFG.H에서 할당된다.
*
*********************************************************************************************************
*/

void  TestStatTask (void *pdata)
{
    INT8U   i;
    INT16S  key;
    char    s[81];


    pdata = pdata;                                         /* 컴파일러 경고를 피한다                   */


    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* uC/OS-II의 클럭 틱 ISR을 설정한다        */
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* 클럭 틱의 속도를 설정한다                */
    OS_EXIT_CRITICAL();

    PC_DispStr(0, 22, "Determining  CPU's capacity ...", DISP_FGND_WHITE);
    OSStatInit();                                          /* uC/OS-II통계 태스크를 초기화한다         */
    PC_DispClrRow(22, DISP_FGND_WHITE + DISP_BGND_BLACK);

    TestInitModules();                                     /* 모든 빌딩 블록을 초기화 한다...          */
                                                           /* ... 사용되는 태스크 포함                 */

                                                           /* 응용프로그램의 태스크 생성               */
    OSTaskCreateExt(TestClkTask, 
                   (void *)0, 
                   &TestClkTaskStk[TASK_STK_SIZE], 
                   TEST_CLK_TASK_PRIO,
                   TEST_CLK_TASK_PRIO,  
                   &TestClkTaskStk[0],  
                   TASK_STK_SIZE,  
                   (void *)0,  
                   OS_TASK_OPT_SAVE_FP);
    OSTaskCreateExt(TestRxTask, 
                   (void *)0, 
                   &TestRxTaskStk[TASK_STK_SIZE], 
                   TEST_RX_TASK_PRIO,
                   TEST_RX_TASK_PRIO,  
                   &TestRxTaskStk[0],  
                   TASK_STK_SIZE,  
                   (void *)0,  
                   OS_TASK_OPT_SAVE_FP);
    OSTaskCreateExt(TestTxTask, 
                   (void *)0, 
                   &TestTxTaskStk[TASK_STK_SIZE], 
                   TEST_TX_TASK_PRIO,
                   TEST_TX_TASK_PRIO,  
                   &TestTxTaskStk[0],  
                   TASK_STK_SIZE,  
                   (void *)0,  
                   OS_TASK_OPT_SAVE_FP);
    OSTaskCreateExt(TestTmrTask,         
                   (void *)0,  
                   &TestTmrTaskStk[TASK_STK_SIZE],  
                   TEST_TMR_TASK_PRIO,
                   TEST_TMR_TASK_PRIO,            
                   &TestTmrTaskStk[0],  
                   TASK_STK_SIZE,          
                   (void *)0,  
                   OS_TASK_OPT_SAVE_FP);
    OSTaskCreateExt(TestDIOTask,      
                   (void *)0,  
                   &TestDIOTaskStk[TASK_STK_SIZE],  
                   TEST_DIO_TASK_PRIO,
                   TEST_DIO_TASK_PRIO,        
                   &TestDIOTaskStk[0], 
                   TASK_STK_SIZE,        
                   (void *)0, 
                   OS_TASK_OPT_SAVE_FP);
    OSTaskCreateExt(TestAIOTask,  
                   (void *)0,  
                   &TestAIOTaskStk[TASK_STK_SIZE],  
                   TEST_AIO_TASK_PRIO,
                   TEST_AIO_TASK_PRIO,         
                   &TestAIOTaskStk[0], 
                   TASK_STK_SIZE,     
                   (void *)0, 
                   OS_TASK_OPT_SAVE_FP);
                   
    TestDispLit();                                         /* 고정 문자를 화면에 표시                  */
    
    sprintf(s, "V%1d.%02d", OSVersion() / 100, OSVersion() % 100);
    PC_DispStr(13, 23, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);
    
    for (;;) {
        sprintf(s, "%5d", OSTaskCtr);                      /* 실행되는 태스크의 수를 표시              */
        PC_DispStr(30, 23, s, DISP_FGND_BLUE + DISP_BGND_CYAN);

        sprintf(s, "%5d", OSCtxSwCtr);                     /* 초당 컨텍스트 스위치가 발생하는 수를 표시*/
        PC_DispStr(56, 23, s, DISP_FGND_BLUE + DISP_BGND_CYAN);

        sprintf(s, "%3d", OSCPUUsage);                     /* CPU 사용율을 %로 표시                    */
        PC_DispStr(75, 23, s, DISP_FGND_BLUE + DISP_BGND_CYAN);

        if (PC_GetKey(&key) == TRUE) {                     /* 키가 눌렸느지를 확인                     */     
            if (key == 0x1B) {                             /* 예, ESCAPE 키 인지 확인                  */
#if MODULE_COMM_PC
                CommRclIntVect(COMM1);                     /* 이전의 COMM1 벡터를 복구한다             */
#endif
                PC_DOSReturn();                            /* DOS로 돌아간다                           */
            }
        }

        OSCtxSwCtr = 0;

        OSTimeDlyHMSM(0, 0, 1, 0);                         /* 1초간 지연                               */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                               Function executed when Timers Time Out
*********************************************************************************************************
*/

static  void  TestTmr0TO (void *arg)
{
    arg = arg;
    PC_DispStr( 8, 17, "Timer #0 Timed Out!", DISP_FGND_WHITE + DISP_BGND_RED);
}



static  void  TestTmr1TO (void *arg)
{
    arg = arg;
    PC_DispStr( 8, 19, "Timer #1 Timed Out!", DISP_FGND_WHITE + DISP_BGND_RED);
}

/*
*********************************************************************************************************
*                                           타이머 관리 태스크
*********************************************************************************************************
*/

void  TestTmrTask (void *data)
{
    char    s[81];
    INT16U  time;


    data = data;                                           /* 컴파일러 경고를 피한다                   */
    
                                                           /* 타이머 #0 설정                           */
    TmrCfgFnct(0, TestTmr0TO, (void *)0);                  /* 타이머 #0이 만료되면 실행                */
    TmrSetMST(0, 1, 3, 9);                                 /* 타이머 #0을 1분 3.9초로 설정             */
    TmrStart(0);  
                                                           /* 타이머 #1 설정                           */
    TmrCfgFnct(1, TestTmr1TO, (void *)0);                  /* 타이머 #1이 만료되면 실행                */
    TmrSetMST(1, 2, 0, 0);                                 /* 타이머 #1을 2분으로 설정                 */
    TmrStart(1);
    
    for (;;) {
        TmrFormat(0, s);                                   /* 타이머 #0의 남은 시간을 얻는다           */
        PC_DispStr( 8, 16, s, DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);
        
        TmrFormat(1, s);                                   /* 타이머 #1의 남은 시간을 얻는다           */
        PC_DispStr( 8, 18, s, DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);

        OSTimeDlyHMSM(0, 0, 0, 50);                        /* 초당 20회 반복                           */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                      비동기 시리얼 통신 태스크
*                                          데이터 전송 태스크
*********************************************************************************************************
*/

void  TestTxTask (void *data)
{
    INT16U  ctr;
    char    s[81];
    char   *ps;
    
    
    data  = data;                                          /* 컴파일러 경고를 피한다                   */
    ctr   = 0;
    for (;;) {
        sprintf(s, "%05d\n", ctr);        
        PC_DispStr(49, 16, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);
        ps = s;
        while (*ps != NUL) {
            CommPutChar(COMM1, *ps, OS_TICKS_PER_SEC);
            OSTimeDly(5);                                  /* DOS 6.xx에서 실행한다면 이 라인을 지운다 */
            ps++;
        }
        ctr++;
    }
}
