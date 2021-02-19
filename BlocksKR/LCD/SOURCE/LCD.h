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
*                                                  ���
*********************************************************************************************************
*/

#ifndef  CFG_H
#define  DISP_DLY_CNTS              8       /* 40uS �����̿� ���� �ݺ� Ƚ��(����Ʈ���� ����)         */

#define  DISP_PORT_CMD         0x0303       /* RS�� E�� �����ϴ� ��Ʈ�� ������ �ּ�                    */
#define  DISP_PORT_DATA        0x0300       /* LCD ����� ������ ��Ʈ �ּ�                             */
#endif

#define  DISP_SEL_CMD_REG           0
#define  DISP_SEL_DATA_REG          1

/*
*********************************************************************************************************
*                                                 �Լ�����
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
*                                         �ϵ���� ���� �Լ� ����
*
*********************************************************************************************************
*/

void  DispDataWr(INT8U data);
void  DispInitPort(void);
void  DispSel(INT8U sel);
