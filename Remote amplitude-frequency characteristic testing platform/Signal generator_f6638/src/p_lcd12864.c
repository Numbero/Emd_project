#include<msp430.h>
#include<stdio.h>
#include "p_lcd12864.h"


/**************************************************
�������ƣ�lcd_delay_n
��    �ܣ�������ʱ ֻҪ����������ʾ���� ����ʵ�ʵ���
��    ����n
����ֵ  ����
**************************************************/
void lcd_delay_n(unsigned int n)
{
    unsigned int i;
    for(i=n;i>0;i--)  _NOP();
}



/**************************************************
�������ƣ�my_abs
��    �ܣ������ֵ   ����math��h�е�abs�����о��� �����Լ�д
��    ����a
����ֵ  ����
**************************************************/
unsigned int my_abs(int a)
{
    if(a<0)
        a=-a;
    return a;
}



/**************************************************
�������ƣ�write_cmd
��    �ܣ���Һ����д��������
��    ����cmd--��������
����ֵ  ����
**************************************************/
void write_cmd(unsigned char cmd)
{
    unsigned char lcdtemp = 0;
    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;
    do
    {
        LCD_EN_H;
        _NOP();
        lcdtemp = LCD2MCU_Data;
        LCD_EN_L;
    }
    while(lcdtemp&0x80);
    
    LCD_DataOut;
    LCD_RS_L;
    LCD_RW_L;
    MCU2LCD_Data = cmd;
    
    LCD_EN_H;
    _NOP();
    LCD_EN_L;
}



/**************************************************
�������ƣ�write_data
��    �ܣ���Һ����д����
��    ����dat--��ʾ����
����ֵ  ����
**************************************************/
void write_data(unsigned char dat)
{
    unsigned char lcdtemp;
    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;
    do
    {
        LCD_EN_H;
        _NOP();
        lcdtemp = LCD2MCU_Data;
        LCD_EN_L;
    }
    while(lcdtemp&0x80);
    
    LCD_RS_H;
    LCD_RW_L;
    LCD_DataOut;
    MCU2LCD_Data = dat;
    LCD_EN_H;
    _NOP();
    LCD_EN_L;
}




/**************************************************
�������ƣ�lcd_read_data
��    �ܣ���ȡ12864��һ���ֽڵ�����
��    ������
����ֵ  ����ʾ������
**************************************************/
unsigned char lcd_read_data(void)
{
    unsigned char Data_Temp;
    unsigned char lcdtemp;
    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;
    do
    {
        LCD_EN_H;
        _NOP();
        lcdtemp = LCD2MCU_Data;
        LCD_EN_L;
    }
    while(lcdtemp&0x80);
    
    LCD_RS_H;
    LCD_RW_H;
    LCD_DataIn;
    
    LCD_EN_H;
    _NOP();
    Data_Temp = LCD2MCU_Data;
    LCD_EN_L;
    
    return Data_Temp;
}




/**************************************************
�������ƣ�lcd_setxy
��    �ܣ�������ʾλ��    
��    ����X(1~16),Y(1~4)
����ֵ  ����
**************************************************/
void lcd_setxy(unsigned char x,unsigned char y)
{
    switch(y)
    {
        case 1:
        write_cmd(LCDb_L1 + x);break;
        case 2:
        write_cmd(LCDb_L2 + x);break;
        case 3:
        write_cmd(LCDb_L3 + x);break;
        case 4:
        write_cmd(LCDb_L4 + x);break;
        default:break;
    }
}




/**************************************************
�������ƣ�display_line
��    �ܣ���ָ��λ����ʾ�ַ���    
��    ��������x y �ַ���str
����ֵ  ����
**************************************************/
void display_line(unsigned char x,unsigned char y,const char* str)
{
    unsigned char LCD_temp;
    lcd_setxy(x,y);
    LCD_temp=*str;
    while(LCD_temp != 0x00)
    {
        write_data(LCD_temp);
        LCD_temp=*(++str);
    }
}




/**************************************************
�������ƣ�display_3digit
��    �ܣ���ָ��λ�ÿ�ʼ��ʾ��λ����    
��    ��������x y ����d
����ֵ  ����
**************************************************/
void display_3digit(unsigned char x,unsigned char y,unsigned int d)
{
    unsigned char a[3],i;
    a[0]=d/100;
    a[1]=(d%100)/10;
    a[2]=d%10;
    lcd_setxy(x,y);
    for(i=0;i<3;i++)
    {
        write_data(0x30+a[i]);
        //DelayUs2x(15);
        lcd_delay_n(1);
    }
}




/**************************************************
�������ƣ�display_2digit
��    �ܣ���ָ��λ�ÿ�ʼ��ʾ��λ����    
��    ��������x y ����d
����ֵ  ����
**************************************************/
void display_2digit(unsigned char x,unsigned char y,unsigned int d)
{
    unsigned char a[2],i;
    a[0]=d/10;
    a[1]=d%10;
    lcd_setxy(x,y);
    for(i=0;i<2;i++)
    {
        write_data(0x30+a[i]);
        //DelayUs2x(15);
        lcd_delay_n(1);
    }
}




/**************************************************
�������ƣ�display_float
��    �ܣ���ָ��λ�ÿ�ʼ��ʾ������    
��    ��������x/y,����d,С�����epsλ
����ֵ  ����
**************************************************/
void display_float(unsigned char x,unsigned char y,float d,unsigned int eps)
{
    char str[15];
    char ch[20];
    //**********************************************
    //sprintf(a,"%f",d);  //�޸�.f�м�����ֿɸı䱣����λС��
    //��֪��Ϊʲô������û����ĳ��򣬵��������ͨ����
    //**********************************************
    int high;		//float_��������
    double low;		//float_С������
    int n,i;

    high = (int)d;
    low = d - high;

    n = 0;
    if(high == 0)
    {
    	ch[n] = '0';
    	n++;
    }
    while(high > 0)		//����������ת��Ϊ�ַ���
    {
    	ch[n] = '0' + high % 10;
        high = high / 10;
        n++;
    }
    n = n-1;	//n����
    for(i = n;i >= 0;i--)	//���������ִ����ַ�����
    {
        str[n-i]=ch[i];
    }
    n = n - i;
    str[n] = '.';		//��С�����������֮��

    for(i=0;i < eps;i++)	//��С��λ�����ַ�����
    {
    	low *= 10;
    	n++;
    	str[n] = '0' + (int)low;
    	low -= (int)low;
    }
    if((int)(low*10) >= 5)
    	str[n] += 1;

    n++;	//��ĩβ�����ַ���������
    str[n] = '\0';

    display_line(x,y,str);
}

//˫������ʾ����
void display_double(unsigned char x,unsigned char y,double d,unsigned int eps)
{
    char str[15];
    char ch[20];

    long high;		//double_��������
    double low;		//double_С������
    int n,i;

    high = (long)d;
    low = d - high;

    n = 0;
    if(high == 0)
    {
    	ch[n] = '0';
    	n++;
    }
    while(high > 0)		//����������ת��Ϊ�ַ���
    {
    	ch[n] = '0' + high % 10;
        high = high / 10;
        n++;
    }
    n = n-1;	//n����
    for(i = n;i >= 0;i--)	//���������ִ����ַ�����
    {
        str[n-i]=ch[i];
    }
    n = n - i;
    str[n] = '.';		//��С�����������֮��

    for(i=0;i < eps;i++)	//��С��λ�����ַ�����
    {
    	low *= 10;
    	n++;
    	str[n] = '0' + (long)low;
    	low -= (long)low;
    }
    if((long)(low*10) >= 5)
    	str[n] += 1;

    n++;	//��ĩβ�����ַ���������
    str[n] = '\0';

    display_line(x,y,str);
}

void display_long(unsigned char x,unsigned char y,unsigned long d)
{
	char str[15];
    char ch[20];

    int n,i;

    n = 0;
    if(d == 0)
	{
    	ch[n] = '0';
    	n++;
    }
    while(d > 0)		//����������ת��Ϊ�ַ���
    {
	   	ch[n] = '0' + d % 10;
	    d = d / 10;
        n++;
    }
    n = n-1;	//n����
	for(i = n;i >= 0;i--)	//���������ִ����ַ�����
    {
        str[n-i]=ch[i];
    }
	n++;	//��ĩβ�����ַ���������
	str[n] = '\0';

	display_line(x,y,str);
}


/**************************************************
�������ƣ�clr_screen
��    �ܣ������ʾ    
��    ������
����ֵ  ����
**************************************************/
void clr_screen(void)
{
    write_cmd(0x01);
    //delay_ms(15);
}




/**************************************************
�������ƣ�display_page
��    �ܣ���ʾһҳ�ַ�    
��    �����ַ�����s
����ֵ  ����

��ʽconst  char   *Page1[]=
{
     {"**��>>�˵�<<��**"},
     {"����  ͨѶ   QQ "},
     {"��Ϣ  ����  ���"},
     {"����  ����   UC "}
     
};
**************************************************/
void display_page( const char **s)
{
    unsigned char  i;
    clr_screen();
    for(i=1;i<5;i++)
        display_line(1,i,s[i-1]);
    
}




/**************************************************
�������ƣ�init12864
��    �ܣ���ʼ��Һ��ģ��
��    ������
����ֵ  ����
**************************************************/
void init12864(void)
{
    LCD_DataOut;
    LCD_CMDOut;                  //Һ�����ƶ˿�����Ϊ���
    
    lcd_delay_n(50);
    write_cmd(0x30);            //����ָ�
    lcd_delay_n(50);
    write_cmd(0x30);            //ѡ��8bit������
    lcd_delay_n(50);
    write_cmd(0x02);            //��ַ��λ
    lcd_delay_n(50);
    write_cmd(0x0c);            //������ʾ��,�α�ر�
    lcd_delay_n(50);
    write_cmd(0x01);            //�����ʾ
    lcd_delay_n(50);
    write_cmd(0x06);            //�α�����
    lcd_delay_n(50);
    write_cmd(0x80);            //�趨��ʾ����ʼ��ַ
    lcd_delay_n(2000);
}





/**************************************************
�������ƣ�Clear_GDRAM
��    �ܣ����Һ��GDRAM�е��������
��    ������
����ֵ  ����
**************************************************/
void clear_GDRAM(void)
{
    unsigned char i,j,k;
    
    write_cmd(0x34);               //����չָ�
    i = 0x80;            
    for(j = 0;j < 32;j++)
    {
        write_cmd(i++);
        write_cmd(0x80);
        for(k = 0;k < 16;k++)
        {
            write_data(0x00);
        }
    }
    i = 0x80;
    for(j = 0;j < 32;j++)
    {
        write_cmd(i++);
        write_cmd(0x88);	   
        for(k = 0;k < 16;k++)
        {
            write_data(0x00);
        } 
    }   
    write_cmd(0x30);                  //�ص�����ָ�
}




/*******************************************
��������:draw_picture
��    ��:������Һ����Ļ�ϻ�ͼ
��    ��:ͼƬ����ptr
����ֵ  :��
��    ʽ�� ���x�߶�=128x64 
unsigned char const logo[]={
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x07,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x3F,0xFF,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0xFF,0xFF,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x01,0xFF,0xFF,0xF8,0x00,0x00,0x00,0x7F,0x80,0x00,0x00,0x7F,0x80,0x00,0x00,
    0x00,0x03,0xFF,0xFF,0xFE,0x00,0x00,0x01,0xFF,0xE0,0x00,0x01,0xFF,0xE0,0x00,0x00,
    0x00,0x07,0xFF,0xFF,0xFE,0x00,0x00,0x07,0xFF,0xF0,0x00,0x07,0xFF,0xF0,0x00,0x00,
    0x00,0x0F,0xF9,0xFD,0xFF,0x00,0x00,0x0F,0xFF,0xF8,0x00,0x0F,0xFF,0xF8,0x00,0x00,
    0x00,0x0F,0xF0,0xF0,0xFF,0x80,0x00,0x1F,0xFF,0xFC,0x00,0x1F,0xFF,0xFC,0x00,0x00,
    0x00,0x1F,0xF0,0x70,0x7F,0x80,0x00,0x3F,0xFF,0xFC,0x00,0x3F,0xFF,0xFC,0x00,0x00,
    0x00,0x1F,0xE1,0x70,0x7F,0x80,0x00,0x7F,0xFF,0xFE,0x00,0x7F,0xFF,0xFE,0x00,0x00,
    0x00,0x3F,0xE1,0x66,0x7F,0xC0,0x00,0xFF,0xFF,0xFE,0x00,0xFF,0xFF,0xFE,0x00,0x00,
    0x00,0x3F,0xE3,0xE0,0x7F,0xC0,0x01,0xFF,0xFF,0xFE,0x01,0xFF,0xFF,0xFE,0x00,0x00,
    0x00,0x3F,0xE0,0x60,0x7F,0xE0,0x01,0xFF,0xEF,0xFF,0x01,0xFF,0xEF,0xFF,0x00,0x00,
    0x00,0x7F,0xF0,0xF0,0x7F,0xC0,0x03,0xFF,0x87,0xFF,0x03,0xFF,0x87,0xFF,0x00,0x00,
    0x00,0x3F,0xF0,0xF8,0xFF,0xE0,0x03,0xFF,0x83,0xFF,0x83,0xFF,0x81,0xFF,0x80,0x00,
    0x00,0x7F,0xFF,0xFF,0xFF,0xE0,0x07,0xFF,0x01,0xFF,0x07,0xFF,0x01,0xFF,0x00,0x00,
    0x00,0x7F,0xFF,0xFF,0xFF,0xF0,0x07,0xFF,0x00,0xFF,0x87,0xFF,0x00,0xFF,0x80,0x00,
    0x00,0x7F,0xE0,0x00,0xBF,0xF0,0x0F,0xFE,0x00,0xFF,0x8F,0xFE,0x00,0xFF,0x80,0x00,
    0x00,0xFF,0x80,0x00,0x0F,0xF0,0x0F,0xFE,0x00,0xFF,0x8F,0xFE,0x00,0xFF,0x80,0x00,
    0x00,0xFE,0x00,0x00,0x07,0xF0,0x1F,0xFC,0x00,0x7F,0x9F,0xFC,0x00,0x7F,0x80,0x00,
    0x00,0xFF,0xB0,0x00,0x5F,0xF0,0x0F,0xFC,0x00,0xFF,0x8F,0xFC,0x00,0x7F,0x80,0x00,
    0x01,0xFF,0xF8,0x01,0xFF,0xF0,0x1F,0xF8,0x3D,0xFF,0x9F,0xFC,0x3D,0xFF,0x80,0x00,
    0x01,0xFF,0xFF,0xDF,0xFF,0xF8,0x0F,0xFC,0x37,0x0F,0x8F,0xFC,0x37,0x9F,0x80,0x00,
    0x07,0xFF,0xFF,0xFF,0xFF,0xF8,0x1F,0xF8,0x66,0x07,0x1F,0xF8,0x26,0x0F,0x80,0x00,
    0x07,0xFF,0xFF,0xFF,0xFF,0xFC,0x0F,0xFC,0x6C,0x63,0x8F,0xFC,0x66,0x63,0x80,0x00,
    0x1F,0xC7,0xFF,0xFF,0xFC,0xFE,0x0F,0xFE,0xEC,0x80,0x8F,0xFE,0xED,0x80,0x80,0x00,
    0x1F,0xC3,0xFF,0xFF,0xF0,0x7E,0x07,0xFF,0xFC,0x00,0x07,0xFF,0xFE,0x80,0x00,0x00,
    0x3F,0xC7,0xFF,0xFF,0x80,0x7F,0x07,0xFF,0xFE,0x00,0x07,0xFF,0xFE,0x00,0x00,0x00,
    0x3F,0xC3,0xFD,0xFA,0x00,0x3F,0x01,0xFF,0xFF,0x00,0x21,0xFF,0xFF,0x00,0x20,0x00,
    0x3F,0x87,0xF0,0x00,0x00,0x3F,0x00,0xFF,0xFF,0x00,0x40,0xFF,0xFF,0x00,0x40,0x00,
    0x7F,0x83,0xF8,0x00,0x00,0x3F,0x00,0x1F,0xFF,0x80,0x40,0x1F,0xFF,0xC0,0xC0,0x00,
    0x7F,0x87,0xF0,0x00,0x00,0x3F,0x00,0x00,0x00,0xC1,0x80,0x00,0x00,0xE3,0x80,0x00,
    0x7F,0xC3,0xF8,0x00,0x00,0x3F,0x80,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,
    0x7B,0x83,0xF0,0x00,0x00,0x3F,0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x1C,0x00,0x00,
    0x79,0xC1,0xF0,0x00,0x00,0x27,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x21,0xC0,0x00,0x00,0x00,0x67,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0xE0,0x00,0x00,0x00,0x40,0x07,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0x00,0xE0,0x00,0x00,0x00,0xC0,0x07,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0x00,0x30,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x38,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x9C,0x00,0x00,0x06,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x01,0x07,0x00,0x00,0x0C,0x60,0x07,0xF8,0x00,0x00,0x00,0x01,0xE0,0x00,0x00,0x70,
    0x01,0x81,0xC0,0x00,0x38,0x10,0x03,0x30,0x00,0x00,0x00,0x03,0x60,0x00,0x00,0x30,
    0x01,0x00,0xF8,0x00,0xE0,0x10,0x03,0x30,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x30,
    0x01,0x00,0x03,0xAD,0x00,0x10,0x01,0xE1,0xE3,0xF7,0xF8,0x06,0x01,0xE1,0xE1,0xF0,
    0x00,0xA0,0x0B,0x0E,0xA2,0xA0,0x01,0xE3,0x31,0xC3,0x30,0x06,0x03,0x33,0x33,0x30,
    0x00,0x00,0x54,0x00,0x00,0x00,0x01,0xE3,0xF1,0x81,0xE0,0x06,0xF3,0x33,0x33,0x30,
    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xE3,0x01,0x81,0xE0,0x06,0x63,0x33,0x33,0x30,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC3,0x31,0x80,0xC0,0x03,0x63,0x33,0x33,0x30,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC1,0xE3,0xE0,0xC0,0x01,0xC1,0xE1,0xE1,0xF8,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x80,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
********************************************/
void draw_picture(const unsigned char *ptr)
{
    unsigned char i,j,k;
    
    clr_screen();
    
    write_cmd(0x34);        //����չָ�
    i = 0x80;            
    for(j = 0;j < 32;j++)
    {
        write_cmd(i++);
        write_cmd(0x80);
        for(k = 0;k < 16;k++)
        {
            write_data(*ptr++);
        }
    }
    i = 0x80;
    for(j = 0;j < 32;j++)
    {
        write_cmd(i++);
        write_cmd(0x88);	   
        for(k = 0;k < 16;k++)
        {
            write_data(*ptr++);
        } 
    }  
    write_cmd(0x36);        //�򿪻�ͼ��ʾ
    write_cmd(0x30);        //�ص�����ָ�
}




/**************************************************
�������ƣ�set_white
��    �ܣ�����λ�÷���    
��    ��������y ��ʼx ����end_x  ģʽclear 0���� 1��ԭ
����ֵ  ����
**************************************************/
void set_white(unsigned char y,unsigned char x,unsigned char end_x,unsigned char clear)
{
    unsigned char i, j, white_x, white_y,white_end_x,clr_x,clr_y;		//
    white_end_x = (end_x-x+1);
    white_end_x <<= 1;
    write_cmd(0x36);                   //�򿪻�ͼģʽ
    if(y==1)
    {
        white_x = (0x80+x-1);
        white_y = 0x80;
        clr_x = 0x80;
        clr_y = 0x80;
    }
    else if(y==2)
    {
        white_x = (0x80+x-1);
        white_y = 0x90;
        clr_x = 0x80;
        clr_y = 0x90;
    }
    else if(y==3)
    {
        white_x = (0x88+x-1);
        white_y = 0x80;
        clr_x = 0x88;
        clr_y = 0x80;
    }
    else if(y==4)
    {
        white_x = (0x88+x-1);
        white_y = 0x90;
        clr_x = 0x88;
        clr_y = 0x90;
    }
    if(clear==0)                     //Ҫ����ʱ���Ƚ����е�Һ��ȫ����ɲ����ף��˴���ָy��
    {
        for(i=0;i<16;i++ )                //16��
        {
            write_cmd(clr_y++);	         //���û�ͼ����Y��ַ����0
            write_cmd(clr_x);		 //���û�ͼ����X��ַ����0
            for(j=0;j<16;j++)		 //
            {
                write_data(0x00);	 //��ɲ�����
                //nop();
            }
        }
    }
    //nop();
    for(i=0;i<16;i++ )			//16�У���Ϊ��16*16����
    {
        write_cmd(white_y++);		//���û�ͼ����Y��ַ����0
        write_cmd(white_x);		//���û�ͼ����X��ַ����0
        for(j=0;j<white_end_x;j++)	//
        {
            if(clear==1)
            {
                write_data(0x00);       //ȡ����һ�е�8����ķ��ף�Һ����ַ�Զ���1
                //���˴���ָһ��һ��Һ��������ɵ��У�
            }
            else
            {
                write_data(0xff);       //������һ�е�8���㣬Һ����ַ�Զ���1
                //���˴���ָһ��һ��Һ��������ɵ��У�
            }
            // nop();
        }
    }
    write_cmd(0x30);                   //�ص�����ģʽ
}




/**************************************************
�������ƣ�draw_dot
��    �ܣ�����λ�ô��    
��    ��������xy  color 0���� 1��ԭ
����ֵ  ����
**************************************************/
void draw_dot(unsigned char x,unsigned char y,unsigned char color)
{
    unsigned char  row,tier,tier_bit;
    unsigned char  read_old_h,read_old_l;
    write_cmd(0x34);
    write_cmd(0x36);
    tier=x>>4;
    tier_bit=x&0x0f;
    if(y<32)
        row=y;
    else
    {
        row=y-32;
        tier+=8;
    }
    write_cmd(row+0x80);
    write_cmd(tier+0x80);
    lcd_read_data();
    read_old_h=lcd_read_data();
    read_old_l=lcd_read_data();
    write_cmd(row+0x80);
    write_cmd(tier+0x80);
    
    if(tier_bit<8)
    {
        switch(color)
        {
            case 0:read_old_h&=(~(0x01<<(7-tier_bit)));break;
            case 1:read_old_h|=(0x01<<(7-tier_bit))   ;break;
            case 2:read_old_h^=(0x01<<(7-tier_bit))   ;break;
            default:break;
        }
        write_data(read_old_h);
        write_data(read_old_l);
        
    }
    else
    {
        switch(color)
        {
            case 0:read_old_l&=(~(0x01<<(15-tier_bit)));break;
            case 1:read_old_l|=(0x01<<(15-tier_bit))   ;break;
            case 2:read_old_l^=(0x01<<(15-tier_bit))   ;break;
            default:break;
        }
        
        write_data(read_old_h);
        write_data(read_old_l);
    }
    
    write_cmd(0x30);
    
}




/**************************************************
�������ƣ�draw_level_line
��    �ܣ�ˮƽ��    
��    ������ʼx0 x1 ��y����  color 0���� 1��ԭ
����ֵ  ����
**************************************************/
void draw_level_line(unsigned char x0,unsigned char x1,unsigned char y,unsigned char color)
{
    unsigned char temp;
    if(x0>x1)
    {
        temp=x1;
        x1=x0;
        x0=temp;
    }
    for(;x0<=x1;x0++)
    {
        draw_dot(x0,y,color);
    }
    
}



/**************************************************
�������ƣ�draw_vertical_line
��    �ܣ���ֱ��    
��    ������ʼy0 y1 ��x����  color 0 1
����ֵ  ����
**************************************************/
void draw_vertical_line(unsigned char y0,unsigned char y1,unsigned char x,unsigned char color)
{
    unsigned char temp;
    if(y0>y1)
    {
        temp=y1;
        y1=y0;
        y0=temp;
    }
    for(;y0<=y1;y0++)
        draw_dot(x,y0,color);
}




/**************************************************
�������ƣ�draw_line
��    �ܣ�������ֱ��    
��    ����startx starty endx endy    color 
����ֵ  ����
**************************************************/
void draw_line(unsigned char startx,unsigned char starty
               ,unsigned char endx,unsigned char endy,unsigned char color)
{
    int t,distance;
    int x=0,y=0,delta_x,delta_y;
    int incx,incy;
    delta_x=endx-startx;
    delta_y=endy-starty;
    
    if(delta_x>0)
    {
        incx=1;
    }
    else if(delta_x==0)
    {
        draw_vertical_line(startx,starty,endy,color);
        return;
    }
    else
    {
        incx= -1;
    }
    if(delta_y>0)
    {
        incy=1;
    }
    else if(delta_y==0)
    {
        draw_level_line(startx,endx,starty,color);
        return;
    }
    else
    {
        incy=-1;
    }
    delta_x=my_abs(delta_x);
    delta_y=my_abs(delta_y);
    if(delta_x>delta_y)
    {
        distance=delta_x;
    }
    else
    {
        distance=delta_y;
    }
    draw_dot(startx,starty,color);
    for(t=0;t<=distance+1;t++)
    {
        draw_dot(startx,starty,color);
        x+=delta_x;
        y+=delta_y;
        if(x>distance)
        {
            x-=distance;
            startx+=incx;
        }
        if(y>distance)
        {
            y-=distance;
            starty+=incy;
        }
        
    }
    
}




/**************************************************
�������ƣ�contin_line
��    �ܣ���������Y �����ߣ�YΪ0-63ע���������ʱ��ת��  �ߴ�startx��endx 0-127Ϊ���Χ    
��    ����startx  endx endy    color 
����ֵ  ����
**************************************************/
void contin_line(unsigned char startx ,unsigned char endx ,unsigned char Y)
{
     static unsigned char i=0,y0=0,y1=0,f=1; //i���߿�ʼ����
     if(f)                                     //���ڽ�startxֻ�ڵ�һ�δ��ݸ�i
     {
          f=0;
        i = startx;
     }
    // x0=i;
     y1=Y;                              //���ú�����ͼ�Σ���ȫ�����ˣ�
     if(i!=startx)                      //��֤����00��������һ��
          draw_line(i-1,y0,i,y1,1);
     //x1=x0;
     y0=y1;
      
     if(i++>=endx)  //���߽�������
     {
          
          i=startx;
          clear_GDRAM();
     }
}




/**************************************************
�������ƣ�draw_curve
��    �ܣ���һϵ���޷����ַ�����str���� ��С0-63ע�⴫��ǰ���������߿�ʼλ�� xstart �����Сsize    
��    �������startx    size  *str
����ֵ  ����
**************************************************/
void draw_curve(unsigned char xstart, unsigned char size ,unsigned char *str)
{
    static unsigned char i=0,endx=0,y0=0,y1=0,f=1;
    if(f) //ֻ����һ��
    {
        f=0;
        i = xstart;
    }
    
    endx = xstart + size;
    if(endx>=128)  //��֤ͼ�β����
        endx =127;
    for(;i<endx;i++)
    {
        y1 = str[i-xstart];
        if(i!=xstart)
            draw_line(i-1,y0,i,y1,1);
        y0 = y1;
    }
    i = xstart;  
    //Clear_GDRAM();
    
}





/**************************************************
�������ƣ�draw_circle
��    �ܣ�������Բ    
��    ����Բ������xy �뾶r    color 
����ֵ  ����
**************************************************/
void draw_circle(unsigned char x,unsigned char y,unsigned char r,unsigned char color)
{
    unsigned char a,b;
    float c;
    a = 0;
    b = r;
    //  c = 1.25 - r;
    c = 3 - 2*r;
    while(a < b)
    {
        draw_dot(x+a,y+b,color);
        draw_dot(x-a,y+b,color);
        draw_dot(x+a,y-b,color);
        draw_dot(x-a,y-b,color);
        
        draw_dot(x+b,y+a,color);
        draw_dot(x-b,y+a,color);
        draw_dot(x+b,y-a,color);
        draw_dot(x-b,y-a,color);
        
        if(c < 0)
        {
            c = c+4*a + 6; 
        }
        else
        {
            c= c + 4*(a - b) + 10;
            b-=1;
        }
        a = a + 1;  //���ƴ����
        
    }
    if(a == b)
    {
        draw_dot(x+a,y+b,color);
        draw_dot(x-a,y+b,color);
        draw_dot(x+a,y-b,color);
        draw_dot(x-a,y+b,color);
        
        draw_dot(x+b,y+a,color);
        draw_dot(x-b,y+a,color);
        draw_dot(x+b,y-a,color);
        draw_dot(x-b,y-a,color);
        
    }
}
/******************************************
��������: LCD12864_portini()
��    ��: 12864�˿ڳ�ʼ��
��    ��: ��
����ֵ  : ��
********************************************/
void LCD12864_portini()
{
    P9DIR = 0xFF;
    P5DIR |= BIT4;
	P5DIR |= BIT5;
    P8DIR |= BIT0;
	P8DIR |= BIT7;
}
/******************************************
��������: LCD12864_sendbyte
��    ��: ��12864Һ��д��һ���ֽ����ݻ���ָ��
��    ��: DatCmd--ΪiDatʱ�����ݣ�ΪiCmdʱ��ָ��
	  	  dByte--Ϊд��12864�����ݻ���ָ��
����ֵ  : ��
********************************************/
void LCD12864_sendbyte(unsigned char DatCmd, unsigned char dByte)
{
	if (DatCmd == iCmd)		//ָ�����
		LCDb_CLR_RS;
	else
		LCDb_SET_RS;

	LCDb_CLR_RW;			//д����
	LCDb_SET_E;
	LCDb_DO = dByte;		//д������
	_delay_us(50);
	LCDb_CLR_E;
}
/*******************************************
��������: LCD12864_sendstr
��    ��: ��12864Һ��д��һ���ַ���
��    ��: ptString--�ַ���ָ��
����ֵ  : ��
********************************************/
void LCD12864_sendstr(unsigned char *ptString)
{
	while((*ptString)!='\0')		 //�ַ���δ����һֱд
	{
		LCD12864_sendbyte(iDat, *ptString++);
	}
}
/*******************************************
��������: LCD12864_clear
��    ��: 12864Һ������
��    ��: ��
����ֵ  : ��
********************************************/
void LCD12864_clear(void)
{
	LCD12864_sendbyte(iCmd,LCDb_CLS);
	_delay_ms(2);// ����ָ��д���2ms ����ʱ�Ǻܱ�Ҫ��!!!
}

/*******************************************
��������: LCD12864_gotoXY
��    ��: �ƶ���ָ��λ��
��    ��: Row--ָ������
	  	  Col--ָ������
����ֵ  : ��
********************************************/
void LCD12864_gotoXY(unsigned char Row, unsigned char Col)
{
	switch (Row)		  //ѡ����
	{
		case 2:
			LCD12864_sendbyte(iCmd, LCDb_L2 + Col); break;	//д���2�е�ָ����
		case 3:
			LCD12864_sendbyte(iCmd, LCDb_L3 + Col); break;	//д���3�е�ָ����
		case 4:
			LCD12864_sendbyte(iCmd, LCDb_L4 + Col); break;	//д���4�е�ָ����
		default:
			LCD12864_sendbyte(iCmd, LCDb_L1 + Col); break;	//д���1�е�ָ����
	}
}
/*******************************************
��������: LCD12864_initial
��    ��: 12864Һ����ʼ��
��    ��: ��
����ֵ  : ��
********************************************/
void LCD12864_initial(void)
{
	LCDb_SET_RST;
	_delay_ms(100);				        // �ȴ��ڲ���λ
	LCDb_CLR_RST;
	LCD12864_portini();			        //�˿ڳ�ʼ��
	LCD12864_sendbyte(iCmd, LCDb_FUNCTION);	        //���ܡ�ģʽ�趨
	LCD12864_sendbyte(iCmd, LCDb_ON);		//����ʾ
	LCD12864_clear();				//����
	LCD12864_sendbyte(iCmd, LCDb_ENTRY);	        // ����ģʽ�趨
}
