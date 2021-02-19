/*
*********************************************************************************************************
*                                   Embedded Systems Building Blocks
*                                Complete and Ready-to-Use Modules in C
*
*                                         Matrix Keyboard Driver
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* Filename   : KEY.H
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*                                            사용자 정의 상수
*
* 노트: 이들 #define 상수들은 사용자 환경에 맞도록 변경되어야 한다
*********************************************************************************************************
*/

#ifndef  CFG_H
#define  KEY_BUF_SIZE               10      /* 키보드 버퍼의 크기                                      */

#define  KEY_PORT_ROW           0x0312      /* 키보드 매트릭스의 행에 해당하는 포트의 어드레스         */
#define  KEY_PORT_COL           0x0311      /* 키보드 매트릭스의 열에 해당하는 포트의 어드레스         */
#define  KEY_PORT_CW            0x0313      /* 키보드 I/O포트 제어용 워드의 어드레스                   */

#define  KEY_MAX_ROWS                4      /* 키보드의 최대 행(row) 수                                */
#define  KEY_MAX_COLS                6      /* 키보드의 최대 열(column) 수                             */

#define  KEY_RPT_DLY                 2      /* 자동 반복기능이 다시 수행되기 전의 스캔타임 값          */
#define  KEY_RPT_START_DLY          10      /* 자동 반복기능이 수행되기까지의 스캔타임 값              */

#define  KEY_SCAN_TASK_DLY          50      /* 키를 스캔하는 시간 간격                                 */
#define  KEY_SCAN_TASK_PRIO         50      /* 키보드 스캐닝 태스크의 우선순위                         */
#define  KEY_SCAN_TASK_STK_SIZE   1024      /* 키보드 스캐닝 태스크의 스택 크기                        */

#define  KEY_SHIFT1_MSK           0x80      /* SHIFT1키는 열입력 포트의 비트7에 위치함                 */
                                            /*     (0x00은 SHIFT1키가 없음을 말한다)                   */
#define  KEY_SHIFT1_OFFSET          24      /* SHIFT1키가 눌려졌을 때 더해질 오프셋 값                 */

#define  KEY_SHIFT2_MSK           0x40      /* SHIFT2키는 열입력 포트의 비트6에 위치함                 */
                                            /*     (0x00은 SHIFT2키가 없음을 말한다)                   */
#define  KEY_SHIFT2_OFFSET          48      /* SHIFT2키가 눌려졌을 때 더해질 오프셋 값                 */

#define  KEY_SHIFT3_MSK           0x00      /* SHIFT3키는 열입력 포트의 비트5에 위치함                 */
                                            /*     (0x00은 SHIFT3키가 없음을 말한다)                   */
#define  KEY_SHIFT3_OFFSET           0      /* SHIFT3키가 눌려졌을 때 더해질 오프셋 값                 */
#endif

#define  KEY_ALL_ROWS             0xFF      /* 모든 행을 선택 (즉, 모든 행은 0으로 출력)               */

/*
*********************************************************************************************************
*                                                함수원형
*********************************************************************************************************
*/

void    KeyFlush(void);                /* 키보드 버퍼를 클리어 한다                                    */
INT8U   KeyGetKey(INT16U to);          /* 키보드 드라이버로부터 스캔코드를 읽어온다. 없으면 -1을 리턴  */
INT32U  KeyGetKeyDownTime(void);       /* 키가 눌려져 있는 시간을 리턴한다 (밀리초, milliseconds)      */
BOOLEAN KeyHit(void);                  /* 키가 눌려져 있는 지를 확인(눌렸으면 TRUE, 그렇지 않으면FALSE)*/
void    KeyInit(void);                 /* 키보드 드라이버를 초기화                                     */


void    KeyInitPort(void);             /* I/O포트를 초기화                                             */
INT8U   KeyGetCol(void);               /* 열을 읽는다                                                  */
void    KeySelRow(INT8U row);          /* 행을 선택한다                                                */
