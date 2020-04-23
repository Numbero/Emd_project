/*
 * AD_7705.c
 *
 *  Created on: 2016��7��23��
 *      Author: Numbero
 */
#include<msp430.h>
#include"AD_7705.h"
#include"delay.h"

#define uint unsigned int
#define uchar unsigned char


// ��ʱ
void delay(void)
{
  uint i;
  for(i=3000;i>0;i--);         // �����ʱ�����ò���
}

// д��ָ�����AD7705��д���λ
void AD_write_cmd(uchar cmd)
{
  int i;
  ADCLK_H;        // ʱ���ø�
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
    delay();delay();        // delay()������Ŀ�����ͬ
    ADCLK_H;
    delay();
  }
  ADCLK_H;
  ADDIN_H;
}

// �������ݣ�����ʮ��λ
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

// ��ʼ��AD7705
void InitAD7705_ch0(void)
{
  int i;
  INIT_1;
  INIT_2;
  CS_L;
  ADCLK_H;
  ADDIN_H;
  for(i=0;i<5000;i++)       // ��ֹ�ӿ���ʧ��Communications Register���ص��ȴ�д״̬
  {
    ADCLK_L;
    ADCLK_H;
  }
  AD_write_cmd(0x20);    // ��Clock Registerд,ͨ��1
  AD_write_cmd(0x0B);    // ʱ�� MCLKΪ 2MHz,CLK λ���㣨����Ƶ��<---8(20Hz),9(25Hz),A(100Hz),B(200Hz))

  AD_write_cmd(0x10);    // ��Setup Registerд,ͨ��1
  AD_write_cmd(0x44);    // У��ģʽ������Ϊ1��������

  //AD_write_cmd(0x21);    // ��Clock Registerд,ͨ��2
  //AD_write_cmd(0x0B);    // ʱ�� MCLKΪ 2MHz,CLK λ���㣨����Ƶ��<---8(20Hz),9(25Hz),A(100Hz),B(200Hz))

  //AD_write_cmd(0x11);  	 // ��Setup Registerд,ͨ��2
  //AD_write_cmd(0x44);    // У��ģʽ������Ϊ1��������
}

void InitAD7705_ch1(void)
{
  int i;
  INIT_1;
  INIT_2;
  CS_L;
  ADCLK_H;
  ADDIN_H;
  for(i=0;i<5000;i++)       // ��ֹ�ӿ���ʧ��Communications Register���ص��ȴ�д״̬
  {
    ADCLK_L;
    ADCLK_H;
  }

  AD_write_cmd(0x21);    // ��Clock Registerд,ͨ��2
  AD_write_cmd(0x0B);    // ʱ�� MCLKΪ 2MHz,CLK λ���㣨����Ƶ��<---8(20Hz),9(25Hz),A(100Hz),B(200Hz))

  AD_write_cmd(0x11);  	 // ��Setup Registerд,ͨ��2
  AD_write_cmd(0x44);    // У��ģʽ������Ϊ1��������
}

//ѡ��ͨ�������棬xΪͨ��1/2��yλ����1/2/4/8/16/32/64/128
//x����0��1,y����1~128
void Set_ch(uint x,uint y)
{
  int i;
  ADCLK_H;
  ADDIN_H;
  for(i=0;i<5000;i++)       // ��ֹ�ӿ���ʧ��Communications Register���ص��ȴ�д״̬
  {
    ADCLK_L;
    ADCLK_H;
  }
  AD_write_cmd(0x20+x);		//����ͨ������Ƶ��
  AD_write_cmd(0x0B);

  AD_write_cmd(0x10+x);     //����ͨ������

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

//ѡ���ȡͨ��0/1
unsigned int Get_data(unsigned char ch)
{
    unsigned int memory;
    while(DRDYIN);

    switch(ch)
    {
    case 0:
    	AD_write_cmd(0x38);      // �� Data Register����ͨ��1
    	break;
    case 1:
    	AD_write_cmd(0x39);      // �� Data Register����ͨ��2
    	break;
    default:break;
    }

    memory = read_data(16);
    return memory;
}


//���n��ƽ��ֵ
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

//���n��ƽ��ֵ
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

