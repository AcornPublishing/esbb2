/*
*********************************************************************************************************
*                                     Embedded Systems Building Blocks
*                                  Complete and Ready-to-Use Modules in C
*
*                                    Asynchronous Serial Communications
*                                          Buffered Serial I/O
*                                     (Foreground/Background Systems)
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* Filename   : COMMBGND.H
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

#define  COMM_RX_BUF_SIZE     128                /* ���ſ� �� ������ ������ ����                       */
#define  COMM_TX_BUF_SIZE     128                /* �۽ſ� �� �۹��� ������ ����                       */

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

INT8U    CommGetChar(INT8U ch, INT8U *err);
INT8U    CommGetTxChar(INT8U ch, INT8U *err);
void     CommInit(void);
BOOLEAN  CommIsEmpty(INT8U ch);
BOOLEAN  CommIsFull(INT8U ch);
INT8U    CommPutChar(INT8U ch, INT8U c);
void     CommPutRxChar(INT8U ch, INT8U c);