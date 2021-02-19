/*
*********************************************************************************************************
*                                               uC/OS-II
*                                        The Real-Time Kernel
*
*                        (c) Copyright 1992-1999, Jean J. Labrosse, Weston, FL
*                                          All Rights Reserved
*
*                                       80x86/80x88 Specific code
*                                          LARGE MEMORY MODEL
*
*                                          Borland C/C++ V4.51
*
* File         : OS_CPU.H
* By           : Jean J. Labrosse
* Port Version : V1.00
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*/

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT  extern
#endif

/*
*********************************************************************************************************
*                                             데이터 타입
*                                         (컴파일러에 의존적임)
*********************************************************************************************************
*/

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* 무부호형 8비트 값                                  */
typedef signed   char  INT8S;                    /* 부호형 8비트 값                                    */
typedef unsigned int   INT16U;                   /* 무부호형 16비트 값                                 */
typedef signed   int   INT16S;                   /* 부호형 16비트 값                                   */
typedef unsigned long  INT32U;                   /* 무부호형 32비트 값                                 */
typedef signed   long  INT32S;                   /* 부호형 32비트 값                                   */
typedef float          FP32;                     /* 단정도 부동 소수                                   */
typedef double         FP64;                     /* 배정도 부동 소수                                   */

typedef unsigned int   OS_STK;                   /* 스택의 각 항목은 16비트 값이다                     */

#define BYTE           INT8S                     /* uC/OS V1.xx로의 역호환성을 위한 데이터 타입을 ..   */
#define UBYTE          INT8U                     /* .. 정의한다. uCOS-II에서는 실제 필요하지 않다 ..   */
#define WORD           INT16S
#define UWORD          INT16U
#define LONG           INT32S
#define ULONG          INT32U
 
/* 
*********************************************************************************************************
*                                Intel 80x86 (리얼 모드, 라지 모델)
*
* 방법 #1:  간단한 명령어를 사용해서 인터럽트를 활성화/비활성화 한다. 크리티컬 섹션을 진입할 때
*           인터럽트를 비활성화 했더라도 크리티컬 섹션을 수행한 후 인터럽트는 활성화 된다.
*           OS_CPU_A.ASM에서 OSIntCtxSw() 함수의 상수를 10에서 8로 변경해야 한다.
*
* 방법 #2:  인터럽트의 상태를 보존해서 활성화/비활성화 한다. 즉, 크리티컬 섹션을 진입할 때 인터럽트가
*           비활성화 됐다면, 크리티컬 섹션을 떠날 때 비활성화 된다.
*           OS_CPU_A.ASM에서 OSIntCtxSw() 함수의 상수를 10에서 8로 변경해야 한다.
*********************************************************************************************************
*/
#define  OS_CRITICAL_METHOD    2

#if      OS_CRITICAL_METHOD == 1
#define  OS_ENTER_CRITICAL()  asm  CLI                /* 인터럽트 비활성화                             */
#define  OS_EXIT_CRITICAL()   asm  STI                /* 인터럽트 활성화                               */
#endif

#if      OS_CRITICAL_METHOD == 2
#define  OS_ENTER_CRITICAL()  asm {PUSHF; CLI}        /* 인터럽트 비활성화                             */
#define  OS_EXIT_CRITICAL()   asm  POPF               /* 인터럽트 활성화                               */
#endif

/*
*********************************************************************************************************
*                           Intel 80x86 (리얼 모드, 라지 모델) 기타
*********************************************************************************************************
*/

#define  OS_STK_GROWTH        1                       /* 스택이 메모리 위에서 아래로 커진다            */

#define  uCOS                 0x80                    /* 컨텍스트 스위치를 수행하는 인터럽트 벡터 번호 */

#define  OS_TASK_SW()         asm  INT   uCOS

/*
*********************************************************************************************************
*                                              전역 변수
*********************************************************************************************************
*/

OS_CPU_EXT  INT8U  OSTickDOSCtr;   /* 매 'n'번의 틱 마다 DOS 틱 핸들러를 호출하기 위해 사용되는 카운터 */

/*
*********************************************************************************************************
*                                              함수 원형
*********************************************************************************************************
*/

void  OSFPInit(void);
void  OSFPRestore(void *pblk);
void  OSFPSave(void *pblk);
 

