/*
*********************************************************************************************************
*                                          PC SUPPORT FUNCTIONS
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                           All Rights Reserved
*
* File : PC.H
* By   : Jean J. Labrosse
* Translated by : Won-Ho, Sung
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 상수
*                                     VGA 모니터에 대한 색상 속성
*
* 설명       : 이 상수들은 PC_Disp???() 함수가 사용한다. 이 함수들의 전달인자 중 'color' 인자는 문자를
*              점멸할 지의 여부에 상관 없이 전경, 배경 색이 모두 지정되어야 한다.
*              배경 색을 지정하지 않으면 기본적으로 검정 바탕이 표시된다.
*              예를 들어, 푸른 바탕에 횐색으로 'A'문자를 점멸하고자 한다면 다음과 같이 색 조합을
*              지정해야 한다.
*
*              PC_DispChar(0, 0, 'A', DISP_FGND_WHITE + DISP_BGND_BLUE + DISP_BLINK);
*
*********************************************************************************************************
*/
#define DISP_FGND_BLACK           0x00
#define DISP_FGND_BLUE            0x01
#define DISP_FGND_GREEN           0x02
#define DISP_FGND_CYAN            0x03
#define DISP_FGND_RED             0x04
#define DISP_FGND_PURPLE          0x05
#define DISP_FGND_BROWN           0x06
#define DISP_FGND_LIGHT_GRAY      0x07
#define DISP_FGND_DARK_GRAY       0x08
#define DISP_FGND_LIGHT_BLUE      0x09
#define DISP_FGND_LIGHT_GREEN     0x0A
#define DISP_FGND_LIGHT_CYAN      0x0B
#define DISP_FGND_LIGHT_RED       0x0C
#define DISP_FGND_LIGHT_PURPLE    0x0D
#define DISP_FGND_YELLOW          0x0E
#define DISP_FGND_WHITE           0x0F

#define DISP_BGND_BLACK           0x00
#define DISP_BGND_BLUE            0x10
#define DISP_BGND_GREEN           0x20
#define DISP_BGND_CYAN            0x30
#define DISP_BGND_RED             0x40
#define DISP_BGND_PURPLE          0x50
#define DISP_BGND_BROWN           0x60
#define DISP_BGND_LIGHT_GRAY      0x70

#define DISP_BLINK                0x80

/*
*********************************************************************************************************
*                                              함수원형
*********************************************************************************************************
*/

void    PC_DispChar(INT8U x, INT8U y, INT8U c, INT8U color);
void    PC_DispClrCol(INT8U x, INT8U bgnd_color);
void    PC_DispClrRow(INT8U y, INT8U bgnd_color);
void    PC_DispClrScr(INT8U bgnd_color);
void    PC_DispStr(INT8U x, INT8U y, INT8U *s, INT8U color);

void    PC_DOSReturn(void);
void    PC_DOSSaveReturn(void);

void    PC_ElapsedInit(void);
void    PC_ElapsedStart(void);
INT16U  PC_ElapsedStop(void);

void    PC_GetDateTime(char *s);
BOOLEAN PC_GetKey(INT16S *c);

void    PC_SetTickRate(INT16U freq);

void   *PC_VectGet(INT8U vect);
void    PC_VectSet(INT8U vect, void (*isr)(void));
