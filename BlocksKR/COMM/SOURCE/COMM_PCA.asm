;********************************************************************************************************
;                                     Embedded Systems Building Blocks
;                                  Complete and Ready-to-Use Modules in C
;
;                                    Asynchronous Serial Communications
;                                    IBM-PC Serial I/O Low Level Driver
;
;                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
;                                           All Rights Reserved
;
; Filename   : COMM_PCA.ASM
; Programmer : Jean J. Labrosse
; Translated by : Won-Ho, Sung
; 노트       : uC/OS-II를 사용하지 않는다면 OSIntNesting를 증가하는 부분과 OSIntExit()를 호출하는
;              부분을 지워야 한다.
;********************************************************************************************************

            PUBLIC _Comm1ISR
            PUBLIC _Comm2ISR

            EXTRN  _OSIntExit:FAR
            EXTRN  _CommISRHandler:FAR

            EXTRN  _OSIntNesting:BYTE

.MODEL      LARGE
.CODE
.186

;/*$PAGE*/
;*********************************************************************************************************
;                                            COM1 ISR 처리
;*********************************************************************************************************
;
_Comm1ISR   PROC   FAR
;
            PUSHA                                ; 인터럽트가 발생한 태스크의 문맥을 저장한다
            PUSH   ES
            PUSH   DS
;
            MOV    AX, DGROUP                    ; DS에 DGROUP을 로드한다
            MOV    DS, AX
;
;           노트 : uC/OS-II를 사용하지 않는다면 다음 라인(INC _OSIntNesting)을 주석문 처리하라
            INC    BYTE PTR _OSIntNesting        ; uC/OS-II에게 ISR을 실행하고 있다는 것을 알린다
;
            PUSH   1                             ; 전달인자로 COMM1을 넘겨준다
            CALL   FAR PTR _CommISRHandler       ; COMM 인터럽트 핸들러를 호출한다
            ADD    SP,2
;
;           노트 : uC/OS-II를 사용하지 않는다면 다음 라인(CALL _OSIntExit)을 주석문 처리하라
            CALL   FAR PTR _OSIntExit            ; OS에게 인터럽트 처리가 종료된 것을 알린다
;
            POP    DS                            ; 태스크 문캑을 복귀시킨다
            POP    ES
            POPA
;
            IRET                                 ; 태스크로 리턴한다
;
_Comm1ISR   ENDP
;
;/*$PAGE*/
;*********************************************************************************************************
;                                            COM2 ISR 처리
;*********************************************************************************************************
;
_Comm2ISR   PROC   FAR
;
            PUSHA                                ; 인터럽트가 발생한 태스크의 문맥을 저장한다
            PUSH   ES
            PUSH   DS
;
            MOV    AX, DGROUP                    ; DS에 DGROUP을 로드한다
            MOV    DS, AX
;
;           노트 : uC/OS-II를 사용하지 않는다면 다음 라인(INC _OSIntNesting)을 주석문 처리하라
            INC    BYTE PTR _OSIntNesting        ; uC/OS-II에게 ISR을 실행하고 있다는 것을 알린다
;
            PUSH   2                             ; 전달인자로 COMM1을 넘겨준다
            CALL   FAR PTR _CommISRHandler       ; COMM 인터럽트 핸들러를 호출한다
            ADD    SP,2
;
;           노트 : uC/OS-II를 사용하지 않는다면 다음 라인(CALL _OSIntExit)을 주석문 처리하라
            CALL   FAR PTR _OSIntExit            ; OS에게 인터럽트 처리가 종료된 것을 알린다
;
            POP    DS                            ; 태스크 문맥을 복귀시킨다
            POP    ES
            POPA
;
            IRET                                 ; 태스크로 리턴한다
;
_Comm2ISR   ENDP
;
            END
