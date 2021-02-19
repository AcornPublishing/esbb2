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
*                                              데이터 타입
*********************************************************************************************************
*/

typedef struct {
    INT16U     RingBufRxCtr;                /* Rx 링 버퍼에 저장된 데이터 수                           */
    OS_EVENT  *RingBufRxSem;                /* 수신 세마포어의 포인터                                  */
    INT8U     *RingBufRxInPtr;              /* 다음 데이터를 저장할 곳을 가리키는 포인터               */ 
    INT8U     *RingBufRxOutPtr;             /* 다음에 읽어갈 데이터를 가리키는 포인터                  */ 
    INT8U      RingBufRx[COMM_RX_BUF_SIZE]; /* 수신용 링 버퍼 저장영역(Rx)                             */ 
    INT16U     RingBufTxCtr;                /* Tx 링 버퍼에 저장된 데이터 수                           */
    OS_EVENT  *RingBufTxSem;                /* 송신 세마포어의 포인터                                  */
    INT8U     *RingBufTxInPtr;              /* 다음 데이터를 저장할 곳을 가리키는 포인터               */
    INT8U     *RingBufTxOutPtr;             /* 다음에 읽어갈 데이터를 가리키는 포인터                  */
    INT8U      RingBufTx[COMM_TX_BUF_SIZE]; /* 송신용 링 버퍼 저장영역(Tx)                             */
} COMM_RING_BUF;

/*
*********************************************************************************************************
*                                              전역변수
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
*               이 함수는 통신채널에 데이터가 수신되거나 타임아웃이 만료될 때 까지 대기한다.
*
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
*               'to'    데이터가 수신되기를 기다리는 타임아웃을 클럭 틱 단위로 지정한다.
*                       이 값을 0으로 지정하면 함수는 데이터가 수신될 때 까지 무한정 대기한다.
*
*               'err'   에러코드가 저장될 변수를 가리키는 포인터. 다음과 같은 값이 설정된다:
*                           COMM_NO_ERR : 버퍼에서 데이터를 읽어 낸 경우
*                           COMM_RX_TIMEOUT : 타임아웃이 만료된 경우
*                           COMM_BAD_CH : 무효 채널을 지정한 경우
* 리턴값      : 버퍼에서 읽어낸 데이터(타임아웃이 만료된 경우는 NUL을 리턴한다)
*********************************************************************************************************
*/

INT8U  CommGetChar (INT8U ch, INT16U to, INT8U *err)
{
    INT8U          c;
    INT8U          oserr;
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
    OSSemPend(pbuf->RingBufRxSem, to, &oserr);             /* 데이터가 수신되기를 기다린다             */
    if (oserr == OS_TIMEOUT) {                             /* 타임아웃이 만료되었는가?                 */
        *err = COMM_RX_TIMEOUT;                            /* 예, 에러코드를 리턴한다                  */
        return (NUL);
    } else {
        OS_ENTER_CRITICAL();
        pbuf->RingBufRxCtr--;                              /* 아니오, 데이터 카운터를 감소시킨다       */
        c = *pbuf->RingBufRxOutPtr++;                      /* 버퍼에서 데이터를 읽어낸다               */
        if (pbuf->RingBufRxOutPtr == &pbuf->RingBufRx[COMM_RX_BUF_SIZE]) {     /* 포인터 경계 처리     */
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
    if (pbuf->RingBufTxCtr > 0) {                          /* 버퍼에 데이터가 있는가?                  */
        pbuf->RingBufTxCtr--;                              /* 예, 데이터 카운터를 감소시킨다           */
        c = *pbuf->RingBufTxOutPtr++;                      /* 버퍼에서 데이터를 읽는다                 */
        if (pbuf->RingBufTxOutPtr == &pbuf->RingBufTx[COMM_TX_BUF_SIZE]) {     /* 포인터 경계 처리     */
            pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];
        }
        OSSemPost(pbuf->RingBufTxSem);                     /* 버퍼에 한개의 저장영역이 생겼음을 알린다 */
        *err = COMM_NO_ERR;
        return (c);                                        /* 정상적으로 데이터를 읽었음               */
    } else {
        *err = COMM_TX_EMPTY;
        return (NUL);                                      /* 버퍼가 비었음                            */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                          통신모듈 초기화
*
*
* 설명        : 응용프로그램은 통신모듈을 초기화 하기 위해 이 함수를 호출한다.
*               통신모듈의 다른 함수를 호출하기 전에 반드시 이 함수를 먼저 호출해야 한다.
* 전달인자    : 없음
*********************************************************************************************************
*/

void  CommInit (void)
{
    COMM_RING_BUF *pbuf;


    pbuf                  = &Comm1Buf;                     /* COMM1에 대한 링 버퍼 초기화              */
    pbuf->RingBufRxCtr    = 0;
    pbuf->RingBufRxInPtr  = &pbuf->RingBufRx[0];
    pbuf->RingBufRxOutPtr = &pbuf->RingBufRx[0];
    pbuf->RingBufRxSem    = OSSemCreate(0);
    pbuf->RingBufTxCtr    = 0;
    pbuf->RingBufTxInPtr  = &pbuf->RingBufTx[0];
    pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];
    pbuf->RingBufTxSem    = OSSemCreate(COMM_TX_BUF_SIZE);

    pbuf                  = &Comm2Buf;                     /* COMM2에 대한 링 버퍼 초기화              */
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
*                                       수신 버퍼가 비었는지 확인
*
*
* 설명        : 응용프로그램은 통신 채널에 데이터가 유효한지 확인하기 위해 이 함수를 호출한다.
*               한 바이트 이상의 데이터가 유효하면 FALSE를 리턴하고 그렇지 않은 경우 TRUE를 리턴한다.
*
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
* 리턴값      : TRUE    버퍼가 빈 경우
*               FALSE   버퍼가 비지 않는 경우 또는 무효 채널을 지정한 경우
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
    if (pbuf->RingBufRxCtr > 0) {                          /* 버퍼에 데이터가 있는가?                  */
        empty = FALSE;                                     /* 버퍼가 비지 않음                         */
    } else {
        empty = TRUE;                                      /* 버퍼가 비었음                            */
    }
    OS_EXIT_CRITICAL();
    return (empty);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                    송신버퍼가 완전히 찼는지 확인
*
*
* 설명        : 응용프로그램은 송신버퍼에 데이터를 더 저장할 수 있는지 확인하기 위해 이 함수를 호출한다.
*               즉, 이 함수는 송신버퍼가 완전히 찼는지 확인한다.
*               버퍼가 찬 경우 함수는 TRUE를 리턴하고 그렇지 않은 경우는 FALSE를 리턴한다.
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
* 리턴값      : TRUE    버퍼가 완전히 찼음.
*               FALSE   버퍼가 차지 않았거나 무효채널을 지정한 경우.
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
    if (pbuf->RingBufTxCtr < COMM_TX_BUF_SIZE) {           /* 버퍼가 찼는지 확인                       */
        full = FALSE;                                      /* 버퍼가 차지 않음                         */
    } else {
        full = TRUE;                                       /* 버퍼가 완전히 찼음                       */
    }
    OS_EXIT_CRITICAL();
    return (full);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                              데이터 출력
*
*
* 설명        : 이 함수는 통신채널로 데이터를 보내기 위해 호출된다. 버퍼가 완전히 찬 경우는 버퍼가 빌
*               때까지 기다린다. 지정한 타임아웃이 만료될 때까지 버퍼가 비지 않으면 호출함수로 리턴한다.
*               타임아웃을 0으로 지정하면 버퍼가 빌 때까지 영원히 기다린다.
*               가장 먼저 버퍼에 저장되는 데이터가 송신 ISR에 의해 전송된다.
*               버퍼가 완전히 빈 상태에서 첫 번째 데이터가 버퍼에 저장될 때 송신 ISR이 활성화 된다.
*
*
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
*               'c'     버퍼에 저장하고자 하는 데이터.
*               'to'    버퍼가 찬 경우 빌 때까지 기다리도록 지정하는 클럭 틱 단위의 타임아웃 값.
*                       타임아웃으로 0을 지정하면 버퍼가 빌 때까지 영원히 기다린다.
* 리턴값      : COMM_NO_ERR      데이터가 정상적으로 버퍼에 저장된 경우
*               COMM_TX_TIMEOUT  지정된 타임아웃이 만료될 때 까지 버퍼가 비지 않은 경우
*               COMM_BAD_CH      무효한 채널 번호를 지정한 경우
*********************************************************************************************************
*/

INT8U  CommPutChar (INT8U ch, INT8U c, INT16U to)
{
    INT8U          oserr;
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
    OSSemPend(pbuf->RingBufTxSem, to, &oserr);             /* 송신버퍼에 빈 공간이 생기길 기다린다     */
    if (oserr == OS_TIMEOUT) {
        return (COMM_TX_TIMEOUT);                          /* 타임아웃 만료됨, 에러코드 리턴           */
    }
    OS_ENTER_CRITICAL();
    pbuf->RingBufTxCtr++;                                  /* 데이터 카운터를 증가시킨다               */
    *pbuf->RingBufTxInPtr++ = c;                           /* 버퍼에 데이터를 저장한다                 */
    if (pbuf->RingBufTxInPtr == &pbuf->RingBufTx[COMM_TX_BUF_SIZE]) {     /* 포인터 경계 처리          */
        pbuf->RingBufTxInPtr = &pbuf->RingBufTx[0];
    }
    if (pbuf->RingBufTxCtr == 1) {                         /* 첫 번째 데이터인가?                      */
        CommTxIntEn(ch);                                   /* 예, 송신 인터럽트 활성화                 */
    }
    OS_EXIT_CRITICAL();
    return (COMM_NO_ERR);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        링 버퍼에 데이터 저장
*
*
* 설명        : 이 함수는 수신용 링 버퍼에 데이터를 저장하기 위해 ISR에 의해 호출된다.
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
*               'c'     링 버퍼에 저장하려는 데이터.  버퍼가 차 있다면 데이터는 저장되지 않고 무시된다.
*
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
    if (pbuf->RingBufRxCtr < COMM_RX_BUF_SIZE) {           /* 버퍼에 차 있는가?                        */
        pbuf->RingBufRxCtr++;                              /* 아니오, 데이터 카운터 증가               */
        *pbuf->RingBufRxInPtr++ = c;                       /* 버퍼에 저장                              */
        if (pbuf->RingBufRxInPtr == &pbuf->RingBufRx[COMM_RX_BUF_SIZE]) { /* 포인터 경계 처리          */
            pbuf->RingBufRxInPtr = &pbuf->RingBufRx[0];
        }
        OSSemPost(pbuf->RingBufRxSem);                     /* 데이터가 수신된 것을 알려줌              */
    }
}
