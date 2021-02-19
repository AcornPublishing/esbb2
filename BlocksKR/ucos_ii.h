/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                              (c) Copyright 1999, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : uCOS_II.H
* By   : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*/

/*$PAGE*/ 
 
/*
*********************************************************************************************************
*                                                 ��Ÿ
*********************************************************************************************************
*/

#define  OS_VERSION              200   /* uC/OS-II ���� (Vx.yy 100�� ���Ѵ�)                           */

#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif

#define  OS_PRIO_SELF           0xFF   /* �ڽ��� �켱������ �˸���                                     */

#if      OS_TASK_STAT_EN
#define  OS_N_SYS_TASKS            2                    /* �ý����� ����ϴ� �½�ũ ��                 */
#else
#define  OS_N_SYS_TASKS            1
#endif

#define  OS_STAT_PRIO       (OS_LOWEST_PRIO - 1)        /* ��� �½�ũ �켱����                        */
#define  OS_IDLE_PRIO       (OS_LOWEST_PRIO)            /* IDLE �½�ũ �켱����                        */

#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO) / 8 + 1)   /* �̺�Ʈ ���̺��� ũ��                        */
#define  OS_RDY_TBL_SIZE   ((OS_LOWEST_PRIO) / 8 + 1)   /* �غ���� ���̺��� ũ��                      */

#define  OS_TASK_IDLE_ID       65535   /* Idle �½�ũ�� ��� �½�ũ I.D. ��ȣ                          */
#define  OS_TASK_STAT_ID       65534

                                       /* �½�ũ ����(OSTCBStat�� ���� ��Ʈ ����)                      */
#define  OS_STAT_RDY            0x00   /* ���� �غ� ����                                               */
#define  OS_STAT_SEM            0x01   /* ������� ��ٸ�                                            */
#define  OS_STAT_MBOX           0x02   /* ���Ϲڽ��� ��ٸ�                                            */
#define  OS_STAT_Q              0x04   /* ť�� ��ٸ�                                                  */
#define  OS_STAT_SUSPEND        0x08   /* �½�ũ ������� ����                                         */

#define  OS_EVENT_TYPE_MBOX        1
#define  OS_EVENT_TYPE_Q           2
#define  OS_EVENT_TYPE_SEM         3

                                       /* �½�ũ �ɼ�(OSTaskCreateExt() ����)                          */
#define  OS_TASK_OPT_STK_CHK  0x0001   /* �½�ũ�� ���� ������ �����Ѵ�                                */
#define  OS_TASK_OPT_STK_CLR  0x0002   /* �½�ũ�� ������ �� ������ Ŭ�����Ѵ�                         */
#define  OS_TASK_OPT_SAVE_FP  0x0004   /* �ε� �Ҽ� ���������� ������ �����Ѵ�                         */


#ifndef  FALSE
#define  FALSE                     0
#endif

#ifndef  TRUE
#define  TRUE                      1
#endif
 
/*
*********************************************************************************************************
*                                              ���� �ڵ�
*********************************************************************************************************
*/

#define OS_NO_ERR                 0
#define OS_ERR_EVENT_TYPE         1
#define OS_ERR_PEND_ISR           2

#define OS_TIMEOUT               10
#define OS_TASK_NOT_EXIST        11

#define OS_MBOX_FULL             20

#define OS_Q_FULL                30

#define OS_PRIO_EXIST            40
#define OS_PRIO_ERR              41
#define OS_PRIO_INVALID          42

#define OS_SEM_OVF               50

#define OS_TASK_DEL_ERR          60
#define OS_TASK_DEL_IDLE         61
#define OS_TASK_DEL_REQ          62
#define OS_TASK_DEL_ISR          63

#define OS_NO_MORE_TCB           70

#define OS_TIME_NOT_DLY          80
#define OS_TIME_INVALID_MINUTES  81
#define OS_TIME_INVALID_SECONDS  82
#define OS_TIME_INVALID_MILLI    83
#define OS_TIME_ZERO_DLY         84

#define OS_TASK_SUSPEND_PRIO     90
#define OS_TASK_SUSPEND_IDLE     91

#define OS_TASK_RESUME_PRIO     100
#define OS_TASK_NOT_SUSPENDED   101

#define OS_MEM_INVALID_PART     110
#define OS_MEM_INVALID_BLKS     111
#define OS_MEM_INVALID_SIZE     112
#define OS_MEM_NO_FREE_BLKS     113
#define OS_MEM_FULL             114

#define OS_TASK_OPT_ERR         130

/*$PAGE*/
 
/*
*********************************************************************************************************
*                                          �̺�Ʈ ��Ʈ�� ���
*********************************************************************************************************
*/

#if (OS_MAX_EVENTS >= 2)
typedef struct {
    void   *OSEventPtr;                    /* �޽��� �Ǵ� ť ����ü�� ���� ������                      */
    INT8U   OSEventTbl[OS_EVENT_TBL_SIZE]; /* �̺�Ʈ�� �߻��ϱ⸦ ��ٸ��� �½�ũ ���                 */
    INT16U  OSEventCnt;                    /* ���������� ��� ���� ī��Ʈ                            */
    INT8U   OSEventType;                   /* OS_EVENT_TYPE_MBOX, OS_EVENT_TYPE_Q, OS_EVENT_TYPE_SEM   */
    INT8U   OSEventGrp;                    /* �̺�Ʈ �߻��� ��ٸ��� �½�ũ �׷�                       */
} OS_EVENT;
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                        �޽��� ���Ϲڽ� ������
*********************************************************************************************************
*/

#if OS_MBOX_EN
typedef struct {
    void   *OSMsg;                         /* ���Ϲڽ��� �޽����� ����Ű�� ������                      */
    INT8U   OSEventTbl[OS_EVENT_TBL_SIZE]; /* �̺�Ʈ�� �߻��ϱ⸦ ��ٸ��� �½�ũ ���                 */
    INT8U   OSEventGrp;                    /* �̺�Ʈ �߻��� ��ٸ��� �½�ũ �׷�                       */
} OS_MBOX_DATA;
#endif

/*
*********************************************************************************************************
*                                     �޸� ��Ƽ�� ������ ����ü
*********************************************************************************************************
*/

#if OS_MEM_EN && (OS_MAX_MEM_PART >= 2)
typedef struct {                       /* �޸� ��Ʈ�� ���                                           */
    void   *OSMemAddr;                 /* �޸� ��Ƽ���� ������ ����Ű�� ������                       */
    void   *OSMemFreeList;             /* �� �޸� ����� ����Ʈ�� ����Ű�� ������                    */
    INT32U  OSMemBlkSize;              /* �� �޸� ����� ����Ʈ ���� ũ��                            */
    INT32U  OSMemNBlks;                /* �� ��Ƽ�ǿ����� ��� ��                                      */
    INT32U  OSMemNFree;                /* �� ��Ƽ�ǿ� �����ִ� �޸� ��� ��                          */
} OS_MEM;


typedef struct {
    void   *OSAddr;                    /* �޸� ��Ƽ���� ���� �ּҸ� ����Ű�� ������                  */
    void   *OSFreeList;                /* �� �޸� ��� ����� ������ ����Ű�� ������                 */
    INT32U  OSBlkSize;                 /* �� �޸� ����� ����Ʈ ���� ũ��                            */
    INT32U  OSNBlks;                   /* �� ��Ƽ�ǿ����� ��ü ��� ��                                 */
    INT32U  OSNFree;                   /* �� �޸� ����� ��                                          */
    INT32U  OSNUsed;                   /* ���� �޸� ����� ��                                      */
} OS_MEM_DATA;
#endif

/*$PAGE*/
 
/*
*********************************************************************************************************
*                                          �޽��� ť ������
*********************************************************************************************************
*/

#if OS_Q_EN
typedef struct {
    void   *OSMsg;                          /* ť���� ���� ���� �޽����� ����Ű�� ������               */
    INT16U  OSNMsgs;                        /* �޽��� ť�� �ִ� �޽��� ��                              */
    INT16U  OSQSize;                        /* �޽��� ���� ũ��                                        */
    INT8U   OSEventTbl[OS_EVENT_TBL_SIZE];  /* �̺�Ʈ�� �߻��ϱ⸦ ��ٸ��� �½�ũ ���                */
    INT8U   OSEventGrp;                     /* �̺�Ʈ �߻��� ��ٸ��� �½�ũ �׷�                      */
} OS_Q_DATA;
#endif

/*
*********************************************************************************************************
*                                           �������� ������
*********************************************************************************************************
*/

#if OS_SEM_EN
typedef struct {
    INT16U  OSCnt;                          /* �������� ī��Ʈ                                         */
    INT8U   OSEventTbl[OS_EVENT_TBL_SIZE];  /* �̺�Ʈ�� �߻��ϱ⸦ ��ٸ��� �½�ũ ���                */
    INT8U   OSEventGrp;                     /* �̺�Ʈ �߻��� ��ٸ��� �½�ũ �׷�                      */
} OS_SEM_DATA;
#endif

/*
*********************************************************************************************************
*                                          �½�ũ ���� ������
*********************************************************************************************************
*/

#if OS_TASK_CREATE_EXT_EN
typedef struct {
    INT32U  OSFree;                    /* ���ÿ��� ������ ���� ����Ʈ ��                             */
    INT32U  OSUsed;                    /* ���ÿ��� ���� ����Ʈ ��                                    */
} OS_STK_DATA;
#endif

/*$PAGE*/
 
/*
*********************************************************************************************************
*                                        �½�ũ ��Ʈ�� ���
*********************************************************************************************************
*/

typedef struct os_tcb {
    OS_STK        *OSTCBStkPtr;        /* ������ �ֻ����� ����Ű�� ������                              */

#if OS_TASK_CREATE_EXT_EN    
    void          *OSTCBExtPtr;        /* TCB Ȯ���� ���� ����� ���� �����Ϳ� ���� ������             */
    OS_STK        *OSTCBStkBottom;     /* ������ �ٴ��� ����Ű�� ������                                */
    INT32U         OSTCBStkSize;       /* �½�ũ ������ ũ��(���� ����� ��)                           */
    INT16U         OSTCBOpt;           /* OSTaskCreateExt()�� ���޵Ǵ� �½�ũ �ɼ�                     */
    INT16U         OSTCBId;            /* �½�ũ ID (0..65535)                                         */
#endif

    struct os_tcb *OSTCBNext;          /* TCB ����Ʈ���� ���� TCB�� ����Ű�� ������                    */
    struct os_tcb *OSTCBPrev;          /* TCB �����׿��� ���� TCB�� ����Ű�� ������                    */

#if (OS_Q_EN && (OS_MAX_QS >= 2)) || OS_MBOX_EN || OS_SEM_EN
    OS_EVENT      *OSTCBEventPtr;      /* �̺�Ʈ ��Ʈ�� ����� ����Ű�� ������                         */
#endif

#if (OS_Q_EN && (OS_MAX_QS >= 2)) || OS_MBOX_EN
    void          *OSTCBMsg;           /* OSMboxPost() �Ǵ� OSQPost()�κ��� ���ŵ� �޽���              */
#endif    

    INT16U         OSTCBDly;           /* �½�ũ ���� �Ǵ� ��� ���� �̺�Ʈ�� ���� Ÿ�Ӿƿ�            */
    INT8U          OSTCBStat;          /* �½�ũ ����                                                  */
    INT8U          OSTCBPrio;          /* �½�ũ �켱���� (0 == ���� ����, 63 == ���� ����)            */

    INT8U          OSTCBX;             /* �½�ũ �켱������ ���õ� �׷��� ��Ʈ ��ġ(0..7)              */
    INT8U          OSTCBY;             /* �½�ũ �켱������ ���õ� �غ� ���̺� �ε���                  */
    INT8U          OSTCBBitX;          /* �غ� ���̺��� �׼����ϱ� ���� ��Ʈ ����ũ                    */
    INT8U          OSTCBBitY;          /* �غ� �׷� �׼��� ��Ʈ�� ����ũ �ϴ� ��Ʈ                     */
    
#if OS_TASK_DEL_EN    
    BOOLEAN        OSTCBDelReq;        /* �½�ũ �ڽ��� ���� ������ �˷��ش�                           */
#endif
} OS_TCB;

/*$PAGE*/
 
/*
*********************************************************************************************************
*                                              ���� ����
*********************************************************************************************************
*/
                                       
OS_EXT  INT32U       OSCtxSwCtr;               /* ���ؽ�Ʈ ����ġ�� �߻��� Ƚ���� ���� ī����          */

#if     (OS_MAX_EVENTS >= 2)
OS_EXT  OS_EVENT    *OSEventFreeList;          /* �� �̺�Ʈ ��Ʈ�� ����� ����Ʈ�� ����Ű�� ������     */
OS_EXT  OS_EVENT     OSEventTbl[OS_MAX_EVENTS];/* �̺�Ʈ ��Ʈ�� ��� ���̺�                            */
#endif

OS_EXT  INT32U       OSIdleCtr;                /* Idle ī����                                          */

#if     OS_TASK_STAT_EN
OS_EXT  INT8S        OSCPUUsage;               /* �ۼ�Ʈ ������ CPU �����                             */
OS_EXT  INT32U       OSIdleCtrMax;             /* 1�� ���� �� �� �ִ� idel ī������ �ִ� ��            */
OS_EXT  INT32U       OSIdleCtrRun;             /* 1���� ��Ÿ�� ���� ������ idel ī���� ��              */
OS_EXT  BOOLEAN      OSStatRdy;                /* ��� �½�ũ�� �غ�Ǿ����� �˷��ִ� ������           */
#endif

OS_EXT  INT8U        OSIntNesting;             /* ���ͷ�Ʈ ��ø ����                                   */

OS_EXT  INT8U        OSLockNesting;            /* ��Ƽ�½�ŷ ��� ��ø ����                            */

OS_EXT  INT8U        OSPrioCur;                /* ���� �½�ũ �켱����                                 */
OS_EXT  INT8U        OSPrioHighRdy;            /* �ֿ켱 �½�ũ �켱����                               */

OS_EXT  INT8U        OSRdyGrp;                        /* �غ� ����Ʈ �׷�                              */
OS_EXT  INT8U        OSRdyTbl[OS_RDY_TBL_SIZE];       /* ���� ������ �½�ũ ���̺�                     */

OS_EXT  BOOLEAN      OSRunning;                       /* Ŀ���� ����ǰ� ������ �˷��ִ� ������        */

#if     OS_TASK_CREATE_EN  || OS_TASK_CREATE_EXT_EN || OS_TASK_DEL_EN
OS_EXT  INT8U        OSTaskCtr;                       /* ������ �½�ũ ��                              */
#endif

OS_EXT  OS_TCB      *OSTCBCur;                        /* ���� ����Ǵ� TCB�� ����Ű�� ������           */
OS_EXT  OS_TCB      *OSTCBFreeList;                   /* �� TCB ����Ʈ�� ����Ű�� ������               */
OS_EXT  OS_TCB      *OSTCBHighRdy;                    /* ���� ������ �ֿ켱 ������ TCB�� ���� ������   */
OS_EXT  OS_TCB      *OSTCBList;                       /* TCB�� ���� ��ũ�� ����Ʈ�� ���� ������        */
OS_EXT  OS_TCB      *OSTCBPrioTbl[OS_LOWEST_PRIO + 1];/* ������ TCB�� ����Ű�� ������ ���̺�           */

OS_EXT  INT32U       OSTime;                   /* ƽ ���� �ý��� Ÿ���� ���簪                         */


extern  INT8U const  OSMapTbl[8];              /* �켱����->��Ʈ ����ũ ��ȯ ���̺�                    */
extern  INT8U const  OSUnMapTbl[256];          /* �켱����->�ε��� ��ȯ ���̺�                         */

/*$PAGE*/
 
/*
*********************************************************************************************************
*                                              �Լ� ����
*                                        (Ÿ�� �������� �Լ�)
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        �޽��� ���Ϲڽ� ����
*********************************************************************************************************
*/
#if         OS_MBOX_EN
void       *OSMboxAccept(OS_EVENT *pevent);
OS_EVENT   *OSMboxCreate(void *msg);
void       *OSMboxPend(OS_EVENT *pevent, INT16U timeout, INT8U *err);
INT8U       OSMboxPost(OS_EVENT *pevent, void *msg);
INT8U       OSMboxQuery(OS_EVENT *pevent, OS_MBOX_DATA *pdata);
#endif
/*
*********************************************************************************************************
*                                             �޸� ����
*********************************************************************************************************
*/
#if         OS_MEM_EN && (OS_MAX_MEM_PART >= 2)
OS_MEM     *OSMemCreate(void *addr, INT32U nblks, INT32U blksize, INT8U *err);
void       *OSMemGet(OS_MEM *pmem, INT8U *err);
INT8U       OSMemPut(OS_MEM *pmem, void *pblk);
INT8U       OSMemQuery(OS_MEM *pmem, OS_MEM_DATA *pdata);
#endif
/*
*********************************************************************************************************
*                                            �޽��� ť ����
*********************************************************************************************************
*/
#if         OS_Q_EN && (OS_MAX_QS >= 2)
void       *OSQAccept(OS_EVENT *pevent);
OS_EVENT   *OSQCreate(void **start, INT16U size);
INT8U       OSQFlush(OS_EVENT *pevent);
void       *OSQPend(OS_EVENT *pevent, INT16U timeout, INT8U *err);
INT8U       OSQPost(OS_EVENT *pevent, void *msg);
INT8U       OSQPostFront(OS_EVENT *pevent, void *msg);
INT8U       OSQQuery(OS_EVENT *pevent, OS_Q_DATA *pdata);
#endif
/*$PAGE*/
 
/*
*********************************************************************************************************
*                                            �������� ����
*********************************************************************************************************
*/
#if         OS_SEM_EN
INT16U      OSSemAccept(OS_EVENT *pevent);
OS_EVENT   *OSSemCreate(INT16U value);
void        OSSemPend(OS_EVENT *pevent, INT16U timeout, INT8U *err);
INT8U       OSSemPost(OS_EVENT *pevent);
INT8U       OSSemQuery(OS_EVENT *pevent, OS_SEM_DATA *pdata);
#endif
/*
*********************************************************************************************************
*                                             �½�ũ ����
*********************************************************************************************************
*/
#if         OS_TASK_CHANGE_PRIO_EN
INT8U       OSTaskChangePrio(INT8U oldprio, INT8U newprio);
#endif

INT8U       OSTaskCreate(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT8U prio);

#if         OS_TASK_CREATE_EXT_EN
INT8U       OSTaskCreateExt(void  (*task)(void *pd), 
                            void   *pdata, 
                            OS_STK *ptos, 
                            INT8U   prio,
                            INT16U  id,
                            OS_STK *pbos,
                            INT32U  stk_size,
                            void   *pext,
                            INT16U  opt);
#endif                            

#if         OS_TASK_DEL_EN
INT8U       OSTaskDel(INT8U prio);
INT8U       OSTaskDelReq(INT8U prio);
#endif

#if         OS_TASK_SUSPEND_EN
INT8U       OSTaskResume(INT8U prio);
INT8U       OSTaskSuspend(INT8U prio);
#endif

#if         OS_TASK_CREATE_EXT_EN
INT8U       OSTaskStkChk(INT8U prio, OS_STK_DATA *pdata);
#endif

INT8U       OSTaskQuery(INT8U prio, OS_TCB *pdata);
 
/*
*********************************************************************************************************
*                                              �ð� ����
*********************************************************************************************************
*/
void        OSTimeDly(INT16U ticks);
INT8U       OSTimeDlyHMSM(INT8U hours, INT8U minutes, INT8U seconds, INT16U milli);
INT8U       OSTimeDlyResume(INT8U prio);
INT32U      OSTimeGet(void);
void        OSTimeSet(INT32U ticks);
void        OSTimeTick(void);

/*
*********************************************************************************************************
*                                                  ��Ÿ
*********************************************************************************************************
*/

void        OSInit(void);

void        OSIntEnter(void);
void        OSIntExit(void);

void        OSSchedLock(void);
void        OSSchedUnlock(void);

void        OSStart(void);

void        OSStatInit(void);

INT16U      OSVersion(void);

/*$PAGE*/
 
/*
*********************************************************************************************************
*                                           ���� �Լ� ����
*                             (�������α׷��� �� �Լ��� ȣ���� �� ����)
*********************************************************************************************************
*/

#if         OS_MBOX_EN || OS_Q_EN || OS_SEM_EN
void        OSEventTaskRdy(OS_EVENT *pevent, void *msg, INT8U msk);
void        OSEventTaskWait(OS_EVENT *pevent);
void        OSEventTO(OS_EVENT *pevent);
void        OSEventWaitListInit(OS_EVENT *pevent);
#endif

#if         OS_MEM_EN && (OS_MAX_MEM_PART >= 2)
void        OSMemInit(void);
#endif

#if         OS_Q_EN
void        OSQInit(void);
#endif

void        OSSched(void);

void        OSTaskIdle(void *data);

#if         OS_TASK_STAT_EN
void        OSTaskStat(void *data);
#endif

INT8U       OSTCBInit(INT8U prio, OS_STK *ptos, OS_STK *pbos, INT16U id, INT32U stk_size, void *pext, INT16U opt);

/*$PAGE*/
 
/*
*********************************************************************************************************
*                                              �Լ� ����
*                                        (Ÿ�� �������� �Լ�)
*********************************************************************************************************
*/

void        OSCtxSw(void);

void        OSIntCtxSw(void);

void        OSStartHighRdy(void);

void        OSTaskCreateHook(OS_TCB *ptcb);
void        OSTaskDelHook(OS_TCB *ptcb);
void        OSTaskStatHook(void);
OS_STK     *OSTaskStkInit(void (*task)(void *pd), void *pdata, OS_STK *ptos, INT16U opt);
void        OSTaskSwHook(void);

void        OSTickISR(void);

void        OSTimeTickHook(void);
