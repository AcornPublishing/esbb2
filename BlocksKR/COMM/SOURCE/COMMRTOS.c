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
* Filename   : COMMRTOS.C
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
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
*                                              ������ Ÿ��
*********************************************************************************************************
*/

typedef struct {
    INT16U     RingBufRxCtr;                /* Rx �� ���ۿ� ����� ������ ��                           */
    OS_EVENT  *RingBufRxSem;                /* ���� ���������� ������                                  */
    INT8U     *RingBufRxInPtr;              /* ���� �����͸� ������ ���� ����Ű�� ������               */ 
    INT8U     *RingBufRxOutPtr;             /* ������ �о �����͸� ����Ű�� ������                  */ 
    INT8U      RingBufRx[COMM_RX_BUF_SIZE]; /* ���ſ� �� ���� ���念��(Rx)                             */ 
    INT16U     RingBufTxCtr;                /* Tx �� ���ۿ� ����� ������ ��                           */
    OS_EVENT  *RingBufTxSem;                /* �۽� ���������� ������                                  */
    INT8U     *RingBufTxInPtr;              /* ���� �����͸� ������ ���� ����Ű�� ������               */
    INT8U     *RingBufTxOutPtr;             /* ������ �о �����͸� ����Ű�� ������                  */
    INT8U      RingBufTx[COMM_TX_BUF_SIZE]; /* �۽ſ� �� ���� ���念��(Tx)                             */
} COMM_RING_BUF;

/*
*********************************************************************************************************
*                                              ��������
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
*               �� �Լ��� ���ä�ο� �����Ͱ� ���ŵǰų� Ÿ�Ӿƿ��� ����� �� ���� ����Ѵ�.
*
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
*                           COMM1
*                           COMM2
*               'to'    �����Ͱ� ���ŵǱ⸦ ��ٸ��� Ÿ�Ӿƿ��� Ŭ�� ƽ ������ �����Ѵ�.
*                       �� ���� 0���� �����ϸ� �Լ��� �����Ͱ� ���ŵ� �� ���� ������ ����Ѵ�.
*
*               'err'   �����ڵ尡 ����� ������ ����Ű�� ������. ������ ���� ���� �����ȴ�:
*                           COMM_NO_ERR : ���ۿ��� �����͸� �о� �� ���
*                           COMM_RX_TIMEOUT : Ÿ�Ӿƿ��� ����� ���
*                           COMM_BAD_CH : ��ȿ ä���� ������ ���
* ���ϰ�      : ���ۿ��� �о ������(Ÿ�Ӿƿ��� ����� ���� NUL�� �����Ѵ�)
*********************************************************************************************************
*/

INT8U  CommGetChar (INT8U ch, INT16U to, INT8U *err)
{
    INT8U          c;
    INT8U          oserr;
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
    OSSemPend(pbuf->RingBufRxSem, to, &oserr);             /* �����Ͱ� ���ŵǱ⸦ ��ٸ���             */
    if (oserr == OS_TIMEOUT) {                             /* Ÿ�Ӿƿ��� ����Ǿ��°�?                 */
        *err = COMM_RX_TIMEOUT;                            /* ��, �����ڵ带 �����Ѵ�                  */
        return (NUL);
    } else {
        OS_ENTER_CRITICAL();
        pbuf->RingBufRxCtr--;                              /* �ƴϿ�, ������ ī���͸� ���ҽ�Ų��       */
        c = *pbuf->RingBufRxOutPtr++;                      /* ���ۿ��� �����͸� �о��               */
        if (pbuf->RingBufRxOutPtr == &pbuf->RingBufRx[COMM_RX_BUF_SIZE]) {     /* ������ ��� ó��     */
            pbuf->RingBufRxOutPtr = &pbuf->RingBufRx[0];
        }
        OS_EXIT_CRITICAL();
        *err = COMM_NO_ERR;
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
    if (pbuf->RingBufTxCtr > 0) {                          /* ���ۿ� �����Ͱ� �ִ°�?                  */
        pbuf->RingBufTxCtr--;                              /* ��, ������ ī���͸� ���ҽ�Ų��           */
        c = *pbuf->RingBufTxOutPtr++;                      /* ���ۿ��� �����͸� �д´�                 */
        if (pbuf->RingBufTxOutPtr == &pbuf->RingBufTx[COMM_TX_BUF_SIZE]) {     /* ������ ��� ó��     */
            pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];
        }
        OSSemPost(pbuf->RingBufTxSem);                     /* ���ۿ� �Ѱ��� ���念���� �������� �˸��� */
        *err = COMM_NO_ERR;
        return (c);                                        /* ���������� �����͸� �о���               */
    } else {
        *err = COMM_TX_EMPTY;
        return (NUL);                                      /* ���۰� �����                            */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                          ��Ÿ�� �ʱ�ȭ
*
*
* ����        : �������α׷��� ��Ÿ���� �ʱ�ȭ �ϱ� ���� �� �Լ��� ȣ���Ѵ�.
*               ��Ÿ���� �ٸ� �Լ��� ȣ���ϱ� ���� �ݵ�� �� �Լ��� ���� ȣ���ؾ� �Ѵ�.
* ��������    : ����
*********************************************************************************************************
*/

void  CommInit (void)
{
    COMM_RING_BUF *pbuf;


    pbuf                  = &Comm1Buf;                     /* COMM1�� ���� �� ���� �ʱ�ȭ              */
    pbuf->RingBufRxCtr    = 0;
    pbuf->RingBufRxInPtr  = &pbuf->RingBufRx[0];
    pbuf->RingBufRxOutPtr = &pbuf->RingBufRx[0];
    pbuf->RingBufRxSem    = OSSemCreate(0);
    pbuf->RingBufTxCtr    = 0;
    pbuf->RingBufTxInPtr  = &pbuf->RingBufTx[0];
    pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];
    pbuf->RingBufTxSem    = OSSemCreate(COMM_TX_BUF_SIZE);

    pbuf                  = &Comm2Buf;                     /* COMM2�� ���� �� ���� �ʱ�ȭ              */
    pbuf->RingBufRxCtr    = 0;
    pbuf->RingBufRxInPtr  = &pbuf->RingBufRx[0];
    pbuf->RingBufRxOutPtr = &pbuf->RingBufRx[0];
    pbuf->RingBufRxSem    = OSSemCreate(0);
    pbuf->RingBufTxCtr    = 0;
    pbuf->RingBufTxInPtr  = &pbuf->RingBufTx[0];
    pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];
    pbuf->RingBufTxSem    = OSSemCreate(COMM_TX_BUF_SIZE);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       ���� ���۰� ������� Ȯ��
*
*
* ����        : �������α׷��� ��� ä�ο� �����Ͱ� ��ȿ���� Ȯ���ϱ� ���� �� �Լ��� ȣ���Ѵ�.
*               �� ����Ʈ �̻��� �����Ͱ� ��ȿ�ϸ� FALSE�� �����ϰ� �׷��� ���� ��� TRUE�� �����Ѵ�.
*
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
*                           COMM1
*                           COMM2
* ���ϰ�      : TRUE    ���۰� �� ���
*               FALSE   ���۰� ���� �ʴ� ��� �Ǵ� ��ȿ ä���� ������ ���
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
    if (pbuf->RingBufRxCtr > 0) {                          /* ���ۿ� �����Ͱ� �ִ°�?                  */
        empty = FALSE;                                     /* ���۰� ���� ����                         */
    } else {
        empty = TRUE;                                      /* ���۰� �����                            */
    }
    OS_EXIT_CRITICAL();
    return (empty);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                    �۽Ź��۰� ������ á���� Ȯ��
*
*
* ����        : �������α׷��� �۽Ź��ۿ� �����͸� �� ������ �� �ִ��� Ȯ���ϱ� ���� �� �Լ��� ȣ���Ѵ�.
*               ��, �� �Լ��� �۽Ź��۰� ������ á���� Ȯ���Ѵ�.
*               ���۰� �� ��� �Լ��� TRUE�� �����ϰ� �׷��� ���� ���� FALSE�� �����Ѵ�.
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
*                           COMM1
*                           COMM2
* ���ϰ�      : TRUE    ���۰� ������ á��.
*               FALSE   ���۰� ���� �ʾҰų� ��ȿä���� ������ ���.
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
    if (pbuf->RingBufTxCtr < COMM_TX_BUF_SIZE) {           /* ���۰� á���� Ȯ��                       */
        full = FALSE;                                      /* ���۰� ���� ����                         */
    } else {
        full = TRUE;                                       /* ���۰� ������ á��                       */
    }
    OS_EXIT_CRITICAL();
    return (full);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                              ������ ���
*
*
* ����        : �� �Լ��� ���ä�η� �����͸� ������ ���� ȣ��ȴ�. ���۰� ������ �� ���� ���۰� ��
*               ������ ��ٸ���. ������ Ÿ�Ӿƿ��� ����� ������ ���۰� ���� ������ ȣ���Լ��� �����Ѵ�.
*               Ÿ�Ӿƿ��� 0���� �����ϸ� ���۰� �� ������ ������ ��ٸ���.
*               ���� ���� ���ۿ� ����Ǵ� �����Ͱ� �۽� ISR�� ���� ���۵ȴ�.
*               ���۰� ������ �� ���¿��� ù ��° �����Ͱ� ���ۿ� ����� �� �۽� ISR�� Ȱ��ȭ �ȴ�.
*
*
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
*                           COMM1
*                           COMM2
*               'c'     ���ۿ� �����ϰ��� �ϴ� ������.
*               'to'    ���۰� �� ��� �� ������ ��ٸ����� �����ϴ� Ŭ�� ƽ ������ Ÿ�Ӿƿ� ��.
*                       Ÿ�Ӿƿ����� 0�� �����ϸ� ���۰� �� ������ ������ ��ٸ���.
* ���ϰ�      : COMM_NO_ERR      �����Ͱ� ���������� ���ۿ� ����� ���
*               COMM_TX_TIMEOUT  ������ Ÿ�Ӿƿ��� ����� �� ���� ���۰� ���� ���� ���
*               COMM_BAD_CH      ��ȿ�� ä�� ��ȣ�� ������ ���
*********************************************************************************************************
*/

INT8U  CommPutChar (INT8U ch, INT8U c, INT16U to)
{
    INT8U          oserr;
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
    OSSemPend(pbuf->RingBufTxSem, to, &oserr);             /* �۽Ź��ۿ� �� ������ ����� ��ٸ���     */
    if (oserr == OS_TIMEOUT) {
        return (COMM_TX_TIMEOUT);                          /* Ÿ�Ӿƿ� �����, �����ڵ� ����           */
    }
    OS_ENTER_CRITICAL();
    pbuf->RingBufTxCtr++;                                  /* ������ ī���͸� ������Ų��               */
    *pbuf->RingBufTxInPtr++ = c;                           /* ���ۿ� �����͸� �����Ѵ�                 */
    if (pbuf->RingBufTxInPtr == &pbuf->RingBufTx[COMM_TX_BUF_SIZE]) {     /* ������ ��� ó��          */
        pbuf->RingBufTxInPtr = &pbuf->RingBufTx[0];
    }
    if (pbuf->RingBufTxCtr == 1) {                         /* ù ��° �������ΰ�?                      */
        CommTxIntEn(ch);                                   /* ��, �۽� ���ͷ�Ʈ Ȱ��ȭ                 */
    }
    OS_EXIT_CRITICAL();
    return (COMM_NO_ERR);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        �� ���ۿ� ������ ����
*
*
* ����        : �� �Լ��� ���ſ� �� ���ۿ� �����͸� �����ϱ� ���� ISR�� ���� ȣ��ȴ�.
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
*                           COMM1
*                           COMM2
*               'c'     �� ���ۿ� �����Ϸ��� ������.  ���۰� �� �ִٸ� �����ʹ� ������� �ʰ� ���õȴ�.
*
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
    if (pbuf->RingBufRxCtr < COMM_RX_BUF_SIZE) {           /* ���ۿ� �� �ִ°�?                        */
        pbuf->RingBufRxCtr++;                              /* �ƴϿ�, ������ ī���� ����               */
        *pbuf->RingBufRxInPtr++ = c;                       /* ���ۿ� ����                              */
        if (pbuf->RingBufRxInPtr == &pbuf->RingBufRx[COMM_RX_BUF_SIZE]) { /* ������ ��� ó��          */
            pbuf->RingBufRxInPtr = &pbuf->RingBufRx[0];
        }
        OSSemPost(pbuf->RingBufRxSem);                     /* �����Ͱ� ���ŵ� ���� �˷���              */
    }
}
