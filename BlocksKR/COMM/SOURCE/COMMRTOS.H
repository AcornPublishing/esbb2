/*
*********************************************************************************************************
*                                     Embedded Systems Building Blocks
*                                  Complete and Ready-to-Use Modules in C
*
*                                    Asynchronous Serial Communications
*                                           Buffered Serial I/O 
*                                                 (RTOS)
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* Filename   : COMMRTOS.H
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              설정상수
*********************************************************************************************************
*/

#ifndef  CFG_H

#define  COMM_RX_BUF_SIZE      64                /* Number of characters in Rx ring buffer             */
#define  COMM_TX_BUF_SIZE      64                /* Number of characters in Tx ring buffer             */

#endif

/*
*********************************************************************************************************
*                                                상수
*********************************************************************************************************
*/

#ifndef  NUL
#define  NUL                 0x00
#endif

#define  COMM1                  1
#define  COMM2                  2

                                                 /* 에러코드                                           */
#define  COMM_NO_ERR            0                /* 함수 호출이 성공적으로 수행되었음                  */
#define  COMM_BAD_CH            1                /* 무효한 채널을 지정했음                             */
#define  COMM_RX_EMPTY          2                /* 수신 버퍼가 비었음. 데이터가 없음                  */
#define  COMM_TX_FULL           3                /* 송신 버퍼가 완전히 찼음. 데이터를 저장할 수 없음   */
#define  COMM_TX_EMPTY          4                /* 송신 버퍼가 비었음                                 */
#define  COMM_RX_TIMEOUT        5                /* 데이터 수신 대기 중 타임아웃 만료 발생             */
#define  COMM_TX_TIMEOUT        6                /* 데이터 송신 대기 중 타임아웃 만료 발생             */


#define  COMM_PARITY_NONE       0                /* 패리티 지정 상수                                   */
#define  COMM_PARITY_ODD        1
#define  COMM_PARITY_EVEN       2


#ifdef   COMM_GLOBALS
#define  COMM_EXT
#else
#define  COMM_EXT  extern
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                              함수원형
*********************************************************************************************************
*/

INT8U    CommGetChar(INT8U ch, INT16U to, INT8U *err);
INT8U    CommGetTxChar(INT8U ch, INT8U *err);
void     CommInit(void);
BOOLEAN  CommIsEmpty(INT8U ch);
BOOLEAN  CommIsFull(INT8U ch);
INT8U    CommPutChar(INT8U ch, INT8U c, INT16U to);
void     CommPutRxChar(INT8U ch, INT8U c);
