#include <msp430.h>
#include "delay.h"


#ifndef DAC_12_H
#define DAC_12_H


//***********************
//端口定义
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
//函数声明
//***********************
//DAC初始化
extern void DAC_12_Init();
//设置DAC数值
extern void DAC_12_translate_a(unsigned int n); 	//DAC通道A输出
extern void DAC_12_translate_b(unsigned int n); 	//DAC通道B输出


#endif
