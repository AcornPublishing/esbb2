/*
*********************************************************************************************************
*                                    Embedded Systems Building Blocks
*                                 Complete and Ready-to-Use Modules in C
*
*                                           Master Include File
*
*                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* Filename   : INCLUDES.H
* Programmer : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                  ���
*********************************************************************************************************
*/

                                       /* ��� Ȱ��ȭ(1) , ��Ȱ��ȭ(0)                                 */
#define   MODULE_KEY_MN    0           /* Ű���� ���                                                  */
#define   MODULE_LED       0           /* ��Ƽ�÷����� LED ���                                        */
#define   MODULE_LCD       0           /* LCD ���� ���                                                */
#define   MODULE_CLK       1           /* �ð�/�޷� ���                                               */
#define   MODULE_TMR       1           /* Ÿ�̸� ������ ���                                           */
#define   MODULE_DIO       1           /* �л� I/O ���                                                */
#define   MODULE_AIO       1           /* �Ƴ��α� I/O ���                                            */
#define   MODULE_COMM_PC   1           /* �񵿱� �ø��� ��� ���                                      */
#define   MODULE_COMM_BGND 0           /*    ����/��� �ø��� I/O                                      */
#define   MODULE_COMM_RTOS 1           /*    ����Ÿ�� Ŀ�� �ø��� I/O                                  */

#define   CFG_C                        /* �������α׷� Ư�� �ڵ尡 CFG.C�� ������ ���                 */
#define   CFG_H                        /* ���� #define ��ũ�ΰ� CFG.H�� ������ ���                    */

/*
*********************************************************************************************************
*                                                  ���
*********************************************************************************************************
*/

#define  FALSE    0
#define  TRUE     1

/*$PAGE*/
/*
*********************************************************************************************************
*                                           ǥ�� ���̺귯��(DOS)
*********************************************************************************************************
*/

#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <conio.h>
#include    <dos.h>
#include    <setjmp.h>

/*
*********************************************************************************************************
*                                             uC/OS ��� ����
*********************************************************************************************************
*/

#include    "\software\ucos-ii\ix86l-fp\bc45\os_cpu.h"
#include    "\software\blocks\sample\source\os_cfg.h"
#include    "\software\ucos-ii\source\ucos_ii.h"
#include    "\software\blocks\pc\bc45\pc.h"

/*$PAGE*/
/*
*********************************************************************************************************
*                                           ���� ��� ��� ����
*********************************************************************************************************
*/
#ifdef      CFG_H
#include    "\software\blocks\sample\source\cfg.h"
#endif

#if         MODULE_KEY_MN
#include    "\software\blocks\key_mn\source\key.h"
#endif

#if         MODULE_LCD
#include    "\software\blocks\lcd\source\lcd.h"
#endif

#if         MODULE_LED
#include    "\software\blocks\led\source\led.h"
#endif

#if         MODULE_CLK
#include    "\software\blocks\clk\source\clk.h"
#endif

#if         MODULE_TMR
#include    "\software\blocks\tmr\source\tmr.h"
#endif

#if         MODULE_DIO
#include    "\software\blocks\dio\source\dio.h"
#endif

#if         MODULE_AIO
#include    "\software\blocks\aio\source\aio.h"
#endif

#if         MODULE_COMM_PC
#include    "\software\blocks\comm\source\comm_pc.h"
#endif

#if         MODULE_COMM_BGND
#include    "\software\blocks\comm\source\commbgnd.h"
#endif

#if         MODULE_COMM_RTOS
#include    "\software\blocks\comm\source\commrtos.h"
#endif
