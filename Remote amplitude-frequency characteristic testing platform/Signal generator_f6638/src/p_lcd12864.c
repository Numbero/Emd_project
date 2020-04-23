#include<msp430.h>
#include<stdio.h>
#include "p_lcd12864.h"


/**************************************************
函数名称：lcd_delay_n
功    能：大致延时 只要满足正常显示即可 根据实际调节
参    数：n
返回值  ：无
**************************************************/
void lcd_delay_n(unsigned int n)
{
    unsigned int i;
    for(i=n;i>0;i--)  _NOP();
}



/**************************************************
函数名称：my_abs
功    能：求绝对值   调用math。h中的abs总是有警告 于是自己写
参    数：a
返回值  ：无
**************************************************/
unsigned int my_abs(int a)
{
    if(a<0)
        a=-a;
    return a;
}



/**************************************************
函数名称：write_cmd
功    能：向液晶中写控制命令
参    数：cmd--控制命令
返回值  ：无
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
函数名称：write_data
功    能：向液晶中写数据
参    数：dat--显示数据
返回值  ：无
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
函数名称：lcd_read_data
功    能：读取12864中一个字节的数据
参    数：无
返回值  ：显示的数据
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
函数名称：lcd_setxy
功    能：设置显示位置    
参    数：X(1~16),Y(1~4)
返回值  ：无
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
函数名称：display_line
功    能：在指定位置显示字符串    
参    数：坐标x y 字符串str
返回值  ：无
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
函数名称：display_3digit
功    能：在指定位置开始显示三位数字    
参    数：坐标x y 数字d
返回值  ：无
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
函数名称：display_2digit
功    能：在指定位置开始显示两位数字    
参    数：坐标x y 数字d
返回值  ：无
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
函数名称：display_float
功    能：在指定位置开始显示浮点数    
参    数：坐标x/y,数字d,小数点后eps位
返回值  ：无
**************************************************/
void display_float(unsigned char x,unsigned char y,float d,unsigned int eps)
{
    char str[15];
    char ch[20];
    //**********************************************
    //sprintf(a,"%f",d);  //修改.f中间的数字可改变保留几位小数
    //不知道为什么本来是没问题的程序，但这里编译通不过
    //**********************************************
    int high;		//float_整数部分
    double low;		//float_小数部分
    int n,i;

    high = (int)d;
    low = d - high;

    n = 0;
    if(high == 0)
    {
    	ch[n] = '0';
    	n++;
    }
    while(high > 0)		//将整数部分转换为字符串
    {
    	ch[n] = '0' + high % 10;
        high = high / 10;
        n++;
    }
    n = n-1;	//n修正
    for(i = n;i >= 0;i--)	//将整数部分存入字符串中
    {
        str[n-i]=ch[i];
    }
    n = n - i;
    str[n] = '.';		//将小数点存入整数之后

    for(i=0;i < eps;i++)	//将小数位存入字符串中
    {
    	low *= 10;
    	n++;
    	str[n] = '0' + (int)low;
    	low -= (int)low;
    }
    if((int)(low*10) >= 5)
    	str[n] += 1;

    n++;	//在末尾存入字符串结束符
    str[n] = '\0';

    display_line(x,y,str);
}

//双精度显示函数
void display_double(unsigned char x,unsigned char y,double d,unsigned int eps)
{
    char str[15];
    char ch[20];

    long high;		//double_整数部分
    double low;		//double_小数部分
    int n,i;

    high = (long)d;
    low = d - high;

    n = 0;
    if(high == 0)
    {
    	ch[n] = '0';
    	n++;
    }
    while(high > 0)		//将整数部分转换为字符串
    {
    	ch[n] = '0' + high % 10;
        high = high / 10;
        n++;
    }
    n = n-1;	//n修正
    for(i = n;i >= 0;i--)	//将整数部分存入字符串中
    {
        str[n-i]=ch[i];
    }
    n = n - i;
    str[n] = '.';		//将小数点存入整数之后

    for(i=0;i < eps;i++)	//将小数位存入字符串中
    {
    	low *= 10;
    	n++;
    	str[n] = '0' + (long)low;
    	low -= (long)low;
    }
    if((long)(low*10) >= 5)
    	str[n] += 1;

    n++;	//在末尾存入字符串结束符
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
    while(d > 0)		//将整数部分转换为字符串
    {
	   	ch[n] = '0' + d % 10;
	    d = d / 10;
        n++;
    }
    n = n-1;	//n修正
	for(i = n;i >= 0;i--)	//将整数部分存入字符串中
    {
        str[n-i]=ch[i];
    }
	n++;	//在末尾存入字符串结束符
	str[n] = '\0';

	display_line(x,y,str);
}


/**************************************************
函数名称：clr_screen
功    能：清除显示    
参    数：无
返回值  ：无
**************************************************/
void clr_screen(void)
{
    write_cmd(0x01);
    //delay_ms(15);
}




/**************************************************
函数名称：display_page
功    能：显示一页字符    
参    数：字符数组s
返回值  ：无

格式const  char   *Page1[]=
{
     {"**【>>菜单<<】**"},
     {"拨号  通讯   QQ "},
     {"信息  设置  相机"},
     {"娱乐  备忘   UC "}
     
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
函数名称：init12864
功    能：初始化液晶模块
参    数：无
返回值  ：无
**************************************************/
void init12864(void)
{
    LCD_DataOut;
    LCD_CMDOut;                  //液晶控制端口设置为输出
    
    lcd_delay_n(50);
    write_cmd(0x30);            //基本指令集
    lcd_delay_n(50);
    write_cmd(0x30);            //选择8bit数据流
    lcd_delay_n(50);
    write_cmd(0x02);            //地址归位
    lcd_delay_n(50);
    write_cmd(0x0c);            //整体显示打开,游标关闭
    lcd_delay_n(50);
    write_cmd(0x01);            //清除显示
    lcd_delay_n(50);
    write_cmd(0x06);            //游标右移
    lcd_delay_n(50);
    write_cmd(0x80);            //设定显示的起始地址
    lcd_delay_n(2000);
}





/**************************************************
函数名称：Clear_GDRAM
功    能：清除液晶GDRAM中的随机数据
参    数：无
返回值  ：无
**************************************************/
void clear_GDRAM(void)
{
    unsigned char i,j,k;
    
    write_cmd(0x34);               //打开扩展指令集
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
    write_cmd(0x30);                  //回到基本指令集
}




/*******************************************
函数名称:draw_picture
功    能:在整个液晶屏幕上画图
参    数:图片数组ptr
返回值  :无
格    式： 宽度x高度=128x64 
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
    
    write_cmd(0x34);        //打开扩展指令集
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
    write_cmd(0x36);        //打开绘图显示
    write_cmd(0x30);        //回到基本指令集
}




/**************************************************
函数名称：set_white
功    能：任意位置反白    
参    数：行数y 起始x 结束end_x  模式clear 0反白 1复原
返回值  ：无
**************************************************/
void set_white(unsigned char y,unsigned char x,unsigned char end_x,unsigned char clear)
{
    unsigned char i, j, white_x, white_y,white_end_x,clr_x,clr_y;		//
    white_end_x = (end_x-x+1);
    white_end_x <<= 1;
    write_cmd(0x36);                   //打开绘图模式
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
    if(clear==0)                     //要反白时，先将整行的液晶全部清成不反白（此处行指y）
    {
        for(i=0;i<16;i++ )                //16行
        {
            write_cmd(clr_y++);	         //设置绘图区的Y地址坐标0
            write_cmd(clr_x);		 //设置绘图区的X地址坐标0
            for(j=0;j<16;j++)		 //
            {
                write_data(0x00);	 //清成不反白
                //nop();
            }
        }
    }
    //nop();
    for(i=0;i<16;i++ )			//16行，因为是16*16汉字
    {
        write_cmd(white_y++);		//设置绘图区的Y地址坐标0
        write_cmd(white_x);		//设置绘图区的X地址坐标0
        for(j=0;j<white_end_x;j++)	//
        {
            if(clear==1)
            {
                write_data(0x00);       //取消这一行的8个点的反白，液晶地址自动加1
                //（此处行指一个一个液晶点所组成的行）
            }
            else
            {
                write_data(0xff);       //反白这一行的8个点，液晶地址自动加1
                //（此处行指一个一个液晶点所组成的行）
            }
            // nop();
        }
    }
    write_cmd(0x30);                   //回到基本模式
}




/**************************************************
函数名称：draw_dot
功    能：任意位置打点    
参    数：坐标xy  color 0反白 1复原
返回值  ：无
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
函数名称：draw_level_line
功    能：水平线    
参    数：起始x0 x1 和y坐标  color 0反白 1复原
返回值  ：无
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
函数名称：draw_vertical_line
功    能：垂直线    
参    数：起始y0 y1 和x坐标  color 0 1
返回值  ：无
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
函数名称：draw_line
功    能：画任意直线    
参    数：startx starty endx endy    color 
返回值  ：无
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
函数名称：contin_line
功    能：连续输入Y 连成线，Y为0-63注意输入进来时做转换  线从startx至endx 0-127为最大范围    
参    数：startx  endx endy    color 
返回值  ：无
**************************************************/
void contin_line(unsigned char startx ,unsigned char endx ,unsigned char Y)
{
     static unsigned char i=0,y0=0,y1=0,f=1; //i连线开始坐标
     if(f)                                     //用于将startx只在第一次传递给i
     {
          f=0;
        i = startx;
     }
    // x0=i;
     y1=Y;                              //画该函数的图形，完全连接了，
     if(i!=startx)                      //保证不与00坐标连到一起
          draw_line(i-1,y0,i,y1,1);
     //x1=x0;
     y0=y1;
      
     if(i++>=endx)  //连线结束坐标
     {
          
          i=startx;
          clear_GDRAM();
     }
}




/**************************************************
函数名称：draw_curve
功    能：将一系列无符号字符数组str的数 大小0-63注意传递前做处理，曲线开始位置 xstart 数组大小size    
参    数：起点startx    size  *str
返回值  ：无
**************************************************/
void draw_curve(unsigned char xstart, unsigned char size ,unsigned char *str)
{
    static unsigned char i=0,endx=0,y0=0,y1=0,f=1;
    if(f) //只传递一次
    {
        f=0;
        i = xstart;
    }
    
    endx = xstart + size;
    if(endx>=128)  //保证图形不溢出
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
函数名称：draw_circle
功    能：画任意圆    
参    数：圆心坐标xy 半径r    color 
返回值  ：无
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
        a = a + 1;  //控制打点间隔
        
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
函数名称: LCD12864_portini()
功    能: 12864端口初始化
参    数: 无
返回值  : 无
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
函数名称: LCD12864_sendbyte
功    能: 向12864液晶写入一个字节数据或者指令
参    数: DatCmd--为iDat时是数据，为iCmd时是指令
	  	  dByte--为写入12864的数据或者指令
返回值  : 无
********************************************/
void LCD12864_sendbyte(unsigned char DatCmd, unsigned char dByte)
{
	if (DatCmd == iCmd)		//指令操作
		LCDb_CLR_RS;
	else
		LCDb_SET_RS;

	LCDb_CLR_RW;			//写操作
	LCDb_SET_E;
	LCDb_DO = dByte;		//写入数据
	_delay_us(50);
	LCDb_CLR_E;
}
/*******************************************
函数名称: LCD12864_sendstr
功    能: 向12864液晶写入一个字符串
参    数: ptString--字符串指针
返回值  : 无
********************************************/
void LCD12864_sendstr(unsigned char *ptString)
{
	while((*ptString)!='\0')		 //字符串未结束一直写
	{
		LCD12864_sendbyte(iDat, *ptString++);
	}
}
/*******************************************
函数名称: LCD12864_clear
功    能: 12864液晶清屏
参    数: 无
返回值  : 无
********************************************/
void LCD12864_clear(void)
{
	LCD12864_sendbyte(iCmd,LCDb_CLS);
	_delay_ms(2);// 清屏指令写入后，2ms 的延时是很必要的!!!
}

/*******************************************
函数名称: LCD12864_gotoXY
功    能: 移动到指定位置
参    数: Row--指定的行
	  	  Col--指定的列
返回值  : 无
********************************************/
void LCD12864_gotoXY(unsigned char Row, unsigned char Col)
{
	switch (Row)		  //选择行
	{
		case 2:
			LCD12864_sendbyte(iCmd, LCDb_L2 + Col); break;	//写入第2行的指定列
		case 3:
			LCD12864_sendbyte(iCmd, LCDb_L3 + Col); break;	//写入第3行的指定列
		case 4:
			LCD12864_sendbyte(iCmd, LCDb_L4 + Col); break;	//写入第4行的指定列
		default:
			LCD12864_sendbyte(iCmd, LCDb_L1 + Col); break;	//写入第1行的指定列
	}
}
/*******************************************
函数名称: LCD12864_initial
功    能: 12864液晶初始化
参    数: 无
返回值  : 无
********************************************/
void LCD12864_initial(void)
{
	LCDb_SET_RST;
	_delay_ms(100);				        // 等待内部复位
	LCDb_CLR_RST;
	LCD12864_portini();			        //端口初始化
	LCD12864_sendbyte(iCmd, LCDb_FUNCTION);	        //功能、模式设定
	LCD12864_sendbyte(iCmd, LCDb_ON);		//打开显示
	LCD12864_clear();				//清屏
	LCD12864_sendbyte(iCmd, LCDb_ENTRY);	        // 输入模式设定
}
