/*
 * AD_7705.h
 *
 *  Created on: 2016年7月23日
 *      Author: Numbero
 */

#ifndef AD_7705_H_
#define AD_7705_H_

#include "stdio.h"
#include<msp430.h>
#include "intrinsics.h"

#define ADCLK_H   P6OUT |= BIT2         // CLK--->P6.2
#define ADCLK_L   P6OUT &= ~BIT2

#define CS_H      P6OUT |= BIT0         // CS--->P6.0
#define CS_L      P6OUT &= ~BIT0

#define ADDIN_H   P6OUT |= BIT1       	// DIN--->P6.1
#define ADDIN_L   P6OUT &= ~BIT1

#define DOUT    P6IN & BIT4        		// DOUT--->P6.4
#define DRDYIN  P6IN & BIT3         	// DRDY--->P6.3,实时查询DRDY的状态

#define INIT_1 P6DIR |= (BIT0+BIT1+BIT2)
#define INIT_2 P6DIR &= ~(BIT3+BIT4)


extern void delay(void);
extern void AD_write_cmd(unsigned char cmd);
extern unsigned int read_data(int data);
//初始化
extern void InitAD7705_ch0(void);
extern void InitAD7705_ch1(void);
//设置通道增益
extern void Set_ch(unsigned int x,unsigned int y);
//获取数值
extern unsigned int Get_data(unsigned char ch);
extern unsigned long ADC_ave(char n,unsigned char ch);
extern unsigned long ADC_ave_pro(char n,unsigned char ch);

#endif /* AD_7705_H_ */
