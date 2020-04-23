/*
 * AD_7705.c
 *
 *  Created on: 2016年7月23日
 *      Author: Numbero
 */
#include<msp430.h>
#include"AD_7705.h"
#include"delay.h"

#define uint unsigned int
#define uchar unsigned char


// 延时
void delay(void)
{
  uint i;
  for(i=3000;i>0;i--);         // 需根据时钟配置参数
}

// 写入指令，配置AD7705，写入八位
void AD_write_cmd(uchar cmd)
{
  int i;
  ADCLK_H;        // 时钟置高
  delay();
  for(i=1;i<=8;i++)
  {
    if(cmd & 0x80)
      ADDIN_H;
    else
      ADDIN_L;
    cmd <<=1;
    delay();
    ADCLK_L;
    delay();delay();        // delay()函数数目间隔相同
    ADCLK_H;
    delay();
  }
  ADCLK_H;
  ADDIN_H;
}

// 读出数据，读出十六位
uint read_data(int data)
{
  int i;
  uint digit=0;
  while(DRDYIN);
  ADCLK_H;
  delay();
  for(i=1;i<=data;i++)
  {
    digit<<=1;
    ADCLK_L;
    delay();delay();
    if(DOUT)
      digit|=0x01;
    delay();
    ADCLK_H;
    delay();
  }
  ADCLK_H;
  ADDIN_H;
  return digit;
}

// 初始化AD7705
void InitAD7705_ch0(void)
{
  int i;
  INIT_1;
  INIT_2;
  CS_L;
  ADCLK_H;
  ADDIN_H;
  for(i=0;i<5000;i++)       // 防止接口迷失，Communications Register返回到等待写状态
  {
    ADCLK_L;
    ADCLK_H;
  }
  AD_write_cmd(0x20);    // 向Clock Register写,通道1
  AD_write_cmd(0x0B);    // 时钟 MCLK为 2MHz,CLK 位置零（采样频率<---8(20Hz),9(25Hz),A(100Hz),B(200Hz))

  AD_write_cmd(0x10);    // 向Setup Register写,通道1
  AD_write_cmd(0x44);    // 校正模式，增益为1，单极性

  //AD_write_cmd(0x21);    // 向Clock Register写,通道2
  //AD_write_cmd(0x0B);    // 时钟 MCLK为 2MHz,CLK 位置零（采样频率<---8(20Hz),9(25Hz),A(100Hz),B(200Hz))

  //AD_write_cmd(0x11);  	 // 向Setup Register写,通道2
  //AD_write_cmd(0x44);    // 校正模式，增益为1，单极性
}

void InitAD7705_ch1(void)
{
  int i;
  INIT_1;
  INIT_2;
  CS_L;
  ADCLK_H;
  ADDIN_H;
  for(i=0;i<5000;i++)       // 防止接口迷失，Communications Register返回到等待写状态
  {
    ADCLK_L;
    ADCLK_H;
  }

  AD_write_cmd(0x21);    // 向Clock Register写,通道2
  AD_write_cmd(0x0B);    // 时钟 MCLK为 2MHz,CLK 位置零（采样频率<---8(20Hz),9(25Hz),A(100Hz),B(200Hz))

  AD_write_cmd(0x11);  	 // 向Setup Register写,通道2
  AD_write_cmd(0x44);    // 校正模式，增益为1，单极性
}

//选择通道与增益，x为通道1/2，y位增益1/2/4/8/16/32/64/128
//x——0或1,y——1~128
void Set_ch(uint x,uint y)
{
  int i;
  ADCLK_H;
  ADDIN_H;
  for(i=0;i<5000;i++)       // 防止接口迷失，Communications Register返回到等待写状态
  {
    ADCLK_L;
    ADCLK_H;
  }
  AD_write_cmd(0x20+x);		//设置通道采样频率
  AD_write_cmd(0x0B);

  AD_write_cmd(0x10+x);     //设置通道增益

  switch(y)
  {
  	case 1:AD_write_cmd(0x44);break;
  	case 2:AD_write_cmd(0x4c);break;
  	case 4:AD_write_cmd(0x54);break;
  	case 8:AD_write_cmd(0x5c);break;
  	case 16:AD_write_cmd(0x64);break;
  	case 32:AD_write_cmd(0x6c);break;
  	case 64:AD_write_cmd(0x74);break;
  	case 128:AD_write_cmd(0x7c);break;
  	default:break;
  }
}

//选择读取通道0/1
unsigned int Get_data(unsigned char ch)
{
    unsigned int memory;
    while(DRDYIN);

    switch(ch)
    {
    case 0:
    	AD_write_cmd(0x38);      // 从 Data Register读，通道1
    	break;
    case 1:
    	AD_write_cmd(0x39);      // 从 Data Register读，通道2
    	break;
    default:break;
    }

    memory = read_data(16);
    return memory;
}


//求出n次平均值
unsigned long ADC_ave(char n,unsigned char ch)
{
	volatile char i,pit;
	unsigned int ue;
	unsigned int ADC_temp[50];
	unsigned long ADC_tem;
	for(ue=0;ue<n;ue++)
	{
		ADC_temp[ue] = Get_data(ch);
		_delay_ms(4);
	}
	ADC_tem = 0;
	for(pit=0;pit<n;pit++)
	{
		ADC_tem += ADC_temp[pit];
	}
	ADC_tem = ADC_tem/n;
	return ADC_tem;
}

//求出n次平均值
unsigned long ADC_ave_pro(char n,unsigned char ch)
{
	volatile char i,pit;
	unsigned int ue,max,min;
	unsigned int ADC_temp[300];
	unsigned long ADC_tem;
	for(ue=0;ue<n;ue++)
	{
		ADC_temp[ue]=Get_data(ch);
		_delay_ms(4);
	}
	ADC_tem=0;
	for(pit=4;pit<n;pit++)
	{
		ADC_tem+=ADC_temp[pit];
	}
	max=ADC_temp[4];
	min=ADC_temp[4];
	for(i=4;i<n;i++)
	{
		if(ADC_temp[i]>max) max=ADC_temp[i];
		if(ADC_temp[i]<min) min=ADC_temp[i];
	}
	ADC_tem=(ADC_tem-max-min)/(n-6);

	return ADC_tem;
}

