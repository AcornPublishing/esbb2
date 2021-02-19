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
*                                                 상수
*********************************************************************************************************
*/

#ifndef  CFG_H
#define  DISP_PORT_DIG   0x0301             /* DIGITS 출력용 포트 어드레스                             */
#define  DISP_PORT_SEG   0x0300             /* SEGMENTS 출력용 포트 어드레스                           */

#define  DISP_N_DIG           8             /* 상태 표시기를 포함한 총 숫자의 갯수                     */
#define  DISP_N_SS            7             /* 7-세그먼트의 총 갯수                                    */
#endif

/*
*********************************************************************************************************
*                                               함수 원형
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
*                                         하드웨어 관련 함수 원형
*
*********************************************************************************************************
*/

void  DispInitPort(void);
void  DispOutDig(INT8U msk);
void  DispOutSeg(INT8U seg);
