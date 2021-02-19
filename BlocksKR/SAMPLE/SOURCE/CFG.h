/*
*********************************************************************************************************
*                                  Embedded Systems Building Blocks
*                               Complete and Ready-to-Use Modules in C
*
*                                        Configuration Header File
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* Filename   : CFG.H
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          실험 태스크 우선순위
*********************************************************************************************************
*/
                                                 
#define  TEST_AIO_TASK_PRIO         10
#define  TEST_CLK_TASK_PRIO         11
#define  TEST_TX_TASK_PRIO          12
#define  TEST_RX_TASK_PRIO          13
#define  TEST_DIO_TASK_PRIO         14
#define  TEST_TMR_TASK_PRIO         15

#define  STAT_TASK_PRIO             16

/*
*********************************************************************************************************
*                                            키보드 설정 상수
*                                                 (3장)
*
* 노트: 이들 #define 상수들은 사용자 환경에 맞도록 변경되어야 한다
*********************************************************************************************************
*/

#if      MODULE_KEY_MN

#define  KEY_BUF_SIZE               10      /* 키보드 버퍼의 크기                                      */

#define  KEY_MAX_ROWS                4      /* 키보드의 최대 행(row) 수                                */
#define  KEY_MAX_COLS                6      /* 키보드의 최대 열(column) 수                             */

#define  KEY_PORT_ROW           0x0312      /* 키보드 매트릭스의 행에 해당하는 포트의 어드레스         */
#define  KEY_PORT_COL           0x0311      /* 키보드 매트릭스의 열에 해당하는 포트의 어드레스         */
#define  KEY_PORT_CW            0x0313      /* 키보드 I/O포트 제어용 워드의 어드레스                   */

#define  KEY_RPT_DLY                20      /* 자동 반복기능이 다시 수행되기 전의 스캔타임 값          */
#define  KEY_RPT_START_DLY         100      /* 자동 반복기능이 수행되기까지의 스캔타임 값              */

#define  KEY_SCAN_TASK_DLY          50      /* 키를 스캔하는 시간 간격                                 */
#define  KEY_SCAN_TASK_PRIO         50      /* 키보드 스캐닝 태스크의 우선순위                         */
#define  KEY_SCAN_TASK_STK_SIZE   1024      /* 키보드 스캐닝 태스크의 스택 크기                        */

#define  KEY_SHIFT1_MSK           0x80      /* SHIFT1 키는 열입력 포트의 비트7에 위치함                */
                                            /*     (0x00은 SHIFT1 키가 없음을 말한다)                  */
#define  KEY_SHIFT1_OFFSET          24      /* SHIFT1 키가 눌려졌을 때 더해질 오프셋 값                */

#define  KEY_SHIFT2_MSK           0x40      /* SHIFT2 키는 열입력 포트의 비트6에 위치함                */
                                            /*     (0x00은 SHIFT2 키가 없음을 말한다)                  */
#define  KEY_SHIFT2_OFFSET          48      /* SHIFT2 키가 눌려졌을 때 더해질 오프셋 값                */

#define  KEY_SHIFT3_MSK           0x00      /* SHIFT3 키는 열입력 포트의 비트5에 위치함                */
                                            /*     (0x00은 SHIFT3키가 없음을 말한다)                   */
#define  KEY_SHIFT3_OFFSET           0      /* SHIFT3 키가 눌려졌을 때 더해질 오프셋 값                */

#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                             멀티플렉싱 방식 LED 표시장치 드라이버 설정 상수
*                                                  (4장)
*********************************************************************************************************
*/

#if      MODULE_LED

#define  DISP_PORT_SEG          0x0300      /* SEGMENTS 출력용 포트 어드레스                           */
#define  DISP_PORT_DIG          0x0301      /* DIGITS 출력용 포트 어드레스                             */

#define  DISP_N_DIG                  8      /* 상태 표시기를 포함한 총 숫자의 갯수                     */
#define  DISP_N_SS                   7      /* 7-세그먼트의 총 갯수                                    */

#endif

/*
*********************************************************************************************************
*                                    LCD 표시모듈 드라이버 설정 상수
*                                                (5장)
*********************************************************************************************************
*/

#if      MODULE_LCD

#define  DISP_DLY_CNTS             100      /* 40uS 딜레이에 사용될 반복 횟수(소프트웨어 루프)         */

#define  DISP_PORT_DATA         0x0300      /* LCD 모듈의 데이터 포트 주소                             */
#define  DISP_PORT_CMD          0x0303      /* RS와 E를 제어하는 컨트롤 워드의 주소                    */

#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                       시계/달력 모듈 설정 상수
*                                                (6장)
*********************************************************************************************************
*/

#if      MODULE_CLK

#define  CLK_TASK_PRIO              51      /* ClkTask()의 우선순위                                    */
#define  CLK_DLY_TICKS     OS_TICKS_PER_SEC /* 1초를 지연하기 위한 클럭 틱의 수                        */
#define  CLK_TASK_STK_SIZE         512      /* ClkTask()의 스택 크기                                   */

#define  CLK_DATE_EN                 1      /* DATE 활성화(1일 때)                                     */
#define  CLK_TS_EN                   1      /* 타임스탬프 활성화(1일 때)                               */
#define  CLK_USE_DLY                 1      /* 세마포어를 기다리는 대신 OSTimeDly()를 사용한다         */

#endif

/*
*********************************************************************************************************
*                                            타이머 관리자
*                                                (7장)
*********************************************************************************************************
*/

#if      MODULE_TMR

#define  TMR_TASK_PRIO              52
#define  TMR_DLY_TICKS            (OS_TICKS_PER_SEC / 10)
#define  TMR_TASK_STK_SIZE         512

#define  TMR_MAX_TMR                20

#define  TMR_USE_SEM                 0

#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                      디지털 I/O 모듈 설정 상수
*                                                (8장)
*********************************************************************************************************
*/

#if      MODULE_DIO

#define  DIO_TASK_PRIO              53
#define  DIO_TASK_DLY_TICKS        (OS_TICKS_PER_SEC / 10)
#define  DIO_TASK_STK_SIZE         512

#define  DIO_MAX_DI                  8      /* 디지털입력 채널의 최대 수 (1..255)                      */
#define  DIO_MAX_DO                  8      /* 디지털출력 채널의 최대 수 (1..255)                      */

#define  DI_EDGE_EN                  1      /* 에지 트리거 지원을 위한 코드 생성 (1일 때)              */

#define  DO_BLINK_MODE_EN            1      /* 점멸 모드 지원을 위한 코드 생성 (1일 때)                */

#endif

/*
*********************************************************************************************************
*                                        아날로그 I/O 모듈 설정 상수
*                                                  (10장)
*********************************************************************************************************
*/

#if      MODULE_AIO

#define  AIO_TASK_PRIO              54
#define  AIO_TASK_DLY              100      /* 밀리 초 단위의 태스크 실행 주기                         */
#define  AIO_TASK_STK_SIZE         512

#define  AIO_MAX_AI                  8      /* 아날로그 입력 채널의 최대 수 (1..250)                   */
#define  AIO_MAX_AO                  8      /* 아날로그 출력 채널의 최대 수 (1..250)                   */

#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                  비동기 시리얼 통신 모듈 설정 상수
*                                                (11장)
*********************************************************************************************************
*/

#if      MODULE_COMM_PC

#define  COMM1_BASE        0x03F8           /* PC COM1의 베이스 어드레스                               */
#define  COMM2_BASE        0x02F8           /* PC COM2의 베이스 어드레스                               */

#define  COMM_MAX_RX            2           /* NS16450 UART의 최대 수신 입력 버퍼 ...                  */
                                            /* .... 16450:2, 16550:16                                  */
#endif


#if      MODULE_COMM_BGND

#define  COMM1                  1
#define  COMM2                  2

#define  COMM_RX_BUF_SIZE      64           /* Rx 원형버퍼의 사이즈                                    */
#define  COMM_TX_BUF_SIZE      64           /* Tx 원형버퍼의 사이즈                                    */

#endif


#if      MODULE_COMM_RTOS

#define  COMM1                  1
#define  COMM2                  2

#define  COMM_RX_BUF_SIZE      64           /* Rx 원형버퍼의 사이즈                                    */
#define  COMM_TX_BUF_SIZE      64           /* Tx 원형버퍼의 사이즈                                    */

#endif
