#include <msp430.h>
#include "delay.h"

#ifndef MY_SPI_H
#define MY_SPI_H

#define uchar unsigned char
#define uint unsigned int

//***********************
//�˿ڶ���
//***********************
#define CS_1 P5OUT|=BIT0
#define CS_0 P5OUT&=~BIT0

#define CLK_1 P5OUT|=BIT1
#define CLK_0 P5OUT&=~BIT1

#define DIO_1 P5OUT|=BIT6
#define DIO_0 P5OUT&=~BIT6

#define DO_IN P5IN&BIT7 //��������


//***********************
//��������
//***********************
extern void Spi_Init(void);			//��ʼ��
//��ʼ
extern void Spi_Start();
//����
extern void Spi_Stop();
//��������
void Spi_Repit();
//д��һ���ֽ�
extern void Spi_WriteByte(uchar data);
//��ȡһ���ֽ�
extern uchar Spi_ReadByte();
//�����������,ʵ�����ݽ���
extern uchar Spi_TransByte(uchar data);

#endif
