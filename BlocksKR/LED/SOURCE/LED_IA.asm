;********************************************************************************************************
;                                    Embedded Systems Building Blocks
;                                 Complete and Ready-to-Use Modules in C
;
;                                     Multiplexed LED Display Driver
;                                            LED Multiplex ISR
;                                        Intel 80x86 (LARGE MODEL)
;
;                            (c) Copyright 1999, Jean J. Labrosse, Weston, FL
;                                          All Rights Reserved
;
; File : LED_IA.ASM
; By   : Jean J. Labrosse
; Translated by : Won-Ho, Sung
;********************************************************************************************************

            PUBLIC _DispMuxISR

            EXTRN  _DispMuxHandler:FAR
            EXTRN  _OSIntExit:FAR
            EXTRN  _OSIntNesting:BYTE

.MODEL      LARGE
.CODE
.186

;*********************************************************************************************************
;                           LED 디스플레이 매트릭스에 다음 세그먼트 패턴 출력
;                                         void DispMuxISR(void)
;*********************************************************************************************************

_DispMuxISR PROC FAR
;
            PUSHA                                ;프로세서의 컨텍스트를 저장한다
            PUSH   ES
            PUSH   DS
;
            INC    BYTE PTR _OSIntNesting        ;uC/OS-II에게 ISR이 수행되고 있음을 알림
            CALL   FAR  PTR _DispMuxHandler      ;멀티플렉싱을 수행하는 C루틴을 호출한다
            CALL   FAR  PTR _OSIntExit           ;uC/OS-II 스케줄러에게 ISR이 끝남을 알림
;
            POP    DS                            ;프로세서의 컨텍스트를 복귀시킨다
            POP    ES
            POPA
;
            IRET                                 ;인터럽트가 일어났던 코드로 복귀한다
;
_DispMuxISR ENDP

            END
