/*
*********************************************************************************************************
*                                   Embedded Systems Building Blocks
*                                Complete and Ready-to-Use Modules in C
*
*                                        LCD Display Module Driver
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* Filename   : LCD.H
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                  상수
*********************************************************************************************************
*/

#ifndef  CFG_H
#define  DISP_DLY_CNTS              8       /* 40uS 딜레이에 사용될 반복 횟수(소프트웨어 루프)         */

#define  DISP_PORT_CMD         0x0303       /* RS와 E를 제어하는 컨트롤 워드의 주소                    */
#define  DISP_PORT_DATA        0x0300       /* LCD 모듈의 데이터 포트 주소                             */
#endif

#define  DISP_SEL_CMD_REG           0
#define  DISP_SEL_DATA_REG          1

/*
*********************************************************************************************************
*                                                 함수원형
*********************************************************************************************************
*/

void  DispChar(INT8U row, INT8U col, char c);
void  DispClrLine(INT8U line);
void  DispClrScr(void);
void  DispDefChar(INT8U id, INT8U *pat);
void  DispDummy(void);
void  DispHorBar(INT8U row, INT8U col, INT8U val);
void  DispHorBarInit(void);
void  DispInit(INT8U maxrows, INT8U maxcols);
void  DispStr(INT8U row, INT8U col, char *s);

/*
*********************************************************************************************************
*                                         하드웨어 관련 함수 원형
*
*********************************************************************************************************
*/

void  DispDataWr(INT8U data);
void  DispInitPort(void);
void  DispSel(INT8U sel);
