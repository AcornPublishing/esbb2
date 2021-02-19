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
; ��Ʈ       : uC/OS-II�� ������� �ʴ´ٸ� OSIntNesting�� �����ϴ� �κа� OSIntExit()�� ȣ���ϴ�
;              �κ��� ������ �Ѵ�.
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
;                                            COM1 ISR ó��
;*********************************************************************************************************
;
_Comm1ISR   PROC   FAR
;
            PUSHA                                ; ���ͷ�Ʈ�� �߻��� �½�ũ�� ������ �����Ѵ�
            PUSH   ES
            PUSH   DS
;
            MOV    AX, DGROUP                    ; DS�� DGROUP�� �ε��Ѵ�
            MOV    DS, AX
;
;           ��Ʈ : uC/OS-II�� ������� �ʴ´ٸ� ���� ����(INC _OSIntNesting)�� �ּ��� ó���϶�
            INC    BYTE PTR _OSIntNesting        ; uC/OS-II���� ISR�� �����ϰ� �ִٴ� ���� �˸���
;
            PUSH   1                             ; �������ڷ� COMM1�� �Ѱ��ش�
            CALL   FAR PTR _CommISRHandler       ; COMM ���ͷ�Ʈ �ڵ鷯�� ȣ���Ѵ�
            ADD    SP,2
;
;           ��Ʈ : uC/OS-II�� ������� �ʴ´ٸ� ���� ����(CALL _OSIntExit)�� �ּ��� ó���϶�
            CALL   FAR PTR _OSIntExit            ; OS���� ���ͷ�Ʈ ó���� ����� ���� �˸���
;
            POP    DS                            ; �½�ũ ��Ĵ�� ���ͽ�Ų��
            POP    ES
            POPA
;
            IRET                                 ; �½�ũ�� �����Ѵ�
;
_Comm1ISR   ENDP
;
;/*$PAGE*/
;*********************************************************************************************************
;                                            COM2 ISR ó��
;*********************************************************************************************************
;
_Comm2ISR   PROC   FAR
;
            PUSHA                                ; ���ͷ�Ʈ�� �߻��� �½�ũ�� ������ �����Ѵ�
            PUSH   ES
            PUSH   DS
;
            MOV    AX, DGROUP                    ; DS�� DGROUP�� �ε��Ѵ�
            MOV    DS, AX
;
;           ��Ʈ : uC/OS-II�� ������� �ʴ´ٸ� ���� ����(INC _OSIntNesting)�� �ּ��� ó���϶�
            INC    BYTE PTR _OSIntNesting        ; uC/OS-II���� ISR�� �����ϰ� �ִٴ� ���� �˸���
;
            PUSH   2                             ; �������ڷ� COMM1�� �Ѱ��ش�
            CALL   FAR PTR _CommISRHandler       ; COMM ���ͷ�Ʈ �ڵ鷯�� ȣ���Ѵ�
            ADD    SP,2
;
;           ��Ʈ : uC/OS-II�� ������� �ʴ´ٸ� ���� ����(CALL _OSIntExit)�� �ּ��� ó���϶�
            CALL   FAR PTR _OSIntExit            ; OS���� ���ͷ�Ʈ ó���� ����� ���� �˸���
;
            POP    DS                            ; �½�ũ ������ ���ͽ�Ų��
            POP    ES
            POPA
;
            IRET                                 ; �½�ũ�� �����Ѵ�
;
_Comm2ISR   ENDP
;
            END
