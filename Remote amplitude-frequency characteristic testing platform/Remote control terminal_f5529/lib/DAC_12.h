#include <msp430.h>
#include "delay.h"


#ifndef DAC_12_H
#define DAC_12_H


//***********************
//�˿ڶ���
//	CS			--->	P6.5
//	SCK			--->	P6.6
//	DI			--->	P7.0
//***********************
#define DAC_CS_0 P6OUT &= ~BIT5
#define DAC_CS_1 P6OUT |= BIT5
#define DAC_SCK_0 P6OUT &= ~BIT6
#define DAC_SCK_1 P6OUT |= BIT6
#define DAC_DI_0 P7OUT &= ~BIT0
#define DAC_DI_1 P7OUT |= BIT0


//***********************
//��������
//***********************
//DAC��ʼ��
extern void DAC_12_Init();
//����DAC��ֵ
extern void DAC_12_translate_a(unsigned int n); 	//DACͨ��A���
extern void DAC_12_translate_b(unsigned int n); 	//DACͨ��B���


#endif
