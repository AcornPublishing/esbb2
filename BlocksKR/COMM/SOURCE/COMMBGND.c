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
* Filename   : COMMBGND.C
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*
* ��Ʈ       : OS_ENTER_CRITICAL() �� OS_EXIT_CRITICAL() ��ũ�δ� ���� ���ͷ�Ʈ�� ��Ȱ��ȭ, Ȱ��ȭ
*              �ϴ� �� ���ȴ�. ���� Borland C++ compiler V3.1�� ����Ѵٸ� �� ���� ��ũ�θ�
*              ������ ���� �����ؾ� �Ѵ�
*
*                   #define OS_ENTER_CRITICAL()  disable()
*                   #define OS_EXIT_CRITICAL()   enable()
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               ��ũ���
*********************************************************************************************************
*/

#include "includes.h"

/*$PAGE*/
/*
*********************************************************************************************************
*                                                 ���
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             ������ Ÿ��
*********************************************************************************************************
*/

typedef struct {
    INT16U  RingBufRxCtr;                   /* Rx �� ���ۿ� ����� ������ ��                           */
    INT8U  *RingBufRxInPtr;                 /* ���� �����͸� ������ ���� ����Ű�� ������               */
    INT8U  *RingBufRxOutPtr;                /* ������ �о �����͸� ����Ű�� ������                  */
    INT8U   RingBufRx[COMM_RX_BUF_SIZE];    /* ���ſ� �� ���� ���念��(Rx)                             */
    INT16U  RingBufTxCtr;                   /* Tx �� ���ۿ� ����� ������ ��                           */
    INT8U  *RingBufTxInPtr;                 /* ���� �����͸� ������ ���� ����Ű�� ������               */
    INT8U  *RingBufTxOutPtr;                /* ������ �о �����͸� ����Ű�� ������                  */
    INT8U   RingBufTx[COMM_TX_BUF_SIZE];    /* �۽ſ� �� ���� ���念��(Tx)                             */
} COMM_RING_BUF;

/*
*********************************************************************************************************
*                                              ���� ����
*********************************************************************************************************
*/

COMM_RING_BUF  Comm1Buf;
COMM_RING_BUF  Comm2Buf;

/*$PAGE*/
/*
*********************************************************************************************************
*                                          �� ���� ������ �б�
*
*
* ����        : �������α׷��� ��� ä�ηκ��� �����͸� �о�� ���� �� �Լ��� ȣ���Ѵ�.
*
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
*                           COMM1
*                           COMM2
*               'err'   �����ڵ尡 ����� ������ ����Ű�� ������. ������ ���� ���� �����ȴ�:
*                           COMM_NO_ERR : ���ۿ��� �����͸� �о� �� ���
*                           COMM_RX_EMPTY : ���۰� ��� �ִ� ���
*                           COMM_BAD_CH : ��ȿ ä���� ������ ���
* ���ϰ�      : ���ۿ� ���� ���� ����� �����͸� �����Ѵ�. ���ۿ� �����Ͱ� ������ NUL�� �����Ѵ�
*********************************************************************************************************
*/

INT8U  CommGetChar (INT8U ch, INT8U *err)
{
    INT8U          c;
    COMM_RING_BUF *pbuf;


    switch (ch) {                                          /* ��� ä�ο� ���� �����͸� ��´�         */
        case COMM1:
             pbuf = &Comm1Buf;
             break;

        case COMM2:
             pbuf = &Comm2Buf;
             break;

        default:
             *err = COMM_BAD_CH;
             return (NUL);
    }
    OS_ENTER_CRITICAL();
    if (pbuf->RingBufRxCtr > 0) {                          /* ���۰� ����°�?                         */
        pbuf->RingBufRxCtr--;                              /* �ƴϿ�, ������ ī���͸� ���ҽ�Ų��       */
        c = *pbuf->RingBufRxOutPtr++;                      /* ���۷κ��� �����͸� ��´�               */
        if (pbuf->RingBufRxOutPtr == &pbuf->RingBufRx[COMM_RX_BUF_SIZE]) {     /* ������ ��� ó��     */
            pbuf->RingBufRxOutPtr = &pbuf->RingBufRx[0];
        }
        OS_EXIT_CRITICAL();
        *err = COMM_NO_ERR;
        return (c);
    } else {
        OS_EXIT_CRITICAL();
        *err = COMM_RX_EMPTY;
        c    = NUL;                                        /* ���۰� �����. NUL ����                  */
        return (c);
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       �۽� �� ���� ������ �б�
*
*
* ����        : �۽� ISR�� ���۷κ��� ���� �۽� �����͸� �о� ���� ���ؼ� �� �Լ��� ȣ���Ѵ�.
*               �� �Լ��� ���ۿ��� �����͸� �о �� ���۰� ��ٸ� FALSE�� �����Ѵ�.
*               ���� �۽� ISR�� �� �Լ��� FALSE�� �����ϸ� ���۰� ����ִ� ���� �ǹ��ϹǷ�
*               �۽� ���ͷ�Ʈ�� ��Ȱ��ȭ �Ѵ�.
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
*                           COMM1
*                           COMM2
*               'err'   �����ڵ尡 ����� ������ ����Ű�� ������:
*                           COMM_NO_ERR : �ϳ� �̻��� �����Ͱ� ���ۿ� �����ϴ� ���
*                           COMM_TX_EMPTY : ���۰� ��� �ִ� ���
*                           COMM_BAD_CH : ��ȿ ä���� ������ ���
* ���ϰ�      : �۽Ź��ۿ��� �о ������. ���۰� �� ���� NUL�� �����Ѵ�.
*********************************************************************************************************
*/

INT8U  CommGetTxChar (INT8U ch, INT8U *err)
{
    INT8U          c;
    COMM_RING_BUF *pbuf;


    switch (ch) {                                          /* ��� ä�ο� ���� �����͸� ��´�         */
        case COMM1:
             pbuf = &Comm1Buf;
             break;

        case COMM2:
             pbuf = &Comm2Buf;
             break;

        default:
             *err = COMM_BAD_CH;
             return (NUL);
    }
    if (pbuf->RingBufTxCtr > 0) {                          /* ���۰� ����°�?                         */
        pbuf->RingBufTxCtr--;                              /* �ƴϿ�, ������ ī���͸� ���ҽ�Ų��       */
        c = *pbuf->RingBufTxOutPtr++;                      /* ���۷κ��� �����͸� ��´�               */
        if (pbuf->RingBufTxOutPtr == &pbuf->RingBufTx[COMM_TX_BUF_SIZE]) {     /* ������ ��� ó��     */
            pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];
        }
        *err = COMM_NO_ERR;
        return (c);                                        /* �����Ͱ� ��ȿ��                          */
    } else {
        *err = COMM_TX_EMPTY;
        return (NUL);                                      /* ���۰� �����                            */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           ��� ��� �ʱ�ȭ
*
*
* ����        : �������α׷��� ��Ÿ���� �ʱ�ȭ �ϱ� ���� �� �Լ��� ȣ���Ѵ�.
*               �ٸ� �Լ��� ȣ���ϱ� ���� ���ʷ� �� �Լ��� ȣ���ؾ� �Ѵ�.
* ��������    : ����.
*********************************************************************************************************
*/

void  CommInit (void)
{
    COMM_RING_BUF *pbuf;


    pbuf                  = &Comm1Buf;                     /* COMM1�� �� ���۸� �ʱ�ȭ �Ѵ�            */
    pbuf->RingBufRxCtr    = 0;
    pbuf->RingBufRxInPtr  = &pbuf->RingBufRx[0];
    pbuf->RingBufRxOutPtr = &pbuf->RingBufRx[0];
    pbuf->RingBufTxCtr    = 0;
    pbuf->RingBufTxInPtr  = &pbuf->RingBufTx[0];
    pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];

    pbuf                  = &Comm2Buf;                     /* COMM2�� �� ���۸� �ʱ�ȭ �Ѵ�            */
    pbuf->RingBufRxCtr    = 0;
    pbuf->RingBufRxInPtr  = &pbuf->RingBufRx[0];
    pbuf->RingBufRxOutPtr = &pbuf->RingBufRx[0];
    pbuf->RingBufTxCtr    = 0;
    pbuf->RingBufTxInPtr  = &pbuf->RingBufTx[0];
    pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                      ���� ���۰� ������� Ȯ��
*
*
* ����        : �������α׷��� �ش� ���ä���� ���Ź��ۿ� �����Ͱ� �ִ� �� Ȯ���ϱ� ���� �� �Լ���
*               ȣ���Ѵ�. �ϳ� �̻��� �����Ͱ� ��ȿ�� ��� FALSE�� �����ϰ�, �׷��� �ʴ� ���
*               TRUE�� �����Ѵ�.
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
*                           COMM1
*                           COMM2
* ���ϰ�      : TRUE    ���۰� �� ���.
*               FALSE   ���ۿ� �����Ͱ� ��ȿ�ϰų� ��ȿ�� ä���� ������ ���.
*********************************************************************************************************
*/

BOOLEAN  CommIsEmpty (INT8U ch)
{
    BOOLEAN        empty;
    COMM_RING_BUF *pbuf;


    switch (ch) {                                          /* ��� ä�ο� ���� �����͸� ��´�         */
        case COMM1:
             pbuf = &Comm1Buf;
             break;

        case COMM2:
             pbuf = &Comm2Buf;
             break;

        default:
             return (TRUE);
    }
    OS_ENTER_CRITICAL();
    if (pbuf->RingBufRxCtr > 0) {                          /* ���۰� ����°�?                         */
        empty = FALSE;                                     /* ���ۿ� �����Ͱ� ��ȿ��                   */
    } else {
        empty = TRUE;                                      /* ���۰� �����                            */
    }
    OS_EXIT_CRITICAL();
    return (empty);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       �۽� ���۰� á���� Ȯ��
*
*
* ����        : �������α׷��� �۽� ���ۿ� �����͸� �� ������ �� �ִ� �� Ȯ���ϱ� ���� �� �Լ���
*               ȣ���Ѵ�. ��, �� �Լ��� �۽� ���۰� á���� Ȯ���Ѵ�.
*               ���۰� �� ���, �� �Լ��� TRUE�� �����ϰ�, �׷��� ���� ��� FALSE�� �����Ѵ�.
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
*                           COMM1
*                           COMM2
* ���ϰ�      : TRUE    ���۰� �� ���.
*               FALSE   ���۰� ���� �ʾҰų� ��ȿ�� ä���� ������ ���.
*********************************************************************************************************
*/

BOOLEAN  CommIsFull (INT8U ch)
{
    BOOLEAN        full;
    COMM_RING_BUF *pbuf;


    switch (ch) {                                          /* ��� ä�ο� ���� �����͸� ��´�         */
        case COMM1:
             pbuf = &Comm1Buf;
             break;

        case COMM2:
             pbuf = &Comm2Buf;
             break;

        default:
             return (TRUE);
    }
    OS_ENTER_CRITICAL();
    if (pbuf->RingBufTxCtr < COMM_TX_BUF_SIZE) {           /* ���۰� á�°�?                           */
        full = FALSE;                                      /* ���۰� ���� �ʾ���                       */
    } else {
        full = TRUE;                                       /* ���۰� á��                              */
    }
    OS_EXIT_CRITICAL();
    return (full);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             ������ ���
*
*
* ����        : �������α׷��� ���ä�η� �� ����Ʈ�� �����͸� ������ ���� �� �Լ��� ȣ���Ѵ�.
*               ������ �����ʹ� �۽� ���ۿ� ����ǰ� �۽� ISR�� ���� �ø��� ��Ʈ�� ���۵ȴ�.
*               ���� �����Ͱ� ������ ù ��° �����Ͷ�� �۽� ISR�� Ȱ��ȭ �ȴ�.
*               �۽� ���۰� á�ٸ�, �����ʹ� ���۵��� �ʰ� ���õȴ�.
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
*                           COMM1
*                           COMM2
*               'c'     �۽��ϰ��� �ϴ� ������.
* ���ϰ�      : COMM_NO_ERR   ���������� �Լ��� �Ϸ�� ���(���۰� ���� �ʾ���)
*               COMM_TX_FULL  ���۰� �� ���
*               COMM_BAD_CH   ��ȿ ä���� ������ ���
*********************************************************************************************************
*/

INT8U  CommPutChar (INT8U ch, INT8U c)
{
    COMM_RING_BUF *pbuf;


    switch (ch) {                                          /* ��� ä�ο� ���� �����͸� ��´�         */
        case COMM1:
             pbuf = &Comm1Buf;
             break;

        case COMM2:
             pbuf = &Comm2Buf;
             break;

        default:
             return (COMM_BAD_CH);
    }
    OS_ENTER_CRITICAL();
    if (pbuf->RingBufTxCtr < COMM_TX_BUF_SIZE) {           /* ���۰� á�°�?                           */
        pbuf->RingBufTxCtr++;                              /* �ƴϿ�, ������ ī��Ʈ ����               */
        *pbuf->RingBufTxInPtr++ = c;                       /* �����͸� ���ۿ� �����Ѵ�                 */
        if (pbuf->RingBufTxInPtr == &pbuf->RingBufTx[COMM_TX_BUF_SIZE]) { /* ������ ��� ó��          */
            pbuf->RingBufTxInPtr = &pbuf->RingBufTx[0];
        }
        if (pbuf->RingBufTxCtr == 1) {                     /* ù ��° �������ΰ�?                      */
            CommTxIntEn(ch);                               /* ��, �۽� ���ͷ�Ʈ Ȱ��ȭ                 */
            OS_EXIT_CRITICAL();
        } else {
            OS_EXIT_CRITICAL();
        }
        return (COMM_NO_ERR);
    } else {
        OS_EXIT_CRITICAL();
        return (COMM_TX_FULL);
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       �� ���ۿ� �����͸� ����
*
*
* ����        : ���� ISR�� �����͸� �� ���ۿ� �����ϱ� ���� �� �Լ��� ȣ���Ѵ�.
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
*                           COMM1
*                           COMM2
*               'c'     ���� �� ���ۿ� �����ϰ��� �ϴ� ������.  ���۰� á�ٸ�, ���ŵ� �����ʹ�
*                       ������� �ʰ� ���õȴ�.
*********************************************************************************************************
*/

void  CommPutRxChar (INT8U ch, INT8U c)
{
    COMM_RING_BUF *pbuf;


    switch (ch) {                                          /* ��� ä�ο� ���� �����͸� ��´�         */
        case COMM1:
             pbuf = &Comm1Buf;
             break;

        case COMM2:
             pbuf = &Comm2Buf;
             break;

        default:
             return;
    }
    if (pbuf->RingBufRxCtr < COMM_RX_BUF_SIZE) {           /* ���۰� á�°�?                           */
        pbuf->RingBufRxCtr++;                              /* �ƴϿ�, ������ ī���͸� �߰���Ų��       */
        *pbuf->RingBufRxInPtr++ = c;                       /* �����͸� ���ۿ� �����Ѵ�                 */
        if (pbuf->RingBufRxInPtr == &pbuf->RingBufRx[COMM_RX_BUF_SIZE]) { /* ������ ��� ó��          */
            pbuf->RingBufRxInPtr = &pbuf->RingBufRx[0];
        }
    }
}