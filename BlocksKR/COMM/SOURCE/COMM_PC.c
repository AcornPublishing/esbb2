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
* Notes      : 1) �� ������ �ڵ�� ��κ��� PC���� ���Ǵ� �ø��� ��� ��Ʈ�ѷ���
*                 National Semiconductor���� NS16450 �̳� Intel i82C50�� ����ϴ� ������ �����ϰ� �ִ�.
*
*              2) OS_ENTER_CRITICAL() �� OS_EXIT_CRITICAL() ��ũ�δ� ���� ���ͷ�Ʈ�� ��Ȱ��ȭ, Ȱ��ȭ
*                 �ϴ� �� ���ȴ�. ���� Borland C++ compiler V3.1�� ����Ѵٸ� �� ���� ��ũ�θ�
*                 ������ ���� �����ؾ� �Ѵ�
*
*                     #define OS_ENTER_CRITICAL()  disable()
*                     #define OS_EXIT_CRITICAL()   enable()
*
*              3) ������ ����� �����ؾ� �Ѵ�
*                     COMM1_BASE    PC ���� COM1�� ���� ���� ��巹��(�Ϲ������� 0x03F8)
*                     COMM2_BASE    PC ���� COM2�� ���� ���� ��巹��(�Ϲ������� 0x02F8)
*                     COMM_MAX_RX   UART�� ���� ���۸� �Ǵ� �������� ��
*                                       NS16450 �� ��� 2
*                                       NS16550 �� ��� 16
*
*              4) COMM_BAD_CH, COMM_NO_ERR �׸��� COMM_TX_EMPTY,
*                 COMM_NO_PARITY, COMM_ODD_PARITY �׸��� COMM_EVEN_PARITY
*                     �� ��� �ٸ� ��⿡ ���ǵǾ� �ִ�(COMM1.H, COMM2.H �Ǵ� COMM3.H)
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
*                                          ������ ���� ����
*********************************************************************************************************
*/

static  void   (*Comm1ISROld)(void);
static  void   (*Comm2ISROld)(void);

/*$PAGE*/
/*
*********************************************************************************************************
*                                               ��Ʈ ����
*
* ����        : �� �Լ��� �ø��� I/O ��Ʈ�� �����Ѵ�.  �ڵ�� IBM-PC ȣȯ���� ���̸� 
*               National Semiconductor����  NS16450�� ����ϴ� ������ �����Ѵ�.
*
* ��������    : 'ch'          COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���� �� �ִ�.
*                                 COMM1
*                                 COMM2
*               'baud'        ���ϴ� ������Ʈ(ǥ�� ������Ʈ�� �ƴ� � ���� ��� ����)
*               'bits'        �������� ��Ʈ �� : 5, 6, 7 �Ǵ� 8.
*               'parity'      �и�Ƽ ����:
*                                 COMM_PARITY_NONE
*                                 COMM_PARITY_ODD
*                                 COMM_PARITY_EVEN
*               'stops'       ���� ��Ʈ ���� : 1 �Ǵ� 2.
*
* ���ϰ�      : COMM_NO_ERR   ä���� ���������� �����Ǿ���.
*               COMM_BAD_CH   �� �������� ä�� ��ȣ�� ������ ���.
*
* ��Ʈ        : 1) NS16450 ������ ��Ʈ ����
*               2) ��� 115200�� 1.8432 MHz ũ������ �����ڿ� 16x Ŭ���� ����ϴ� �Ϳ��� ����.
*               3) 'lcr'�� Line Control Register�� ���ϸ� ������ ���� ���ǵȴ�:
*
*                        B7  B6  B5  B4  B3  B2  B1  B0
*                                                ------ ������ ��Ʈ ��(00 = 5, 01 = 6, 10 = 7 and 11 = 8)
*                                            --         ���� ��Ʈ ��(0 = 1 stop, 1 = 2 stops)
*                                        --             �и�Ƽ Ȱ��ȭ(1 = �и�Ƽ�� Ȱ��ȭ�ȴ�)
*                                    --                 1�� �� ¦�� �и�Ƽ.
*                                --                     Stick �и�Ƽ(16450 ������ ��Ʈ ����)
*                            --                         Break ��Ʈ��(force break when 1)
*                        --                             Divisor �׼��� ��Ʈ(Divisor�� �׼����Ϸ��� 1��..)
*               4) �� �Լ��� ���� ���ͷ�Ʈ�� Ȱ��ȭ������ �۽� ���ͷ�Ʈ�� �״�� �д�.
*********************************************************************************************************
*/
INT8U  CommCfgPort (INT8U ch, INT16U baud, INT8U bits, INT8U parity, INT8U stops)
{
    INT16U  div;                                      /* ������Ʈ divisor                              */
    INT8U   divlo;
    INT8U   divhi;
    INT8U   lcr;                                      /* ���� ��Ʈ�� ��������                          */
    INT16U  base;                                     /* COMM ��Ʈ ���� ��巹��                       */


    switch (ch) {                                     /* COMM ��Ʈ�� ���� ��巹���� ��´�            */
        case COMM1:
             base = COMM1_BASE;
             break;

        case COMM2:
             base = COMM2_BASE;
             break;

        default:
             return (COMM_BAD_CH);
    }
    div   = (INT16U)(115200L / (INT32U)baud);         /* ������Ʈ�� ���� ������ ����Ѵ�               */
    divlo =  div & 0x00FF;                            /* ������ LOW �� HIGH ����Ʈ�� ������            */
    divhi = (div >> 8) & 0x00FF;
    lcr   = ((stops - 1) << 2) + (bits - 5);
    switch (parity) {
        case COMM_PARITY_ODD: 
             lcr |= 0x08;                             /* Ȧ�� �и�Ƽ                                   */
             break;

        case COMM_PARITY_EVEN: 
             lcr |= 0x18;                             /* ¦�� �и�Ƽ                                   */
             break;
    }
    OS_ENTER_CRITICAL();
    outp(base + COMM_UART_LCR, BIT7);                 /* ���� �׼��� ��Ʈ ����                         */
    outp(base + COMM_UART_DIV_LO, divlo);             /* ������ �ε��Ѵ�                               */
    outp(base + COMM_UART_DIV_HI, divhi);
    outp(base + COMM_UART_LCR, lcr);                  /* ���� ��Ʈ�� �������� ����(��Ʈ 8�� 0)         */
    outp(base + COMM_UART_MCR, BIT3 | BIT1 | BIT0);   /* DTR �� RTS��  high�� �ϰ� ���ͷ�Ʈ�� ���     */
    outp(base + COMM_UART_IER, 0x00);                 /* �ۼ��� ���ͷ�Ʈ�� ��� ��Ȱ��ȭ�Ѵ�           */
    OS_EXIT_CRITICAL();
    CommRxFlush(ch);                                  /* Rx �Է��� Ŭ���� �Ѵ�                         */
    return (COMM_NO_ERR);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           COMM ISR �ڵ鷯
*
* ����        : �� �Լ��� COMM ��Ʈ�κ����� ���ͷ�Ʈ�� ó���Ѵ�. �� �Լ��� ���ͷ�Ʈ�� ���� ���� ������
*               �߻��� ���� �� �Ǵ� �۽��� �Ϸ��ؼ� �߻��ߴ� �� �Ǵ� ���ڿ� ���� �߻��ߴ� ���� ����
*               �ؾ� �Ѵ�
* ��������    : 'ch'    COMM ��Ʈ�� ä�� ��ȣ�̸� ���� ������ ����:
*                           COMM1
*                           COMM2
* ��Ʈ        : 'switch'���� Ȯ���� ���� ���Ǿ���.
*********************************************************************************************************
*/
void  CommISRHandler (INT8U ch)
{
    INT8U   c;
    INT8U   iir;                                          /* ���ͷ�Ʈ Ȯ�� ��������(IIR)               */
    INT8U   stat;
    INT16U  base;                                         /* COMM ��Ʈ ���� ��巹��                   */
    INT8U   err;
    INT8U   max;                                          /* ó���� ���ͷ�Ʈ�� �ִ� ��                 */


    switch (ch) {                                         /* ��� ä���� �����͸� ��´�               */
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
    iir = (INT8U)inp(base + COMM_UART_IIR) & 0x07;        /* IIR�� ������ �д´�                       */
    while (iir != 1 && max > 0) {                         /* ��� ���ͷ�Ʈ�� ó���Ѵ�                  */
        switch (iir) {
            case 0:                                       /* �� ���� ���ͷ�Ʈ                        */
                 c = (INT8U)inp(base + COMM_UART_MSR);    /* ���ͷ�Ʈ�� Ŭ�����ϰ� �ƹ��͵� ���� ����  */
                 break;

            case 2:                                       /* �۽ſϷ� ���ͷ�Ʈ                         */
                 c = CommGetTxChar(ch, &err);             /* ���� �۽Ź��ڸ� ���ۿ��� �о�´�         */
                 if (err == COMM_TX_EMPTY) {              /* ���� ���ڰ� ���°�?                       */
                                                          /* ��,  �۽� ���ͷ�Ʈ�� ��Ȱ��ȭ�Ѵ�         */
                     stat = (INT8U)inp(base + COMM_UART_IER) & ~BIT1; 
                     outp(base + COMM_UART_IER, stat);
                 } else {
                     outp(base + COMM_UART_THR, c);       /* �ƴϿ�, ���ڸ� �۽��Ѵ�                   */
                 }
                 break;

            case 4:                                       /* ���� ���ͷ�Ʈ                             */
                 c  = (INT8U)inp(base + COMM_UART_RBR);   /* ���ŵ� ���ڸ� �д´�                      */
                 CommPutRxChar(ch, c);                    /* ���ŵ� ���ڸ� ���ۿ� �����Ѵ�             */
                 break;

            case 6:                                       /* ���� ���� ���ͷ�Ʈ                        */
                 c   = (INT8U)inp(base + COMM_UART_LSR);  /* ���ͷ�Ʈ�� Ŭ�����ϰ� �ƹ��͵� ���� ����  */
                 break;
        }
        iir = (INT8U)inp(base + COMM_UART_IIR) & 0x07;    /* IIR�� ������ �д´�                       */
        max--;
    }
    switch (ch) {
        case COMM1:
        case COMM2:
             outp(PIC_INT_REG_PORT, 0x20);                /* ���ͷ�Ʈ ��Ʈ�ѷ��� �����Ѵ�              */
             break;

        default:
             outp(PIC_INT_REG_PORT, 0x20);
             break;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                      ����� ���ͷ�Ʈ ���� ����
*
* ����        : �� �Լ��� �ش� ��� ä�ο� ���� ����� ���ͷ�Ʈ ���͸� ������Ų��.
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
*                           COMM1
*                           COMM2
* ��Ʈ        : �� �Լ��� 80x86�� �����忡�� ����Ǵ� ������ �����Ѵ�.
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
*                                           ���� ��Ʈ Ŭ����
*
* ����        : �� �Լ��� ���ù��� ���� �ִ� ���� �����͸� ��� Ŭ���� �Ѵ�. ���� �� �Լ��� ���� 
*               ����� ������ ���� NS16550�� NS16450�� ��ġ�� �� �����ϴ�.
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
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
    ctr = COMM_MAX_RX;                                /* Ŭ������ ������ ���� ����                     */
    OS_ENTER_CRITICAL();
    while (ctr-- > 0) {
        inp(base + COMM_UART_RBR);
    }
    OS_EXIT_CRITICAL();
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                         ���� ���ͷ�Ʈ ��Ȱ��ȭ
*
* ����        : �� �Լ��� ���� ���ͷ�Ʈ�� ��Ȱ��ȭ�Ѵ�.
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
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
                                                           /* ���� ���ͷ�Ʈ ��Ȱ��ȭ                   */
             stat = (INT8U)inp(COMM1_BASE + COMM_UART_IER) & ~BIT0;    
             outp(COMM1_BASE + COMM_UART_IER, stat);
             if (stat == 0x00) {                           /* �ۼ��� ���ͷ�Ʈ�� ��� ��Ȱ��ȭ�ƴ°�?   */
                                                           /* ��, PC�� IRQ4�� ��Ȱ��ȭ�Ѵ�             */
                 outp(PIC_MSK_REG_PORT, (INT8U)inp(PIC_MSK_REG_PORT) | BIT4);  
             }
             OS_EXIT_CRITICAL();
             break;

        case COMM2:
             OS_ENTER_CRITICAL();
                                                           /* ���� ���ͷ�Ʈ ��Ȱ��ȭ                   */
             stat = (INT8U)inp(COMM2_BASE + COMM_UART_IIR) & ~BIT0;    
             outp(COMM2_BASE + COMM_UART_IER, stat);
             if (stat == 0x00) {                           /* �ۼ��� ���ͷ�Ʈ�� ��� ��Ȱ��ȭ�ƴ°�?   */
                                                           /* ��, PC�� IRQ3�� ��Ȱ��ȭ�Ѵ�             */
                 outp(PIC_MSK_REG_PORT, (INT8U)inp(PIC_MSK_REG_PORT) | BIT3);  
             }
             OS_EXIT_CRITICAL();
             break;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                          ���� ���ͷ�Ʈ Ȱ��ȭ
*
* ����        : �� �Լ��� ���� ���ͷ�Ʈ�� Ȱ��ȭ�Ѵ�.
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
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
                                                           /* ���� ���ͷ�Ʈ Ȱ��ȭ                     */
             stat = (INT8U)inp(COMM1_BASE + COMM_UART_IER) | BIT0;     
             outp(COMM1_BASE + COMM_UART_IER, stat);
                                                           /* PC�� IRQ4�� Ȱ��ȭ�Ѵ�                   */
             outp(PIC_MSK_REG_PORT, (INT8U)inp(PIC_MSK_REG_PORT) & ~BIT4);     
             OS_EXIT_CRITICAL();
             break;

        case COMM2:
             OS_ENTER_CRITICAL();
                                                           /* ���� ���ͷ�Ʈ�� Ȱ��ȭ�Ѵ�               */
             stat = (INT8U)inp(COMM2_BASE + COMM_UART_IER) | BIT0;     
             outp(COMM2_BASE + COMM_UART_IER, stat);
                                                           /* PC�� IRQ3�� Ȱ��ȭ�Ѵ�                   */
             outp(PIC_MSK_REG_PORT, (INT8U)inp(PIC_MSK_REG_PORT) & ~BIT3);     
             OS_EXIT_CRITICAL();
             break;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                          ���ͷ�Ʈ ���� ����
*
* ����        : �� �Լ��� �ش� ä�� ��ȣ�� ���ͷ�Ʈ ���͸� �����Ѵ�.
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
*                           COMM1
*                           COMM2
* ��Ʈ        : �� �Լ��� 80x86�� �����忡�� ����Ǵ� ������ �����Ѵ�.
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
*                                         �۽� ���ͷ�Ʈ ��Ȱ��ȭ
*
* ����        : �� �Լ��� ������ �۽��� ��Ȱ��ȭ�Ѵ�.
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
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
                                                           /* �۽� ���ͷ�Ʈ ��Ȱ��ȭ                   */
             stat = (INT8U)inp(COMM1_BASE + COMM_UART_IER) & ~BIT1;    
             outp(COMM1_BASE + COMM_UART_IER, stat);
             if (stat == 0x00) {                           /* �ۼ��� ���ͷ�Ʈ�� ��� ��Ȱ��ȭ�ƴ°�?   */
                 cmd = (INT8U)inp(PIC_MSK_REG_PORT) | BIT4;
                 outp(PIC_MSK_REG_PORT, cmd);              /* ��, PC�� IRQ4�� ��Ȱ��ȭ�Ѵ�             */
             }
             OS_EXIT_CRITICAL();
             break;

        case COMM2:
             OS_ENTER_CRITICAL();
                                                           /* �۽� ���ͷ�Ʈ ��Ȱ��ȭ                   */
             stat = (INT8U)inp(COMM2_BASE + COMM_UART_IER) & ~BIT1;    
             outp(COMM2_BASE + COMM_UART_IER, stat);
             if (stat == 0x00) {                           /* �ۼ��� ���ͷ�Ʈ�� ��� ��Ȱ��ȭ�ƴ°�?   */
                 cmd = (INT8U)inp(PIC_MSK_REG_PORT) | BIT3;
                 outp(PIC_MSK_REG_PORT, cmd);              /* ��, PC�� IRQ3�� ��Ȱ��ȭ�Ѵ�             */
             }
             OS_EXIT_CRITICAL();
             break;
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          �۽� ���ͷ�Ʈ Ȱ��ȭ
*
* ����        : �� �Լ��� ������ �۽��� Ȱ��ȭ�Ѵ�. ������ �۽��� ���ͷ�Ʈ ����̴�.
*               ��Ƽ-��� ����̹��� ����Ѵٸ� �� �Լ��� �۽��� ���� ����̹��� Ȱ��ȭ�ؾ� �Ѵ�.
*
* ��������    : 'ch'    COMM ��Ʈ ä�� ��ȣ. ������ ���� ���� ���´�:
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
             stat = (INT8U)inp(COMM1_BASE + COMM_UART_IER) | BIT1;     /* �۽� ���ͷ�Ʈ Ȱ��ȭ         */
             outp(COMM1_BASE + COMM_UART_IER, stat);
             cmd  = (INT8U)inp(PIC_MSK_REG_PORT) & ~BIT4;
             outp(PIC_MSK_REG_PORT, cmd);                              /* PC�� IRQ4�� Ȱ��ȭ�Ѵ�       */
             OS_EXIT_CRITICAL();
             break;

        case COMM2:
             OS_ENTER_CRITICAL();
             stat = (INT8U)inp(COMM2_BASE + COMM_UART_IER) | BIT1;     /* �۽� ���ͷ�Ʈ Ȱ��ȭ         */
             outp(COMM2_BASE + COMM_UART_IER, stat);
             cmd  = (INT8U)inp(PIC_MSK_REG_PORT) & ~BIT3;
             outp(PIC_MSK_REG_PORT, cmd);                              /* PC�� IRQ3�� Ȱ��ȭ�Ѵ�       */
             OS_EXIT_CRITICAL();
             break;
    }
}
