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
*                                                 기타
*********************************************************************************************************
*/

#define  OS_VERSION              200   /* uC/OS-II 버전 (Vx.yy 100을 곱한다)                           */

#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif

#define  OS_PRIO_SELF           0xFF   /* 자신의 우선순위를 알린다                                     */

#if      OS_TASK_STAT_EN
#define  OS_N_SYS_TASKS            2                    /* 시스템이 사용하는 태스크 수                 */
#else
#define  OS_N_SYS_TASKS            1
#endif

#define  OS_STAT_PRIO       (OS_LOWEST_PRIO - 1)        /* 통계 태스크 우선순위                        */
#define  OS_IDLE_PRIO       (OS_LOWEST_PRIO)            /* IDLE 태스크 우선순위                        */

#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO) / 8 + 1)   /* 이벤트 테이블의 크기                        */
#define  OS_RDY_TBL_SIZE   ((OS_LOWEST_PRIO) / 8 + 1)   /* 준비상태 테이블의 크기                      */

#define  OS_TASK_IDLE_ID       65535   /* Idle 태스크와 통계 태스크 I.D. 번호                          */
#define  OS_TASK_STAT_ID       65534

                                       /* 태스크 상태(OSTCBStat에 대한 비트 정의)                      */
#define  OS_STAT_RDY            0x00   /* 실행 준비 상태                                               */
#define  OS_STAT_SEM            0x01   /* 세마포어를 기다림                                            */
#define  OS_STAT_MBOX           0x02   /* 메일박스를 기다림                                            */
#define  OS_STAT_Q              0x04   /* 큐를 기다림                                                  */
#define  OS_STAT_SUSPEND        0x08   /* 태스크 서스펜드 상태                                         */

#define  OS_EVENT_TYPE_MBOX        1
#define  OS_EVENT_TYPE_Q           2
#define  OS_EVENT_TYPE_SEM         3

                                       /* 태스크 옵션(OSTaskCreateExt() 참조)                          */
#define  OS_TASK_OPT_STK_CHK  0x0001   /* 태스크에 대한 스택을 감시한다                                */
#define  OS_TASK_OPT_STK_CLR  0x0002   /* 태스크를 생성할 때 스택을 클리어한다                         */
#define  OS_TASK_OPT_SAVE_FP  0x0004   /* 부동 소수 레지스터의 내용을 저장한다                         */


#ifndef  FALSE
#define  FALSE                     0
#endif

#ifndef  TRUE
#define  TRUE                      1
#endif
 
/*
*********************************************************************************************************
*                                              에러 코드
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
*                                          이벤트 컨트롤 블록
*********************************************************************************************************
*/

#if (OS_MAX_EVENTS >= 2)
typedef struct {
    void   *OSEventPtr;                    /* 메시지 또는 큐 구조체에 대한 포인터                      */
    INT8U   OSEventTbl[OS_EVENT_TBL_SIZE]; /* 이벤트가 발생하기를 기다리는 태스크 목록                 */
    INT16U  OSEventCnt;                    /* 세마포어인 경우 사용된 카운트                            */
    INT8U   OSEventType;                   /* OS_EVENT_TYPE_MBOX, OS_EVENT_TYPE_Q, OS_EVENT_TYPE_SEM   */
    INT8U   OSEventGrp;                    /* 이벤트 발생을 기다리는 태스크 그룹                       */
} OS_EVENT;
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                        메시지 메일박스 데이터
*********************************************************************************************************
*/

#if OS_MBOX_EN
typedef struct {
    void   *OSMsg;                         /* 메일박스의 메시지를 가리키는 포인터                      */
    INT8U   OSEventTbl[OS_EVENT_TBL_SIZE]; /* 이벤트가 발생하기를 기다리는 태스크 목록                 */
    INT8U   OSEventGrp;                    /* 이벤트 발생을 기다리는 태스크 그룹                       */
} OS_MBOX_DATA;
#endif

/*
*********************************************************************************************************
*                                     메모리 파티션 데이터 구조체
*********************************************************************************************************
*/

#if OS_MEM_EN && (OS_MAX_MEM_PART >= 2)
typedef struct {                       /* 메모리 컨트롤 블록                                           */
    void   *OSMemAddr;                 /* 메모리 파티션의 시작을 가리키는 포인터                       */
    void   *OSMemFreeList;             /* 빈 메모리 블록의 리스트를 가리키는 포인터                    */
    INT32U  OSMemBlkSize;              /* 각 메모리 블록의 바이트 단위 크기                            */
    INT32U  OSMemNBlks;                /* 이 파티션에서의 블록 수                                      */
    INT32U  OSMemNFree;                /* 이 파티션에 남아있는 메모리 블록 수                          */
} OS_MEM;


typedef struct {
    void   *OSAddr;                    /* 메모리 파티션의 시작 주소를 가리키는 포인터                  */
    void   *OSFreeList;                /* 빈 메모리 블록 목록의 시작을 가리키는 포인터                 */
    INT32U  OSBlkSize;                 /* 각 메모리 블록의 바이트 단위 크기                            */
    INT32U  OSNBlks;                   /* 이 파티션에서의 전체 블록 수                                 */
    INT32U  OSNFree;                   /* 빈 메모리 블록의 수                                          */
    INT32U  OSNUsed;                   /* 사용된 메모리 블록의 수                                      */
} OS_MEM_DATA;
#endif

/*$PAGE*/
 
/*
*********************************************************************************************************
*                                          메시지 큐 데이터
*********************************************************************************************************
*/

#if OS_Q_EN
typedef struct {
    void   *OSMsg;                          /* 큐에서 빼낼 다음 메시지를 가리키는 포인터               */
    INT16U  OSNMsgs;                        /* 메시지 큐에 있는 메시지 수                              */
    INT16U  OSQSize;                        /* 메시지 규의 크기                                        */
    INT8U   OSEventTbl[OS_EVENT_TBL_SIZE];  /* 이벤트가 발생하기를 기다리는 태스크 목록                */
    INT8U   OSEventGrp;                     /* 이벤트 발생을 기다리는 태스크 그룹                      */
} OS_Q_DATA;
#endif

/*
*********************************************************************************************************
*                                           세마포어 데이터
*********************************************************************************************************
*/

#if OS_SEM_EN
typedef struct {
    INT16U  OSCnt;                          /* 세마포어 카운트                                         */
    INT8U   OSEventTbl[OS_EVENT_TBL_SIZE];  /* 이벤트가 발생하기를 기다리는 태스크 목록                */
    INT8U   OSEventGrp;                     /* 이벤트 발생을 기다리는 태스크 그룹                      */
} OS_SEM_DATA;
#endif

/*
*********************************************************************************************************
*                                          태스크 스택 데이터
*********************************************************************************************************
*/

#if OS_TASK_CREATE_EXT_EN
typedef struct {
    INT32U  OSFree;                    /* 스택에서 사용되지 않은 바이트 수                             */
    INT32U  OSUsed;                    /* 스택에서 사용된 바이트 수                                    */
} OS_STK_DATA;
#endif

/*$PAGE*/
 
/*
*********************************************************************************************************
*                                        태스크 컨트롤 블록
*********************************************************************************************************
*/

typedef struct os_tcb {
    OS_STK        *OSTCBStkPtr;        /* 스택의 최상위를 가리키는 포인터                              */

#if OS_TASK_CREATE_EXT_EN    
    void          *OSTCBExtPtr;        /* TCB 확장을 위한 사용자 정의 데이터에 대한 포인터             */
    OS_STK        *OSTCBStkBottom;     /* 스택의 바닥을 가리키는 포인터                                */
    INT32U         OSTCBStkSize;       /* 태스크 스택의 크기(스택 요소의 수)                           */
    INT16U         OSTCBOpt;           /* OSTaskCreateExt()로 전달되는 태스크 옵션                     */
    INT16U         OSTCBId;            /* 태스크 ID (0..65535)                                         */
#endif

    struct os_tcb *OSTCBNext;          /* TCB 리스트에서 다음 TCB를 가리키는 포인터                    */
    struct os_tcb *OSTCBPrev;          /* TCB 리스테에서 이전 TCB를 가리키는 포인터                    */

#if (OS_Q_EN && (OS_MAX_QS >= 2)) || OS_MBOX_EN || OS_SEM_EN
    OS_EVENT      *OSTCBEventPtr;      /* 이벤트 컨트롤 블록을 가리키는 포인터                         */
#endif

#if (OS_Q_EN && (OS_MAX_QS >= 2)) || OS_MBOX_EN
    void          *OSTCBMsg;           /* OSMboxPost() 또는 OSQPost()로부터 수신된 메시지              */
#endif    

    INT16U         OSTCBDly;           /* 태스크 지연 또는 대기 중인 이벤트에 대한 타임아웃            */
    INT8U          OSTCBStat;          /* 태스크 상태                                                  */
    INT8U          OSTCBPrio;          /* 태스크 우선순위 (0 == 가장 높음, 63 == 가장 낮음)            */

    INT8U          OSTCBX;             /* 태스크 우선순위에 관련된 그룹의 비트 위치(0..7)              */
    INT8U          OSTCBY;             /* 태스크 우선순위에 관련된 준비 테이블 인덱스                  */
    INT8U          OSTCBBitX;          /* 준비 테이블을 액세스하기 위한 비트 마스크                    */
    INT8U          OSTCBBitY;          /* 준비 그룹 액세스 비트를 마스크 하는 비트                     */
    
#if OS_TASK_DEL_EN    
    BOOLEAN        OSTCBDelReq;        /* 태스크 자신을 없앨 것인지 알려준다                           */
#endif
} OS_TCB;

/*$PAGE*/
 
/*
*********************************************************************************************************
*                                              전역 변수
*********************************************************************************************************
*/
                                       
OS_EXT  INT32U       OSCtxSwCtr;               /* 컨텍스트 스위치가 발생한 횟수를 세는 카운터          */

#if     (OS_MAX_EVENTS >= 2)
OS_EXT  OS_EVENT    *OSEventFreeList;          /* 빈 이벤트 컨트롤 블록의 리스트를 가리키는 포인터     */
OS_EXT  OS_EVENT     OSEventTbl[OS_MAX_EVENTS];/* 이벤트 컨트롤 블록 테이블                            */
#endif

OS_EXT  INT32U       OSIdleCtr;                /* Idle 카운터                                          */

#if     OS_TASK_STAT_EN
OS_EXT  INT8S        OSCPUUsage;               /* 퍼센트 단위의 CPU 사용율                             */
OS_EXT  INT32U       OSIdleCtrMax;             /* 1초 동안 셀 수 있는 idel 카운터의 최대 값            */
OS_EXT  INT32U       OSIdleCtrRun;             /* 1초의 런타임 동안 도달한 idel 카운터 값              */
OS_EXT  BOOLEAN      OSStatRdy;                /* 통계 태스크가 준비되었음을 알려주는 프래그           */
#endif

OS_EXT  INT8U        OSIntNesting;             /* 인터럽트 중첩 레벨                                   */

OS_EXT  INT8U        OSLockNesting;            /* 멀티태스킹 잠금 중첩 레벨                            */

OS_EXT  INT8U        OSPrioCur;                /* 현재 태스크 우선순위                                 */
OS_EXT  INT8U        OSPrioHighRdy;            /* 최우선 태스크 우선순위                               */

OS_EXT  INT8U        OSRdyGrp;                        /* 준비 리스트 그룹                              */
OS_EXT  INT8U        OSRdyTbl[OS_RDY_TBL_SIZE];       /* 실행 가능한 태스크 테이블                     */

OS_EXT  BOOLEAN      OSRunning;                       /* 커널이 실행되고 있음을 알려주는 프래그        */

#if     OS_TASK_CREATE_EN  || OS_TASK_CREATE_EXT_EN || OS_TASK_DEL_EN
OS_EXT  INT8U        OSTaskCtr;                       /* 생성된 태스크 수                              */
#endif

OS_EXT  OS_TCB      *OSTCBCur;                        /* 현재 실행되는 TCB을 가리키는 포인터           */
OS_EXT  OS_TCB      *OSTCBFreeList;                   /* 빈 TCB 리스트를 가리키는 포인터               */
OS_EXT  OS_TCB      *OSTCBHighRdy;                    /* 실행 가능한 최우선 순위의 TCB에 대한 포인터   */
OS_EXT  OS_TCB      *OSTCBList;                       /* TCB의 더블 링크드 리스트에 대한 포인터        */
OS_EXT  OS_TCB      *OSTCBPrioTbl[OS_LOWEST_PRIO + 1];/* 생성된 TCB를 가리키는 포인터 테이블           */

OS_EXT  INT32U       OSTime;                   /* 틱 단위 시스템 타임의 현재값                         */


extern  INT8U const  OSMapTbl[8];              /* 우선순위->비트 마스크 변환 테이블                    */
extern  INT8U const  OSUnMapTbl[256];          /* 우선순위->인덱스 변환 테이블                         */

/*$PAGE*/
 
/*
*********************************************************************************************************
*                                              함수 원형
*                                        (타겟 의존적인 함수)
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        메시지 메일박스 관리
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
*                                             메모리 관리
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
*                                            메시지 큐 관리
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
*                                            세마포어 관리
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
*                                             태스크 관리
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
*                                              시간 관리
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
*                                                  기타
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
*                                           내부 함수 원형
*                             (응용프로그램은 이 함수를 호출할 수 없다)
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
*                                              함수 원형
*                                        (타겟 의존적인 함수)
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
