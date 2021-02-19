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
*                                              �������
*********************************************************************************************************
*/

#ifndef  CFG_H

#define  COMM_RX_BUF_SIZE      64                /* Number of characters in Rx ring buffer             */
#define  COMM_TX_BUF_SIZE      64                /* Number of characters in Tx ring buffer             */

#endif

/*
*********************************************************************************************************
*                                                ���
*********************************************************************************************************
*/

#ifndef  NUL
#define  NUL                 0x00
#endif

#define  COMM1                  1
#define  COMM2                  2

                                                 /* �����ڵ�                                           */
#define  COMM_NO_ERR            0                /* �Լ� ȣ���� ���������� ����Ǿ���                  */
#define  COMM_BAD_CH            1                /* ��ȿ�� ä���� ��������                             */
#define  COMM_RX_EMPTY          2                /* ���� ���۰� �����. �����Ͱ� ����                  */
#define  COMM_TX_FULL           3                /* �۽� ���۰� ������ á��. �����͸� ������ �� ����   */
#define  COMM_TX_EMPTY          4                /* �۽� ���۰� �����                                 */
#define  COMM_RX_TIMEOUT        5                /* ������ ���� ��� �� Ÿ�Ӿƿ� ���� �߻�             */
#define  COMM_TX_TIMEOUT        6                /* ������ �۽� ��� �� Ÿ�Ӿƿ� ���� �߻�             */


#define  COMM_PARITY_NONE       0                /* �и�Ƽ ���� ���                                   */
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
*                                              �Լ�����
*********************************************************************************************************
*/

INT8U    CommGetChar(INT8U ch, INT16U to, INT8U *err);
INT8U    CommGetTxChar(INT8U ch, INT8U *err);
void     CommInit(void);
BOOLEAN  CommIsEmpty(INT8U ch);
BOOLEAN  CommIsFull(INT8U ch);
INT8U    CommPutChar(INT8U ch, INT8U c, INT16U to);
void     CommPutRxChar(INT8U ch, INT8U c);