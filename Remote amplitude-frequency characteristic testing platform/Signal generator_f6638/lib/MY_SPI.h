#include <msp430.h>
#include "delay.h"

#ifndef MY_SPI_H
#define MY_SPI_H

#define uchar unsigned char
#define uint unsigned int

//***********************
//端口定义
//***********************
#define CS_1 P5OUT|=BIT0
#define CS_0 P5OUT&=~BIT0

#define CLK_1 P5OUT|=BIT1
#define CLK_0 P5OUT&=~BIT1

#define DIO_1 P5OUT|=BIT6
#define DIO_0 P5OUT&=~BIT6

#define DO_IN P5IN&BIT7 //输入引脚


//***********************
//函数声明
//***********************
extern void Spi_Init(void);			//初始化
//开始
extern void Spi_Start();
//结束
extern void Spi_Stop();
//继续传输
void Spi_Repit();
//写入一个字节
extern void Spi_WriteByte(uchar data);
//读取一个字节
extern uchar Spi_ReadByte();
//并行输入输出,实现数据交换
extern uchar Spi_TransByte(uchar data);

#endif
