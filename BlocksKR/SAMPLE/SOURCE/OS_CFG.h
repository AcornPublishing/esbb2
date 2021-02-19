/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                         (c) Copyright 1992-1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                   Configuration for Intel 80x86 (Large)
*
* File : OS_CFG.H
* By   : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             uC/OS-II 설정
*********************************************************************************************************
*/

#define OS_MAX_EVENTS            10    /* 응용프로그램에서의 최대 이벤트 컨트롤 블록 수 ...            */
                                       /* ... 2와 같거나 커야함                                        */
#define OS_MAX_MEM_PART           2    /* 최대 메모리 분할 수 ...                                      */
                                       /* ... 2와 같거나 커야함                                        */
#define OS_MAX_QS                 2    /* 응용프로그램에서의 최대 큐 제어 블록 수...                   */
                                       /* ... 2와 같거나 커야함                                        */
#define OS_MAX_TASKS             15    /* 응용프로그램에서의 최대 태스크 수 ...                        */
                                       /* ... 2와 같거나 커야함                                        */

#define OS_LOWEST_PRIO           63    /* 배정될 수 있는 최하위 우선순위 ...                           */
                                       /* ... 63보다 크면 안됨!                                        */

#define OS_TASK_IDLE_STK_SIZE   512    /* Idle 태스크 스택 크기(16비트 엔트리의 수)                    */

#define OS_TASK_STAT_EN           1    /* 통계 태스크 활성화(1),  비활성화(0)                          */
#define OS_TASK_STAT_STK_SIZE   512    /* 통계 태스크의 스택 크기(16비트 엔트리의 수)                  */

#define OS_CPU_HOOKS_EN           1    /* uC/OS-II 프로세서 이식(port) 파일에서 사용됨                 */
#define OS_MBOX_EN                0    /* MAILBOXES 코드 생성                                          */
#define OS_MEM_EN                 0    /* MEMORY MANAGER (고정 크기 메모리 블록)                       */
#define OS_Q_EN                   0    /* QUEUES 코드 생성                                             */
#define OS_SEM_EN                 1    /* SEMAPHORES 코드 생성                                         */
#define OS_TASK_CHANGE_PRIO_EN    0    /* OSTaskChangePrio() 코드 생성                                 */
#define OS_TASK_CREATE_EN         1    /* OSTaskCreate() 코드 생성                                     */
#define OS_TASK_CREATE_EXT_EN     1    /* OSTaskCreateExt() 코드 생성                                  */
#define OS_TASK_DEL_EN            0    /* OSTaskDel() 코드 생성                                        */
#define OS_TASK_SUSPEND_EN        0    /* OSTaskSuspend()와  OSTaskResume() 코드 생성                  */

#define OS_TICKS_PER_SEC        200    /* 1초간의 클럭 틱 설정                                         */

