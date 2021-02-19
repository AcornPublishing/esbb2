/*
*********************************************************************************************************
*                                    Embedded Systems Building Blocks
*                                 Complete and Ready-to-Use Modules in C
*
*                                     Multiplexed LED Display Driver
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* Filename   : LED.H
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 ���
*********************************************************************************************************
*/

#ifndef  CFG_H
#define  DISP_PORT_DIG   0x0301             /* DIGITS ��¿� ��Ʈ ��巹��                             */
#define  DISP_PORT_SEG   0x0300             /* SEGMENTS ��¿� ��Ʈ ��巹��                           */

#define  DISP_N_DIG           8             /* ���� ǥ�ñ⸦ ������ �� ������ ����                     */
#define  DISP_N_SS            7             /* 7-���׸�Ʈ�� �� ����                                    */
#endif

/*
*********************************************************************************************************
*                                               �Լ� ����
*********************************************************************************************************
*/

void  DispClrScr(void);
void  DispInit(void);
void  DispMuxHandler(void);
void  DispMuxISR(void);
void  DispStr(INT8U dig, char *s);
void  DispStatClr(INT8U dig, INT8U bit);
void  DispStatSet(INT8U dig, INT8U bit);

/*
*********************************************************************************************************
*                                         �ϵ���� ���� �Լ� ����
*
*********************************************************************************************************
*/

void  DispInitPort(void);
void  DispOutDig(INT8U msk);
void  DispOutSeg(INT8U seg);
