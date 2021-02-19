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
*                                                  상수
*********************************************************************************************************
*/

                                       /* 모듈 활성화(1) , 비활성화(0)                                 */
#define   MODULE_KEY_MN    0           /* 키보드 모듈                                                  */
#define   MODULE_LED       0           /* 멀티플렉스드 LED 모듈                                        */
#define   MODULE_LCD       0           /* LCD 문자 모듈                                                */
#define   MODULE_CLK       1           /* 시계/달력 모듈                                               */
#define   MODULE_TMR       1           /* 타이머 관리자 모듈                                           */
#define   MODULE_DIO       1           /* 분산 I/O 모듈                                                */
#define   MODULE_AIO       1           /* 아날로그 I/O 모듈                                            */
#define   MODULE_COMM_PC   1           /* 비동기 시리얼 통신 모듈                                      */
#define   MODULE_COMM_BGND 0           /*    전경/배경 시리얼 I/O                                      */
#define   MODULE_COMM_RTOS 1           /*    리얼타임 커널 시리얼 I/O                                  */

#define   CFG_C                        /* 응용프로그램 특정 코드가 CFG.C에 있음을 명시                 */
#define   CFG_H                        /* 설정 #define 매크로가 CFG.H에 있음을 명시                    */

/*
*********************************************************************************************************
*                                                  상수
*********************************************************************************************************
*/

#define  FALSE    0
#define  TRUE     1

/*$PAGE*/
/*
*********************************************************************************************************
*                                           표준 라이브러리(DOS)
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
*                                             uC/OS 헤더 파일
*********************************************************************************************************
*/

#include    "\software\ucos-ii\ix86l-fp\bc45\os_cpu.h"
#include    "\software\blocks\sample\source\os_cfg.h"
#include    "\software\ucos-ii\source\ucos_ii.h"
#include    "\software\blocks\pc\bc45\pc.h"

/*$PAGE*/
/*
*********************************************************************************************************
*                                           빌딩 블록 헤더 파일
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
