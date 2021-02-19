/*
*********************************************************************************************************
*                                     Embedded Systems Building Blocks
*                                  Complete and Ready-to-Use Modules in C
*
*                                    Asynchronous Serial Communications
*                                    IBM-PC Serial I/O Low Level Driver
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* Filename   : COMM_PC.C
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*
* Notes      : 1) 이 파일의 코드는 대부분의 PC에서 사용되는 시리얼 통신 컨트롤러인
*                 National Semiconductor사의 NS16450 이나 Intel i82C50를 사용하는 것으로 가정하고 있다.
*
*              2) OS_ENTER_CRITICAL() 과 OS_EXIT_CRITICAL() 매크로는 각각 인터럽트를 비활성화, 활성화
*                 하는 데 사용된다. 만약 Borland C++ compiler V3.1을 사용한다면 두 개의 매크로를
*                 다음과 같이 정의해야 한다
*
*                     #define OS_ENTER_CRITICAL()  disable()
*                     #define OS_EXIT_CRITICAL()   enable()
*
*              3) 다음의 상수를 정의해야 한다
*                     COMM1_BASE    PC 상의 COM1에 대한 기준 어드레스(일반적으로 0x03F8)
*                     COMM2_BASE    PC 상의 COM2에 대한 기준 어드레스(일반적으로 0x02F8)
*                     COMM_MAX_RX   UART에 의해 버퍼링 되는 데이터의 수
*                                       NS16450 일 경우 2
*                                       NS16550 일 경우 16
*
*              4) COMM_BAD_CH, COMM_NO_ERR 그리고 COMM_TX_EMPTY,
*                 COMM_NO_PARITY, COMM_ODD_PARITY 그리고 COMM_EVEN_PARITY
*                     는 모두 다른 모듈에 정의되어 있다(COMM1.H, COMM2.H 또는 COMM3.H)
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

#define  BIT0                       0x01
#define  BIT1                       0x02
#define  BIT2                       0x04
#define  BIT3                       0x08
#define  BIT4                       0x10
#define  BIT5                       0x20
#define  BIT6                       0x40
#define  BIT7                       0x80

#define  PIC_INT_REG_PORT         0x0020
#define  PIC_MSK_REG_PORT         0x0021

#define  COMM_UART_RBR                 0
#define  COMM_UART_THR                 0
#define  COMM_UART_DIV_LO              0
#define  COMM_UART_DIV_HI              1
#define  COMM_UART_IER                 1
#define  COMM_UART_IIR                 2
#define  COMM_UART_LCR                 3
#define  COMM_UART_MCR                 4
#define  COMM_UART_LSR                 5
#define  COMM_UART_MSR                 6
#define  COMM_UART_SCR                 7

/*
*********************************************************************************************************
*                                          지역적 전역 변수
*********************************************************************************************************
*/

static  void   (*Comm1ISROld)(void);
static  void   (*Comm2ISROld)(void);

/*$PAGE*/
/*
*********************************************************************************************************
*                                               포트 설정
*
* 설명        : 이 함수는 시리얼 I/O 포트를 설정한다.  코드는 IBM-PC 호환기종 용이며 
*               National Semiconductor사의  NS16450를 사용하는 것으로 가정한다.
*
* 전달인자    : 'ch'          COMM 포트 채널 번호. 다음과 같은 값을 가질 수 있다.
*                                 COMM1
*                                 COMM2
*               'baud'        원하는 보레이트(표준 보레이트가 아닌 어떤 값도 사용 가능)
*               'bits'        데이터의 비트 수 : 5, 6, 7 또는 8.
*               'parity'      패리티 설정:
*                                 COMM_PARITY_NONE
*                                 COMM_PARITY_ODD
*                                 COMM_PARITY_EVEN
*               'stops'       스톱 비트 설정 : 1 또는 2.
*
* 리턴값      : COMM_NO_ERR   채널이 정상적으로 설정되었음.
*               COMM_BAD_CH   비 정상적인 채널 번호를 지정한 경우.
*
* 노트        : 1) NS16450 데이터 시트 참조
*               2) 상수 115200는 1.8432 MHz 크리스털 발진자와 16x 클럭을 사용하는 것에서 기인.
*               3) 'lcr'은 Line Control Register를 말하며 다음과 같이 정의된다:
*
*                        B7  B6  B5  B4  B3  B2  B1  B0
*                                                ------ 데이터 비트 수(00 = 5, 01 = 6, 10 = 7 and 11 = 8)
*                                            --         스톱 비트 수(0 = 1 stop, 1 = 2 stops)
*                                        --             패리티 활성화(1 = 패리티가 활성화된다)
*                                    --                 1일 때 짝수 패리티.
*                                --                     Stick 패리티(16450 데이터 시트 참조)
*                            --                         Break 컨트롤(force break when 1)
*                        --                             Divisor 액세스 비트(Divisor를 액세스하려면 1로..)
*               4) 이 함수는 수신 인터럽트는 활성화하지만 송신 인터럽트는 그대로 둔다.
*********************************************************************************************************
*/
INT8U  CommCfgPort (INT8U ch, INT16U baud, INT8U bits, INT8U parity, INT8U stops)
{
    INT16U  div;                                      /* 보레이트 divisor                              */
    INT8U   divlo;
    INT8U   divhi;
    INT8U   lcr;                                      /* 라인 컨트롤 레지스터                          */
    INT16U  base;                                     /* COMM 포트 기준 어드레스                       */


    switch (ch) {                                     /* COMM 포트의 기준 어드레스를 얻는다            */
        case COMM1:
             base = COMM1_BASE;
             break;

        case COMM2:
             base = COMM2_BASE;
             break;

        default:
             return (COMM_BAD_CH);
    }
    div   = (INT16U)(115200L / (INT32U)baud);         /* 보레이트에 대한 제수를 계산한다               */
    divlo =  div & 0x00FF;                            /* 제수를 LOW 와 HIGH 바이트로 나눈다            */
    divhi = (div >> 8) & 0x00FF;
    lcr   = ((stops - 1) << 2) + (bits - 5);
    switch (parity) {
        case COMM_PARITY_ODD: 
             lcr |= 0x08;                             /* 홀수 패리티                                   */
             break;

        case COMM_PARITY_EVEN: 
             lcr |= 0x18;                             /* 짝수 패리티                                   */
             break;
    }
    OS_ENTER_CRITICAL();
    outp(base + COMM_UART_LCR, BIT7);                 /* 제수 액세스 비트 설정                         */
    outp(base + COMM_UART_DIV_LO, divlo);             /* 제수를 로드한다                               */
    outp(base + COMM_UART_DIV_HI, divhi);
    outp(base + COMM_UART_LCR, lcr);                  /* 라인 컨트롤 레지스터 설정(비트 8은 0)         */
    outp(base + COMM_UART_MCR, BIT3 | BIT1 | BIT0);   /* DTR 과 RTS를  high로 하고 인터럽트를 허용     */
    outp(base + COMM_UART_IER, 0x00);                 /* 송수신 인터럽트를 모두 비활성화한다           */
    OS_EXIT_CRITICAL();
    CommRxFlush(ch);                                  /* Rx 입력을 클리어 한다                         */
    return (COMM_NO_ERR);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           COMM ISR 핸들러
*
* 설명        : 이 함수는 COMM 포트로부터의 인터럽트를 처리한다. 이 함수는 인터럽트가 수신 문자 때문에
*               발생한 것인 지 또는 송신을 완료해서 발생했는 지 또는 양자에 의해 발생했는 지를 점검
*               해야 한다
* 전달인자    : 'ch'    COMM 포트의 채널 번호이며 값은 다음과 같다:
*                           COMM1
*                           COMM2
* 노트        : 'switch'문은 확장을 위해 사용되었다.
*********************************************************************************************************
*/
void  CommISRHandler (INT8U ch)
{
    INT8U   c;
    INT8U   iir;                                          /* 인터럽트 확인 레지스터(IIR)               */
    INT8U   stat;
    INT16U  base;                                         /* COMM 포트 기준 어드레스                   */
    INT8U   err;
    INT8U   max;                                          /* 처리할 인터럽트의 최대 수                 */


    switch (ch) {                                         /* 통신 채널의 포인터를 얻는다               */
        case COMM1:
             base = COMM1_BASE;
             break;

        case COMM2:
             base = COMM2_BASE;
             break;

        default:
             base = COMM1_BASE;
             break;
    }
    max = COMM_MAX_RX;
    iir = (INT8U)inp(base + COMM_UART_IIR) & 0x07;        /* IIR의 내용을 읽는다                       */
    while (iir != 1 && max > 0) {                         /* 모든 인터럽트를 처리한다                  */
        switch (iir) {
            case 0:                                       /* 모뎀 상태 인터럽트                        */
                 c = (INT8U)inp(base + COMM_UART_MSR);    /* 인터럽트를 클리어하고 아무것도 하지 않음  */
                 break;

            case 2:                                       /* 송신완료 인터럽트                         */
                 c = CommGetTxChar(ch, &err);             /* 다음 송신문자를 버퍼에서 읽어온다         */
                 if (err == COMM_TX_EMPTY) {              /* 보낼 문자가 없는가?                       */
                                                          /* 예,  송신 인터럽트를 비활성화한다         */
                     stat = (INT8U)inp(base + COMM_UART_IER) & ~BIT1; 
                     outp(base + COMM_UART_IER, stat);
                 } else {
                     outp(base + COMM_UART_THR, c);       /* 아니오, 문자를 송신한다                   */
                 }
                 break;

            case 4:                                       /* 수신 인터럽트                             */
                 c  = (INT8U)inp(base + COMM_UART_RBR);   /* 수신된 문자를 읽는다                      */
                 CommPutRxChar(ch, c);                    /* 수신된 문자를 버퍼에 저장한다             */
                 break;

            case 6:                                       /* 라인 상태 인터럽트                        */
                 c   = (INT8U)inp(base + COMM_UART_LSR);  /* 인터럽트를 클리어하고 아무것도 하지 않음  */
                 break;
        }
        iir = (INT8U)inp(base + COMM_UART_IIR) & 0x07;    /* IIR의 내용을 읽는다                       */
        max--;
    }
    switch (ch) {
        case COMM1:
        case COMM2:
             outp(PIC_INT_REG_PORT, 0x20);                /* 인터럽트 컨트롤러를 리셋한다              */
             break;

        default:
             outp(PIC_INT_REG_PORT, 0x20);
             break;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                      저장된 인터럽트 벡터 복구
*
* 설명        : 이 함수는 해당 통신 채널에 대한 저장된 인터럽트 벡터를 복구시킨다.
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
* 노트        : 이 함수는 80x86이 리얼모드에서 실행되는 것으로 가정한다.
*********************************************************************************************************
*/

void  CommRclIntVect (INT8U ch)
{
    CommRxIntDis(ch);
    CommRxFlush(ch);
    CommTxIntDis(ch);
    switch (ch) {
        case COMM1:
             PC_VectSet(0x0C, Comm1ISROld);
             break;

        case COMM2:
             PC_VectSet(0x0B, Comm1ISROld);
             break;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           수신 포트 클리어
*
* 설명        : 이 함수는 리시버에 남아 있는 수신 데이터를 모두 클리어 한다. 또한 이 함수는 더욱 
*               우수한 성능을 가진 NS16550로 NS16450를 대치할 때 유용하다.
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
*********************************************************************************************************
*/

void  CommRxFlush (INT8U ch)
{
    INT8U  ctr;
    INT16U base;


    switch (ch) {
        case COMM1:
             base = COMM1_BASE;
             break;

        case COMM2:
             base = COMM2_BASE;
             break;
    }
    ctr = COMM_MAX_RX;                                /* 클리어할 데이터 갯수 설정                     */
    OS_ENTER_CRITICAL();
    while (ctr-- > 0) {
        inp(base + COMM_UART_RBR);
    }
    OS_EXIT_CRITICAL();
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         수신 인터럽트 비활성화
*
* 설명        : 이 함수는 수신 인터럽트를 비활성화한다.
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
*********************************************************************************************************
*/

void  CommRxIntDis (INT8U ch)
{
    INT8U stat;


    switch (ch) {
        case COMM1:
             OS_ENTER_CRITICAL();
                                                           /* 수신 인터럽트 비활성화                   */
             stat = (INT8U)inp(COMM1_BASE + COMM_UART_IER) & ~BIT0;    
             outp(COMM1_BASE + COMM_UART_IER, stat);
             if (stat == 0x00) {                           /* 송수신 인터럽트가 모두 비활성화됐는가?   */
                                                           /* 예, PC의 IRQ4를 비활성화한다             */
                 outp(PIC_MSK_REG_PORT, (INT8U)inp(PIC_MSK_REG_PORT) | BIT4);  
             }
             OS_EXIT_CRITICAL();
             break;

        case COMM2:
             OS_ENTER_CRITICAL();
                                                           /* 수신 인터럽트 비활성화                   */
             stat = (INT8U)inp(COMM2_BASE + COMM_UART_IIR) & ~BIT0;    
             outp(COMM2_BASE + COMM_UART_IER, stat);
             if (stat == 0x00) {                           /* 송수신 인터럽트가 모두 비활성화됐는가?   */
                                                           /* 예, PC의 IRQ3을 비활성화한다             */
                 outp(PIC_MSK_REG_PORT, (INT8U)inp(PIC_MSK_REG_PORT) | BIT3);  
             }
             OS_EXIT_CRITICAL();
             break;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                          수신 인터럽트 활성화
*
* 설명        : 이 함수는 수신 인터럽트를 활성화한다.
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
*********************************************************************************************************
*/

void  CommRxIntEn (INT8U ch)
{
    INT8U stat;


    switch (ch) {
        case COMM1:
             OS_ENTER_CRITICAL();
                                                           /* 수신 인터럽트 활성화                     */
             stat = (INT8U)inp(COMM1_BASE + COMM_UART_IER) | BIT0;     
             outp(COMM1_BASE + COMM_UART_IER, stat);
                                                           /* PC의 IRQ4를 활성화한다                   */
             outp(PIC_MSK_REG_PORT, (INT8U)inp(PIC_MSK_REG_PORT) & ~BIT4);     
             OS_EXIT_CRITICAL();
             break;

        case COMM2:
             OS_ENTER_CRITICAL();
                                                           /* 수신 인터럽트를 활성화한다               */
             stat = (INT8U)inp(COMM2_BASE + COMM_UART_IER) | BIT0;     
             outp(COMM2_BASE + COMM_UART_IER, stat);
                                                           /* PC의 IRQ3을 활성화한다                   */
             outp(PIC_MSK_REG_PORT, (INT8U)inp(PIC_MSK_REG_PORT) & ~BIT3);     
             OS_EXIT_CRITICAL();
             break;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                          인터럽트 벡터 설정
*
* 설명        : 이 함수는 해당 채널 번호의 인터럽트 벡터를 설정한다.
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
* 노트        : 이 함수는 80x86이 리얼모드에서 실행되는 것으로 가정한다.
*********************************************************************************************************
*/

void  CommSetIntVect (INT8U ch)
{
    switch (ch) {
        case COMM1:
             Comm1ISROld = PC_VectGet(0x0C);
             PC_VectSet(0x0C, Comm1ISR);
             break;

        case COMM2:
             Comm2ISROld = PC_VectGet(0x0B);
             PC_VectSet(0x0B, Comm2ISR);
             break;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         송신 인터럽트 비활성화
*
* 설명        : 이 함수는 데이터 송신을 비활성화한다.
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
*********************************************************************************************************
*/

void  CommTxIntDis (INT8U ch)
{
    INT8U  stat;
    INT8U  cmd;


    switch (ch) {
        case COMM1:
             OS_ENTER_CRITICAL();
                                                           /* 송신 인터럽트 비활성화                   */
             stat = (INT8U)inp(COMM1_BASE + COMM_UART_IER) & ~BIT1;    
             outp(COMM1_BASE + COMM_UART_IER, stat);
             if (stat == 0x00) {                           /* 송수신 인터럽트가 모두 비활성화됐는가?   */
                 cmd = (INT8U)inp(PIC_MSK_REG_PORT) | BIT4;
                 outp(PIC_MSK_REG_PORT, cmd);              /* 예, PC의 IRQ4를 비활성화한다             */
             }
             OS_EXIT_CRITICAL();
             break;

        case COMM2:
             OS_ENTER_CRITICAL();
                                                           /* 송신 인터럽트 비활성화                   */
             stat = (INT8U)inp(COMM2_BASE + COMM_UART_IER) & ~BIT1;    
             outp(COMM2_BASE + COMM_UART_IER, stat);
             if (stat == 0x00) {                           /* 송수신 인터럽트가 모두 비활성화됐는가?   */
                 cmd = (INT8U)inp(PIC_MSK_REG_PORT) | BIT3;
                 outp(PIC_MSK_REG_PORT, cmd);              /* 예, PC의 IRQ3을 비활성화한다             */
             }
             OS_EXIT_CRITICAL();
             break;
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          송신 인터럽트 활성화
*
* 설명        : 이 함수는 데이터 송신을 활성화한다. 데이터 송신은 인터럽트 방식이다.
*               멀티-드롭 드라이버를 사용한다면 이 함수는 송신을 위해 드라이버를 활성화해야 한다.
*
* 전달인자    : 'ch'    COMM 포트 채널 번호. 다음과 같은 값을 갖는다:
*                           COMM1
*                           COMM2
*********************************************************************************************************
*/

void  CommTxIntEn (INT8U ch)
{
    INT8U  stat;
    INT8U  cmd;


    switch (ch) {
        case COMM1:
             OS_ENTER_CRITICAL();
             stat = (INT8U)inp(COMM1_BASE + COMM_UART_IER) | BIT1;     /* 송신 인터럽트 활성화         */
             outp(COMM1_BASE + COMM_UART_IER, stat);
             cmd  = (INT8U)inp(PIC_MSK_REG_PORT) & ~BIT4;
             outp(PIC_MSK_REG_PORT, cmd);                              /* PC의 IRQ4를 활성화한다       */
             OS_EXIT_CRITICAL();
             break;

        case COMM2:
             OS_ENTER_CRITICAL();
             stat = (INT8U)inp(COMM2_BASE + COMM_UART_IER) | BIT1;     /* 송신 인터럽트 활성화         */
             outp(COMM2_BASE + COMM_UART_IER, stat);
             cmd  = (INT8U)inp(PIC_MSK_REG_PORT) & ~BIT3;
             outp(PIC_MSK_REG_PORT, cmd);                              /* PC의 IRQ3을 활성화한다       */
             OS_EXIT_CRITICAL();
             break;
    }
}
