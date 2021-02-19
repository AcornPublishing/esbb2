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
* 노트       : OS_ENTER_CRITICAL() 과 OS_EXIT_CRITICAL() 매크로는 각각 인터럽트를 비활성화, 활성화
*              하는 데 사용된다. 만약 Borland C++ compiler V3.1을 사용한다면 두 개의 매크로를
*              다음과 같이 정의해야 한다
*
*                   #define OS_ENTER_CRITICAL()  disable()
*                   #define OS_EXIT_CRITICAL()   enable()
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               인크루드
*********************************************************************************************************
*/

#include "includes.h"

/*$PAGE*/
/*
*********************************************************************************************************
*                                                 상수
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             데이터 타입
*********************************************************************************************************
*/

typedef struct {
    INT16U  RingBufRxCtr;                   /* Rx 링 버퍼에 저장된 데이터 수                           */
    INT8U  *RingBufRxInPtr;                 /* 다음 데이터를 저장할 곳을 가리키는 포인터               */
    INT8U  *RingBufRxOutPtr;                /* 다음에 읽어갈 데이터를 가리키는 포인터                  */
    INT8U   RingBufRx[COMM_RX_BUF_SIZE];    /* 수신용 링 버퍼 저장영역(Rx)                             */
    INT16U  RingBufTxCtr;                   /* Tx 링 버퍼에 저장된 데이터 수                           */
    INT8U  *RingBufTxInPtr;                 /* 다음 데이터를 저장할 곳을 가리키는 포인터               */
    INT8U  *RingBufTxOutPtr;                /* 다음에 읽어갈 데이터를 가리키는 포인터                  */
    INT8U   RingBufTx[COMM_TX_BUF_SIZE];    /* 송신용 링 버퍼 저장영역(Tx)                             */
} COMM_RING_BUF;

/*
*********************************************************************************************************
*                                              전역 변수
*********************************************************************************************************
*/

COMM_RING_BUF  Comm1Buf;
COMM_RING_BUF  Comm2Buf;

/*$PAGE*/
/*
*********************************************************************************************************
*                                          링 버퍼 데이터 읽기
*
*
* 설명        : 응용프로그램은 통신 채널로부터 데이터를 읽어내기 위해 이 함수를 호출한다.
*
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
*               'err'   에러코드가 저장될 변수를 가리키는 포인터. 다음과 같은 값이 설정된다:
*                           COMM_NO_ERR : 버퍼에서 데이터를 읽어 낸 경우
*                           COMM_RX_EMPTY : 버퍼가 비어 있는 경우
*                           COMM_BAD_CH : 무효 채널을 지정한 경우
* 리턴값      : 버퍼에 가장 먼저 저장된 데이터를 리턴한다. 버퍼에 데이터가 없으면 NUL을 리턴한다
*********************************************************************************************************
*/

INT8U  CommGetChar (INT8U ch, INT8U *err)
{
    INT8U          c;
    COMM_RING_BUF *pbuf;


    switch (ch) {                                          /* 통신 채널에 대한 포인터를 얻는다         */
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
    if (pbuf->RingBufRxCtr > 0) {                          /* 버퍼가 비었는가?                         */
        pbuf->RingBufRxCtr--;                              /* 아니오, 데이터 카운터를 감소시킨다       */
        c = *pbuf->RingBufRxOutPtr++;                      /* 버퍼로부터 데이터를 얻는다               */
        if (pbuf->RingBufRxOutPtr == &pbuf->RingBufRx[COMM_RX_BUF_SIZE]) {     /* 포인터 경계 처리     */
            pbuf->RingBufRxOutPtr = &pbuf->RingBufRx[0];
        }
        OS_EXIT_CRITICAL();
        *err = COMM_NO_ERR;
        return (c);
    } else {
        OS_EXIT_CRITICAL();
        *err = COMM_RX_EMPTY;
        c    = NUL;                                        /* 버퍼가 비었음. NUL 리턴                  */
        return (c);
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       송신 링 버퍼 데이터 읽기
*
*
* 설명        : 송신 ISR은 버퍼로부터 다음 송신 데이터를 읽어 내기 위해서 이 함수를 호출한다.
*               이 함수는 버퍼에서 데이터를 읽어낸 후 버퍼가 빈다면 FALSE를 리턴한다.
*               따라서 송신 ISR은 이 함수가 FALSE를 리턴하면 버퍼가 비어있는 것을 의미하므로
*               송신 인터럽트를 비활성화 한다.
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
*               'err'   에러코드가 저장될 변수를 가리키는 포인터:
*                           COMM_NO_ERR : 하나 이상의 데이터가 버퍼에 존재하는 경우
*                           COMM_TX_EMPTY : 버퍼가 비어 있는 경우
*                           COMM_BAD_CH : 무효 채널을 지정한 경우
* 리턴값      : 송신버퍼에서 읽어낸 데이터. 버퍼가 빈 경우는 NUL을 리턴한다.
*********************************************************************************************************
*/

INT8U  CommGetTxChar (INT8U ch, INT8U *err)
{
    INT8U          c;
    COMM_RING_BUF *pbuf;


    switch (ch) {                                          /* 통신 채널에 대한 포인터를 얻는다         */
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
    if (pbuf->RingBufTxCtr > 0) {                          /* 버퍼가 비었는가?                         */
        pbuf->RingBufTxCtr--;                              /* 아니오, 데이터 카운터를 감소시킨다       */
        c = *pbuf->RingBufTxOutPtr++;                      /* 버퍼로부터 데이터를 얻는다               */
        if (pbuf->RingBufTxOutPtr == &pbuf->RingBufTx[COMM_TX_BUF_SIZE]) {     /* 포인터 경계 처리     */
            pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];
        }
        *err = COMM_NO_ERR;
        return (c);                                        /* 데이터가 유효함                          */
    } else {
        *err = COMM_TX_EMPTY;
        return (NUL);                                      /* 버퍼가 비었음                            */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           통신 모듈 초기화
*
*
* 설명        : 응용프로그램은 통신모듈을 초기화 하기 위해 이 함수를 호출한다.
*               다른 함수를 호출하기 전에 최초로 이 함수를 호출해야 한다.
* 전달인자    : 없음.
*********************************************************************************************************
*/

void  CommInit (void)
{
    COMM_RING_BUF *pbuf;


    pbuf                  = &Comm1Buf;                     /* COMM1의 링 버퍼를 초기화 한다            */
    pbuf->RingBufRxCtr    = 0;
    pbuf->RingBufRxInPtr  = &pbuf->RingBufRx[0];
    pbuf->RingBufRxOutPtr = &pbuf->RingBufRx[0];
    pbuf->RingBufTxCtr    = 0;
    pbuf->RingBufTxInPtr  = &pbuf->RingBufTx[0];
    pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];

    pbuf                  = &Comm2Buf;                     /* COMM2의 링 버퍼를 초기화 한다            */
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
*                                      수신 버퍼가 비었는지 확인
*
*
* 설명        : 응용프로그램은 해당 통신채널의 수신버퍼에 데이터가 있는 지 확인하기 위해 이 함수를
*               호출한다. 하나 이상의 데이터가 유효한 경우 FALSE를 리턴하고, 그렇지 않는 경우
*               TRUE를 리턴한다.
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
* 리턴값      : TRUE    버퍼가 빈 경우.
*               FALSE   버퍼에 데이터가 유효하거나 무효한 채널을 지정한 경우.
*********************************************************************************************************
*/

BOOLEAN  CommIsEmpty (INT8U ch)
{
    BOOLEAN        empty;
    COMM_RING_BUF *pbuf;


    switch (ch) {                                          /* 통신 채널에 대한 포인터를 얻는다         */
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
    if (pbuf->RingBufRxCtr > 0) {                          /* 버퍼가 비었는가?                         */
        empty = FALSE;                                     /* 버퍼에 데이터가 유효함                   */
    } else {
        empty = TRUE;                                      /* 버퍼가 비었음                            */
    }
    OS_EXIT_CRITICAL();
    return (empty);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                       송신 버퍼가 찼는지 확인
*
*
* 설명        : 응용프로그램은 송신 버퍼에 데이터를 더 저장할 수 있는 지 확인하기 위해 이 함수를
*               호출한다. 즉, 이 함수는 송신 버퍼가 찼는지 확인한다.
*               버퍼가 찬 경우, 이 함수는 TRUE를 리턴하고, 그렇지 않은 경우 FALSE를 리턴한다.
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
* 리턴값      : TRUE    버퍼가 찬 경우.
*               FALSE   버퍼가 차지 않았거나 무효한 채널을 지정한 경우.
*********************************************************************************************************
*/

BOOLEAN  CommIsFull (INT8U ch)
{
    BOOLEAN        full;
    COMM_RING_BUF *pbuf;


    switch (ch) {                                          /* 통신 채널에 대한 포인터를 얻는다         */
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
    if (pbuf->RingBufTxCtr < COMM_TX_BUF_SIZE) {           /* 버퍼가 찼는가?                           */
        full = FALSE;                                      /* 버퍼가 차지 않았음                       */
    } else {
        full = TRUE;                                       /* 버퍼가 찼음                              */
    }
    OS_EXIT_CRITICAL();
    return (full);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             데이터 출력
*
*
* 설명        : 응용프로그램은 통신채널로 한 바이트의 데이터를 보내기 위해 이 함수를 호출한다.
*               보내진 데이터는 송신 버퍼에 저장되고 송신 ISR에 의해 시리얼 포트로 전송된다.
*               만약 데이터가 버퍼의 첫 번째 데이터라면 송신 ISR이 활성화 된다.
*               송신 버퍼가 찼다면, 데이터는 전송되지 않고 무시된다.
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
*               'c'     송신하고자 하는 데이터.
* 리턴값      : COMM_NO_ERR   성공적으로 함수가 완료된 경우(버퍼가 차지 않았음)
*               COMM_TX_FULL  버퍼가 찬 경우
*               COMM_BAD_CH   무효 채널을 지정한 경우
*********************************************************************************************************
*/

INT8U  CommPutChar (INT8U ch, INT8U c)
{
    COMM_RING_BUF *pbuf;


    switch (ch) {                                          /* 통신 채널에 대한 포인터를 얻는다         */
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
    if (pbuf->RingBufTxCtr < COMM_TX_BUF_SIZE) {           /* 버퍼가 찼는가?                           */
        pbuf->RingBufTxCtr++;                              /* 아니오, 데이터 카운트 증가               */
        *pbuf->RingBufTxInPtr++ = c;                       /* 데이터를 버퍼에 저장한다                 */
        if (pbuf->RingBufTxInPtr == &pbuf->RingBufTx[COMM_TX_BUF_SIZE]) { /* 포인터 경계 처리          */
            pbuf->RingBufTxInPtr = &pbuf->RingBufTx[0];
        }
        if (pbuf->RingBufTxCtr == 1) {                     /* 첫 번째 데이터인가?                      */
            CommTxIntEn(ch);                               /* 예, 송신 인터럽트 활성화                 */
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
*                                       링 버퍼에 데이터를 저장
*
*
* 설명        : 수신 ISR은 데이터를 링 버퍼에 저장하기 위해 이 함수를 호출한다.
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
*               'c'     수신 링 버퍼에 저장하고자 하는 데이터.  버퍼가 찼다면, 수신된 데이터는
*                       저장되지 않고 무시된다.
*********************************************************************************************************
*/

void  CommPutRxChar (INT8U ch, INT8U c)
{
    COMM_RING_BUF *pbuf;


    switch (ch) {                                          /* 통신 채널에 대한 포인터를 얻는다         */
        case COMM1:
             pbuf = &Comm1Buf;
             break;

        case COMM2:
             pbuf = &Comm2Buf;
             break;

        default:
             return;
    }
    if (pbuf->RingBufRxCtr < COMM_RX_BUF_SIZE) {           /* 버퍼가 찼는가?                           */
        pbuf->RingBufRxCtr++;                              /* 아니오, 데이터 카운터를 중가시킨다       */
        *pbuf->RingBufRxInPtr++ = c;                       /* 데이터를 버퍼에 저장한다                 */
        if (pbuf->RingBufRxInPtr == &pbuf->RingBufRx[COMM_RX_BUF_SIZE]) { /* 포인터 경계 처리          */
            pbuf->RingBufRxInPtr = &pbuf->RingBufRx[0];
        }
    }
}
