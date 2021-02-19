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
*                                                  ���
*********************************************************************************************************
*/

#define          TASK_STK_SIZE            512    /* �� �½�ũ�� ���� ũ��(16��Ʈ ���� ��)              */

/*
*********************************************************************************************************
*                                                  ����
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
*                                               �Լ� ����
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
* ����      : �� ���� ���α׷��� ���� ���̴�. ���� ���� ���� DOS ������Ʈ���� �ƹ� ���ڵ� �������� �ʰ�
*             ���� ����� �Է��� ��� �Ӻ���� �ý��� ���� ��� ���� �ڵ尡 ����� ���̴�.
*
*                 TEST
*
*                   
*             ���� ���� ���� ������Ʈ �󿡼� TEST�� � ���ڸ� �����Ѵٸ� PC�� ���� ������ ǥ�õ� ���̴�
*             �ٸ� ���� ���� ���� ������ ���� �Է��� �� �ִ�:
*
*                 TEST display
*
* ��������  : argc     ǥ�� C ��ɾ� ������ ���� ��
*             argv     ��ɾ� ���ο��� ���޵� ���ڸ� �����ϴ� �迭
*
* ���ϰ�    : ����
*********************************************************************************************************
*/

void  main (int argc, char *argv[])
{
    if (argc > 1) {
        if (strcmp(argv[1], "display") == 0 || strcmp(argv[1], "DISPLAY") == 0) {
            TestDispMap();                                 /* PC�� ���� ������ ǥ���Ѵ�                */
        }
        exit(0);                                           /* DOS�� ���ư���                           */
    }
    
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* ȭ���� �����                            */
    OSInit();                                              /* uC/OS-II�� �ʱ�ȭ �Ѵ�                   */
    OSFPInit();                                            /* �ε��Ҽ������� �ʱ�ȭ �Ѵ�               */
    PC_DOSSaveReturn();                                    /* DOS�� �����ϱ� ���� ȯ���� �����Ѵ�      */   
    PC_VectSet(uCOS, OSCtxSw);                             /* uC/OS-II�� ���ؽ�Ʈ ����ġ ���͸� ��Ʈ   */
    OSTaskCreateExt(TestStatTask, 
                   (void *)0, 
                   &TestStatTaskStk[TASK_STK_SIZE], 
                   STAT_TASK_PRIO,
                   STAT_TASK_PRIO, 
                   &TestStatTaskStk[0], 
                   TASK_STK_SIZE, 
                   (void *)0, 
                   OS_TASK_OPT_SAVE_FP);
    OSStart();                                             /* ��Ƽ�½�ŷ�� �����Ѵ�                    */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                          �Ƴ��α� I/O �½�ũ
*********************************************************************************************************
*/

void  TestAIOTask (void *data)
{
    char    s[81];
    FP32    value;
    INT16S  temp;
    INT8U   err;
    

    data = data;                                           /* �����Ϸ� ��� ���Ѵ�                   */
    
    AICfgConv(0, 0.01220740, -4095.875, 10);               /* �Ƴ��α� �Է� ä�� #0�� ����             */
    AICfgCal(0,  1.00,           0.00);
    
    for (;;) {
        err  = AIGet(0, &value);                           /* �Ƴ��α� �Է� ä�� #0�� �д´�           */
        temp = (INT16S)value;                              /* �Ҽ��� ���ϴ� �����Ѵ�                   */
        sprintf(s, "%5d", temp);                           
        PC_DispStr(49, 11, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

        OSTimeDlyHMSM(0, 0, 0, 10);                        /* �ʴ� 100ȸ �ݺ�                          */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                               Ŭ�� �½�ũ
*********************************************************************************************************
*/

void  TestClkTask (void *data)
{
    char    s[81];
    INT16U  time;
    TS      ts;


    data  = data;                                          /* �����Ϸ� ��� ���Ѵ�                   */

    ClkSetDateTime(12, 31, 1999, 23, 58, 0);               /* �ð�/�޷��� ��Ʈ�Ѵ�                     */

    for (;;) {
        PC_ElapsedStart();
        ClkFormatDate(2, s);                               /* �ð�/�޷����κ��� ���˵� ��¥�� ��´�   */
        time = PC_ElapsedStop();
        PC_DispStr( 8, 11, "                   ", DISP_FGND_WHITE);
        PC_DispStr( 8, 11, s, DISP_FGND_BLUE + DISP_BGND_CYAN);
        sprintf(s, "%3d uS", time);
        PC_DispStr( 8, 14, s, DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);

        PC_ElapsedStart();
        ClkFormatTime(1, s);                               /* �ð�/�޷����κ��� ���˵� �ð��� ��´�   */
        time = PC_ElapsedStop();
        PC_DispStr( 8, 12, s, DISP_FGND_BLUE + DISP_BGND_CYAN);
        sprintf(s, "%3d uS", time);
        PC_DispStr(22, 14, s, DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);

        ts = ClkGetTS();                                   /* ������ Ÿ�ӽ������� ��´�               */
        ClkFormatTS(2, ts, s);                             /* Ÿ�ӽ������� �����Ѵ�                    */
        PC_DispStr( 8, 13, s, DISP_FGND_BLUE + DISP_BGND_CYAN);

        OSTimeDlyHMSM(0, 0, 0, 100);
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                            ������ I/O �½�ũ
*********************************************************************************************************
*/

void  TestDIOTask (void *data)
{
    BOOLEAN state;


    data = data;                                           /* �����Ϸ� ��� ���Ѵ�                   */
    
    DOSetSyncCtrMax(100);
    
    DOCfgBlink(0, DO_BLINK_EN,  5, 10);                    /* ������ ��� #0, 50%��Ƽ ����             */
    DOCfgMode(0,  DO_MODE_BLINK_ASYNC, FALSE);
    
    DOCfgBlink(1, DO_BLINK_EN, 10, 20);                    /* ������ ��� #1, 50%��Ƽ ����             */
    DOCfgMode(1,  DO_MODE_BLINK_ASYNC, FALSE);
    
    DOCfgBlink(2, DO_BLINK_EN, 25,  0);                    /* ������ ��� #2, 25%��Ƽ ����             */
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
        OSTimeDlyHMSM(0, 0, 0, 100);                       /* �ʴ� 10ȸ �ݺ��Ѵ�                       */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                               ȭ�� ��ġ
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
*                                              ���ڸ��� ǥ��
*
* ����      : �� �Լ��� ���� ���� ǥ���Ѵ�.
*             ���̺��� ������ 0x00���� 0x7F�� ���� ������ ASCII ���ڿ� �ش��ϰ�
*             �������� 0x80���� 0xFF�� ���� ������ Ư�����ڿ� �ش��Ѵ�.
*
* ��������  : ����
*
* ���ϰ�    : ����
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
*                                       �Ӻ���� �ý��� ���� ���
*                                           �� ����� �ʱ�ȭ
*********************************************************************************************************
*/

static  void  TestInitModules (void)
{
    PC_ElapsedInit();

#if MODULE_KEY_MN
    KeyInit();                                             /* Ű���� ��ĳ�� ��� �ʱ�ȭ                */
#endif

#if MODULE_LED
    DispInit();                                            /* LED ��� �ʱ�ȭ                          */
#endif

#if MODULE_LCD
    DispInit(4, 20);                                       /* LCD ��� �ʱ�ȭ(4 x 20)                  */
#endif

#if MODULE_CLK
    ClkInit();                                             /* �ð�/�޷� ��� �ʱ�ȭ                    */
#endif

#if MODULE_TMR
    TmrInit();                                             /* Ÿ�̸� ������ �ʱ�ȭ                     */
#endif

#if MODULE_DIO
    DIOInit();                                             /* ������ I/O ��� �ʱ�ȭ                   */
#endif

#if MODULE_AIO
    AIOInit();                                             /* �Ƴ��α� I/O ��� �ʱ�ȭ                 */
#endif

#if MODULE_COMM_BGND
    CommInit();                                            /* �ø��� I/O ��� �ʱ�ȭ                   */
#endif

#if MODULE_COMM_RTOS
    CommInit();                                            /* �ø��� I/O ��� �ʱ�ȭ                   */
#endif

#if MODULE_COMM_PC
    CommCfgPort(COMM1, 9600, 8, COMM_PARITY_NONE, 1);      /* PC�� COM1�� �ʱ�ȭ �Ѵ�                  */
    CommSetIntVect(COMM1);                                 /* ���ͷ�Ʈ ���͸� �����Ѵ�                 */
    CommRxIntEn(COMM1);                                    /* ���� ���ͷ�Ʈ Ȱ��ȭ                     */
#endif
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                       �񵿱� �ø��� ��� �½�ũ
*                                           ������ ���� �½�ũ
*********************************************************************************************************
*/

void  TestRxTask (void *data)
{
    INT8U  err;
    INT8U  nbytes;
    INT8U  c;
    char   s[81];
    char  *ps;
    
    
    data   = data;                                                   /* �����Ϸ� ��� ���Ѵ�         */
    for (;;) {
        ps     = s;
        nbytes = 0;
        do {
            c     = CommGetChar(COMM1, OS_TICKS_PER_SEC, &err);
            *ps++ = c;
            nbytes++;
        } while (c != '\n' && nbytes < 20);
        *ps = NUL;                                                   /* NUL�� ������ ���ڿ�            */
        PC_DispStr(49, 17, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                               ��� �½�ũ
*
* ��Ʈ    : 1) �� �Լ��� ���� �ڵ忡�� ���Ǵ� ��� ���� ����� �ʱ�ȭ �ϴ� �Լ��� TestInitModules()��
*              ȣ���Ѵ�. ��� ���� ����� �������� �½�ũ�� �����ϹǷ� �켱������ �Ҵ��ؾ� ���� �����Ѵ� 
*              �½�ũ�� �켱������ CFG.H���� �Ҵ�ȴ�.
*
*********************************************************************************************************
*/

void  TestStatTask (void *pdata)
{
    INT8U   i;
    INT16S  key;
    char    s[81];


    pdata = pdata;                                         /* �����Ϸ� ��� ���Ѵ�                   */


    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* uC/OS-II�� Ŭ�� ƽ ISR�� �����Ѵ�        */
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Ŭ�� ƽ�� �ӵ��� �����Ѵ�                */
    OS_EXIT_CRITICAL();

    PC_DispStr(0, 22, "Determining  CPU's capacity ...", DISP_FGND_WHITE);
    OSStatInit();                                          /* uC/OS-II��� �½�ũ�� �ʱ�ȭ�Ѵ�         */
    PC_DispClrRow(22, DISP_FGND_WHITE + DISP_BGND_BLACK);

    TestInitModules();                                     /* ��� ���� ����� �ʱ�ȭ �Ѵ�...          */
                                                           /* ... ���Ǵ� �½�ũ ����                 */

                                                           /* �������α׷��� �½�ũ ����               */
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
                   
    TestDispLit();                                         /* ���� ���ڸ� ȭ�鿡 ǥ��                  */
    
    sprintf(s, "V%1d.%02d", OSVersion() / 100, OSVersion() % 100);
    PC_DispStr(13, 23, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);
    
    for (;;) {
        sprintf(s, "%5d", OSTaskCtr);                      /* ����Ǵ� �½�ũ�� ���� ǥ��              */
        PC_DispStr(30, 23, s, DISP_FGND_BLUE + DISP_BGND_CYAN);

        sprintf(s, "%5d", OSCtxSwCtr);                     /* �ʴ� ���ؽ�Ʈ ����ġ�� �߻��ϴ� ���� ǥ��*/
        PC_DispStr(56, 23, s, DISP_FGND_BLUE + DISP_BGND_CYAN);

        sprintf(s, "%3d", OSCPUUsage);                     /* CPU ������� %�� ǥ��                    */
        PC_DispStr(75, 23, s, DISP_FGND_BLUE + DISP_BGND_CYAN);

        if (PC_GetKey(&key) == TRUE) {                     /* Ű�� ���ȴ����� Ȯ��                     */     
            if (key == 0x1B) {                             /* ��, ESCAPE Ű ���� Ȯ��                  */
#if MODULE_COMM_PC
                CommRclIntVect(COMM1);                     /* ������ COMM1 ���͸� �����Ѵ�             */
#endif
                PC_DOSReturn();                            /* DOS�� ���ư���                           */
            }
        }

        OSCtxSwCtr = 0;

        OSTimeDlyHMSM(0, 0, 1, 0);                         /* 1�ʰ� ����                               */
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
*                                           Ÿ�̸� ���� �½�ũ
*********************************************************************************************************
*/

void  TestTmrTask (void *data)
{
    char    s[81];
    INT16U  time;


    data = data;                                           /* �����Ϸ� ��� ���Ѵ�                   */
    
                                                           /* Ÿ�̸� #0 ����                           */
    TmrCfgFnct(0, TestTmr0TO, (void *)0);                  /* Ÿ�̸� #0�� ����Ǹ� ����                */
    TmrSetMST(0, 1, 3, 9);                                 /* Ÿ�̸� #0�� 1�� 3.9�ʷ� ����             */
    TmrStart(0);  
                                                           /* Ÿ�̸� #1 ����                           */
    TmrCfgFnct(1, TestTmr1TO, (void *)0);                  /* Ÿ�̸� #1�� ����Ǹ� ����                */
    TmrSetMST(1, 2, 0, 0);                                 /* Ÿ�̸� #1�� 2������ ����                 */
    TmrStart(1);
    
    for (;;) {
        TmrFormat(0, s);                                   /* Ÿ�̸� #0�� ���� �ð��� ��´�           */
        PC_DispStr( 8, 16, s, DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);
        
        TmrFormat(1, s);                                   /* Ÿ�̸� #1�� ���� �ð��� ��´�           */
        PC_DispStr( 8, 18, s, DISP_FGND_RED + DISP_BGND_LIGHT_GRAY);

        OSTimeDlyHMSM(0, 0, 0, 50);                        /* �ʴ� 20ȸ �ݺ�                           */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                      �񵿱� �ø��� ��� �½�ũ
*                                          ������ ���� �½�ũ
*********************************************************************************************************
*/

void  TestTxTask (void *data)
{
    INT16U  ctr;
    char    s[81];
    char   *ps;
    
    
    data  = data;                                          /* �����Ϸ� ��� ���Ѵ�                   */
    ctr   = 0;
    for (;;) {
        sprintf(s, "%05d\n", ctr);        
        PC_DispStr(49, 16, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);
        ps = s;
        while (*ps != NUL) {
            CommPutChar(COMM1, *ps, OS_TICKS_PER_SEC);
            OSTimeDly(5);                                  /* DOS 6.xx���� �����Ѵٸ� �� ������ ����� */
            ps++;
        }
        ctr++;
    }
}
