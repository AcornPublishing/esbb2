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
*                                             uC/OS-II ����
*********************************************************************************************************
*/

#define OS_MAX_EVENTS            10    /* �������α׷������� �ִ� �̺�Ʈ ��Ʈ�� ��� �� ...            */
                                       /* ... 2�� ���ų� Ŀ����                                        */
#define OS_MAX_MEM_PART           2    /* �ִ� �޸� ���� �� ...                                      */
                                       /* ... 2�� ���ų� Ŀ����                                        */
#define OS_MAX_QS                 2    /* �������α׷������� �ִ� ť ���� ��� ��...                   */
                                       /* ... 2�� ���ų� Ŀ����                                        */
#define OS_MAX_TASKS             15    /* �������α׷������� �ִ� �½�ũ �� ...                        */
                                       /* ... 2�� ���ų� Ŀ����                                        */

#define OS_LOWEST_PRIO           63    /* ������ �� �ִ� ������ �켱���� ...                           */
                                       /* ... 63���� ũ�� �ȵ�!                                        */

#define OS_TASK_IDLE_STK_SIZE   512    /* Idle �½�ũ ���� ũ��(16��Ʈ ��Ʈ���� ��)                    */

#define OS_TASK_STAT_EN           1    /* ��� �½�ũ Ȱ��ȭ(1),  ��Ȱ��ȭ(0)                          */
#define OS_TASK_STAT_STK_SIZE   512    /* ��� �½�ũ�� ���� ũ��(16��Ʈ ��Ʈ���� ��)                  */

#define OS_CPU_HOOKS_EN           1    /* uC/OS-II ���μ��� �̽�(port) ���Ͽ��� ����                 */
#define OS_MBOX_EN                0    /* MAILBOXES �ڵ� ����                                          */
#define OS_MEM_EN                 0    /* MEMORY MANAGER (���� ũ�� �޸� ���)                       */
#define OS_Q_EN                   0    /* QUEUES �ڵ� ����                                             */
#define OS_SEM_EN                 1    /* SEMAPHORES �ڵ� ����                                         */
#define OS_TASK_CHANGE_PRIO_EN    0    /* OSTaskChangePrio() �ڵ� ����                                 */
#define OS_TASK_CREATE_EN         1    /* OSTaskCreate() �ڵ� ����                                     */
#define OS_TASK_CREATE_EXT_EN     1    /* OSTaskCreateExt() �ڵ� ����                                  */
#define OS_TASK_DEL_EN            0    /* OSTaskDel() �ڵ� ����                                        */
#define OS_TASK_SUSPEND_EN        0    /* OSTaskSuspend()��  OSTaskResume() �ڵ� ����                  */

#define OS_TICKS_PER_SEC        200    /* 1�ʰ��� Ŭ�� ƽ ����                                         */

