#ifndef __OLED_IIC_13_H
#define __OLED_IIC_13_H

#include<msp430.h>

#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF
#define X_WIDTH 	128
#define Y_WIDTH 	64
//-----------------OLED IIC端口定义----------------

#define OLED_SDIN_Set()	P4OUT |= BIT1
#define OLED_SDIN_Clr()	P4OUT &= ~BIT1

#define OLED_SCLK_Set()	P4OUT |= BIT2
#define OLED_SCLK_Clr()	P4OUT &= ~BIT2

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

//OLED控制用函数
extern void OLED_WR_Byte(unsigned dat,unsigned cmd);
extern void OLED_Display_On(void);
extern void OLED_Display_Off(void);
extern void OLED_Init(void);
extern void OLED_Clear(void);
extern void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char t);
extern void OLED_Fill(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char dot);
extern void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char Char_Size);
extern void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size);
extern void OLED_ShowFloat(unsigned char x,unsigned char y,float d,unsigned char eps,unsigned char size);
extern void OLED_ShowDouble(unsigned char x,unsigned char y,double d,unsigned char eps,unsigned char size);
extern void OLED_ShowLong(unsigned char x,unsigned char y,unsigned long d,unsigned char size);
extern void OLED_ShowString(unsigned char x,unsigned char y, unsigned char *p,unsigned char Char_Size);
extern void OLED_Set_Pos(unsigned char x, unsigned char y);
extern void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no);
extern void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
extern void Delay_50ms(unsigned int Del_50ms);
extern void Delay_1ms(unsigned int Del_1ms);
extern void fill_picture(unsigned char fill_Data);
extern void Picture();
extern void IIC_Start();
extern void IIC_Stop();
extern void Write_IIC_Command(unsigned char IIC_Command);
extern void Write_IIC_Data(unsigned char IIC_Data);
extern void Write_IIC_Byte(unsigned char IIC_Byte);

#endif

