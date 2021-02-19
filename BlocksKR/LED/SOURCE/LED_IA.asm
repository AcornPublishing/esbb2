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
;                           LED ���÷��� ��Ʈ������ ���� ���׸�Ʈ ���� ���
;                                         void DispMuxISR(void)
;*********************************************************************************************************

_DispMuxISR PROC FAR
;
            PUSHA                                ;���μ����� ���ؽ�Ʈ�� �����Ѵ�
            PUSH   ES
            PUSH   DS
;
            INC    BYTE PTR _OSIntNesting        ;uC/OS-II���� ISR�� ����ǰ� ������ �˸�
            CALL   FAR  PTR _DispMuxHandler      ;��Ƽ�÷����� �����ϴ� C��ƾ�� ȣ���Ѵ�
            CALL   FAR  PTR _OSIntExit           ;uC/OS-II �����ٷ����� ISR�� ������ �˸�
;
            POP    DS                            ;���μ����� ���ؽ�Ʈ�� ���ͽ�Ų��
            POP    ES
            POPA
;
            IRET                                 ;���ͷ�Ʈ�� �Ͼ�� �ڵ�� �����Ѵ�
;
_DispMuxISR ENDP

            END
