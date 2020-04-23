#ifndef _B_LCD12864_H_
#define _B_LCD12864_H_

#include "stdio.h"
#include<msp430.h>
#include "intrinsics.h"

#define LCD_DataIn    P9DIR=0X00
#define LCD_DataOut   P9DIR=0Xff
#define LCD2MCU_Data  P9IN
#define MCU2LCD_Data  P9OUT

#define LCD_RS        BIT5
#define LCD_RW        BIT0
#define LCD_EN        BIT7


#define LCD_CMDOut    {P5DIR|= LCD_RS;P8DIR|= LCD_RW+LCD_EN;}
#define LCD_RS_H      P5OUT|=LCD_RS
#define LCD_RS_L      P5OUT&=~LCD_RS
#define LCD_RW_H      P8OUT|=LCD_RW
#define LCD_RW_L      P8OUT&=~LCD_RW
#define LCD_EN_H      P8OUT|=LCD_EN
#define LCD_EN_L      P8OUT&=~LCD_EN
#ifndef LCD12864_H
#define LCD12864_H

#define iDat	1		//数据标志
#define iCmd	0		//指令标志

#define LCDb_RS	    0x20 	//BIT5  	//P5.5
#define LCDb_RW		0x01 	//BIT0  	//P8.0
#define LCDb_E	    0x80	//BIT7		//P8.7
#define LCDb_RST	0x10	//BIT4		//P5.4

#define LCDb_L1		0x80	//第一行的地址
#define LCDb_L2		0x90	//第二行的地址
#define LCDb_L3		0x88	//第三行的地址
#define LCDb_L4		0x98	//第四行的地址

#define LCDb_SET_RS			P5OUT|=LCDb_RS	//四个控制管脚的控制操作
#define LCDb_SET_RW 		P8OUT|=LCDb_RW
#define LCDb_SET_E  		P8OUT|=LCDb_E
#define LCDb_CLR_RST  		P5OUT|=LCDb_RST
#define LCDb_CLR_RS 		P5OUT&=~LCDb_RS
#define LCDb_CLR_RW 		P8OUT&=~LCDb_RW
#define LCDb_CLR_E  		P8OUT&=~LCDb_E
#define LCDb_SET_RST 		P5OUT&=~LCDb_RST

#define LCDb_DO		P9OUT	        //输出数据总线端口定义

#define LCDb_FUNCTION	0x38   	        // 液晶模式为8位，2行，5*8字符
#define LCDb_BASCMD	0x30		// 基本指令集
#define LCDb_CLS	0x01		// 清屏
#define LCDb_HOME	0x02		// 地址返回原点，不改变DDRAM内容
#define LCDb_ENTRY 	0x06		// 设定输入模式，光标加，屏幕不移动
#define LCDb_C2L	0x10		// 光标左移
#define LCDb_C2R	0x14		// 光标右移
#define LCDb_D2L	0x18		// 屏幕左移
#define LCDb_D2R	0x1C		// 屏幕又移
#define LCDb_ON		0x0C		// 打开显示
#define LCDb_OFF	0x08		// 关闭显示

#ifndef _DELAY_H
#define _DELAY_H

#define F_CPU ((double)1045000)//1.045M MCLK=DCO
#define _delay_us(x) __delay_cycles((long)((F_CPU * (double)x / 1000000.0)))
#define _delay_ms(x) __delay_cycles((long)((F_CPU * (double)x / 1000.0)))


//初始化lcd12864
extern void init12864(void);
//清屏函数
extern void clr_screen(void);
//清除液晶GDRAM中的随机数据
extern void clear_GDRAM(void);
//行显示函数
extern void display_line(unsigned char x,unsigned char y,const char* str);
//页显示函数
extern void display_page( const char **s);
//长整形数数显示函数
extern void display_long(unsigned char x,unsigned char y,unsigned long d);
//浮点数显示函数
extern void display_float(unsigned char x,unsigned char y,float d,unsigned int eps);
//双精度显示函数
extern void display_double(unsigned char x,unsigned char y,double d,unsigned int eps);
//两位数显示函数
extern void display_2digit(unsigned char x,unsigned char y,unsigned int d);
//三位数显示函数
extern void display_3digit(unsigned char x,unsigned char y,unsigned int d);
//全屏图片显示
extern void draw_picture(const unsigned char *ptr);
//任意位置反白显示函数
extern void set_white(unsigned char y,unsigned char x,unsigned char end_x,unsigned char clear);
//打点函数
extern void draw_dot(unsigned char x,unsigned char y,unsigned char color);
//画水平直线
extern void draw_level_line(unsigned char x0,unsigned char x1,unsigned char y,unsigned char color);
//画垂直直线
extern void draw_vertical_line(unsigned char y0,unsigned char y1,unsigned char x,unsigned char color);
//画任意直线
extern void draw_line(unsigned char startx,unsigned char starty
              ,unsigned char endx,unsigned char endy,unsigned char color);
//连续输入Y 连成线
extern void contin_line(unsigned char startx ,unsigned char endx ,unsigned char Y);
//画数组曲线
extern void draw_curve(unsigned char xstart, unsigned char size ,unsigned char *str);
//画圆函数
extern void draw_circle(unsigned char x,unsigned char y,unsigned char r,unsigned char color);


#endif
#endif
#endif
