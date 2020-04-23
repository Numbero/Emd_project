#include <msp430.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "delay.h"
#include "oled_IIC_13.h"
#include "bmp.h"
#include "key.h"
#include "UART.h"
#include "extre_tool.h"
#include "DAC_12.h"

#define ulong unsigned long
#define uchar unsigned char


//******************************
//变量声明区
//*****************************
unsigned int i = 0;
unsigned int j = 0;
unsigned int scan_count = 0;

unsigned long trans_temp = 0;	//传输临时变量

unsigned long adc_data = 0;		//ADC数据

unsigned char mode = 0;			//模式选择
unsigned char mode_pro = 0;		//主从连接模式选择
unsigned char enter = 0;		//确定
unsigned char ButtonFlag = 0;
unsigned char send_sign = 0;


unsigned char uart_dat[32];			//UART总线缓冲区
unsigned char uart_num=0;			//UART总线缓冲区计数器
unsigned char endflag = 0;			//UART停止位标志
unsigned char comm_dat[32];			//UART数据处理区
unsigned char comm_len=0;			//UART数据处理区长度
unsigned char uart_buf[32];			//UART数据发送缓存区
unsigned char uart_temp;

unsigned char wlan_dat[300];		//uart_wlan总线缓冲区
unsigned char wlan_num=0;			//uart_wlan总线缓冲区计数器
unsigned char wlan_endflag = 0;		//uart_wlan停止位标志
unsigned char wlan_comm_dat[300];	//uart_wlan数据处理区
unsigned char wlan_comm_len=0;		//uart_wlan数据处理区长度
unsigned char wlan_buf[32];			//uart_wlan数据发送缓存区
unsigned char wlan_temp;

//长整型输入并显示变量
unsigned char data_in[16];
unsigned char data_val[16];
unsigned char dot = 0;		//小数点位置
unsigned char unit = 0;		//单位标志
unsigned char step_sign = 1;
unsigned long long_temp = 1;		//频率转换临时变量
unsigned long freq_temp;
unsigned long sour_freq;
unsigned long pack = 20000;

unsigned char val_data[40];	//用来保存ADC采集到的数据
int val_temp[40];	//用来保存ADC采集到的数据

double draw_dat;	//曲线绘制增量



//*****************************
//临时函数区
//*****************************

//系统时钟初始化函数,将主时钟倍频到8M
void SysTime_Init()
{
	//**************************主时钟频率设置
	volatile unsigned int i;
	UCSCTL3 = SELREF_2;
	UCSCTL4 |= SELA_2;
	UCSCTL0 = 0x0000;
	do
	{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
		SFRIFG1 &= ~OFIFG;
	}while (SFRIFG1&OFIFG);
	__bis_SR_register(SCG0);
	UCSCTL1 = DCORSEL_5;
	UCSCTL2 |= 249;
	//**************************定时器设置
    //TA0CCTL0 |= CCIE;						//比较中断使能
    //TA0CCR0 = 16384;						//设置计数值,500ms
    //TA0CTL = TASSEL_1 + TACLR;       		//AMCLK, upmode, clear TAR

	__bic_SR_register(SCG0);
	__delay_cycles(250000);
}
//全部初始化
void sys_init()
{
	SysTime_Init();		//系统时钟设置
	UART_Init();		//UART通信初始化
	UART_WLAN_Init();	//UART_WLAN通信初始化
	key_init();			//按键初始化
	//InitAD7705_ch0();	//16位ADC初始化
	//InitAD7705_ch1();
	DAC_12_Init();		//双通道DAC
	//************************************
	//LED初始化
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;
	//中断按键初始化
	P2IE |= BIT1; //设置p2.1可以中断
	P2IES |= BIT1; //设置p2.1为下降沿中断
	P2IFG &= ~BIT1;//设置p2.1为0 无中断请求
	P2REN |= BIT1; //设置p2.1为上下拉电阻使能
	P2OUT |= BIT1;
	//中断按键初始化
	P1IE |= BIT1; //设置p1.1可以中断
	P1IES |= BIT1; //设置p1.1为下降沿中断
	P1IFG &= ~BIT1;//设置p1.1为0 无中断请求
	P1REN |= BIT1; //设置p1.1为上下拉电阻使能
	P1OUT |= BIT1;
	//LED初始化
	P1DIR |= BIT0;
	P1OUT |= BIT0;
	//OLED初始化
	OLED_Init();
	OLED_Clear();
	//OLED_ShowString(0,0,"I'm OLED.",16);
	//触摸屏通信初始化
	_delay_ms(80);//等待屏幕初始化,建议50ms以上
	UART_Send_END();//发送一次结束符，清除上电产生的串口杂波数据
}

//内存拷贝函数
void MEMCOPY(unsigned char *dest,unsigned char *src,unsigned int len)
{
	while(len--)
	{
		*dest++ = *src++;
	}
}

//有效值变换
double Val_trans(unsigned long adc_data)
{
	double temp = 0;
	temp = 0.02447 * exp(0.0005921 * adc_data);
	return temp;
}
double Pack_Trans(unsigned int data)
{
	double temp;
	temp = 0.02487 * exp(data*0.0005921);

	return temp;
}
//****************************频率输入函数，单位为MHz
unsigned long Freq_Input()
{
	memset(data_in,0,16);		//清空各种标志
	memset(data_val,0,16);
	long_temp = 1;
	enter = 0;
	dot = 0;
	unit = 0;
	ulong Freq = 0;

	for(i=0;i<8;i++)
		data_val[i]='_';
	data_val[i] = '\0';

	i = 0;
	OLED_ShowString(0,6,data_val,16);		//显示“_ _ _ _”
	while( enter == 0)
	{
		//按键程序
		key_event();
	    if(key_flag) //如果捕获到有按键事件的话，刷新键值，显示
	    {
	    	 key_flag = 0;
	    	 switch(key_val)
	    	 {
	    	 	 case 0:data_in[i]='1';i++;break;
	    	 	 case 1:data_in[i]='2';i++;break;
	    	 	 case 2:data_in[i]='3';i++;break;
	    	 	 case 4:data_in[i]='4';i++;break;
	    	 	 case 5:data_in[i]='5';i++;break;
	    	 	 case 6:data_in[i]='6';i++;break;
	    	 	 case 8:data_in[i]='7';i++;break;
	    	 	 case 9:data_in[i]='8';i++;break;
	    	 	 case 10:data_in[i]='9';i++;break;
			 	 case 13:data_in[i]='0';i++;break;
			 	 case 12:data_in[i]='.';dot = i;i++;break;		//确定小数点位置

	  		 	 case 14:
	  		 		 unit = i;
	  		 		 data_in[i]='\0';
	  		 		 enter = 1;
	  		 		 i ++;break;		//确定
	  		 	 default:break;
	    	 }
	    	 data_val[i-1] = data_in[i-1];
	    }
	    OLED_ShowString(0,6,data_val,16);
	}
	strcat(data_val," MHz");
	//display_2digit(0,1,dot);
	//display_2digit(4,1,unit);
	enter = 0;
	for(i=0;i<unit;i++)
	{
		if(i < dot)
		{
			for(j = 0;j < (dot-i+5);j++)
				long_temp = long_temp * 10;
			Freq += (data_in[i] - '0') * long_temp;
			long_temp = 1;
		}
		else if(i > dot)
		{
			for(j = 0;j < 6-i+dot;j++)
				long_temp = long_temp * 10;
			Freq += (data_in[i] - '0') * long_temp;
			long_temp = 1;
		}
	}
	return Freq;
}

//****************************幅度输入函数，单位为mV
unsigned long Pack_Input()
{
	memset(data_in,0,16);		//清空各种标志
	memset(data_val,0,16);
	long_temp = 1;
	enter = 0;
	dot = 0;
	unit = 0;
	ulong Pack = 0;

	for(i=0;i<8;i++)
		data_val[i]='_';
	data_val[i] = '\0';

	i = 0;
	OLED_ShowString(0,6,data_val,16);
	while( enter == 0)
	{
		//按键程序
		key_event();
	    if(key_flag) //如果捕获到有按键事件的话，刷新键值，显示
	    {
	    	 key_flag = 0;
	    	 switch(key_val)
	    	 {
	    	 	 case 0:data_in[i]='1';i++;break;
	    	 	 case 1:data_in[i]='2';i++;break;
	    	 	 case 2:data_in[i]='3';i++;break;
	    	 	 case 4:data_in[i]='4';i++;break;
	    	 	 case 5:data_in[i]='5';i++;break;
	    	 	 case 6:data_in[i]='6';i++;break;
	    	 	 case 8:data_in[i]='7';i++;break;
	    	 	 case 9:data_in[i]='8';i++;break;
	    	 	 case 10:data_in[i]='9';i++;break;
			 	 case 13:data_in[i]='0';i++;break;
			 	 case 12:data_in[i]='.';dot = i;i++;break;		//确定小数点位置

	  		 	 case 14:
	  		 		 unit = i;
	  		 		 data_in[i]='\0';
	  		 		 enter = 1;
	  		 		 i ++;break;		//确定
	  		 	 default:break;
	    	 }
	    	 data_val[i-1] = data_in[i-1];
	    }
	    OLED_ShowString(0,6,data_val,16);
	}
	strcat(data_val," mV");
	//display_2digit(0,1,dot);
	//display_2digit(4,1,unit);
	enter = 0;
	for(i=0;i<unit;i++)
	{
		if(i < dot)
		{
			for(j = 0;j < (dot-i+2);j++)
				long_temp = long_temp * 10;
			Pack += (data_in[i] - '0') * long_temp;
			long_temp = 1;
		}
		else if(i > dot)
		{
			for(j = 0;j < 3-i+dot;j++)
				long_temp = long_temp * 10;
			Pack += (data_in[i] - '0') * long_temp;
			long_temp = 1;
		}
	}
	return Pack;
}

//***************************按键测试程序
void key_test()
{
	//按键程序
	key_event();
	if(key_flag) //如果捕获到有按键事件的话，刷新键值，显示
	{
		key_flag = 0;
		switch(key_val)
		{
		case 0:
			mode = 0;
			OLED_Clear();
			break;
		case 3:			//设置频率
			enter = 1;
			mode = 1;
			OLED_Clear();
			break;
		case 7:			//设置幅度
			enter = 1;
			mode = 2;
			OLED_Clear();
			break;
		case 11:		//自动扫频，在触摸屏上显示
			enter = 1;
			mode = 3;
			OLED_Clear();

    		UART_WLAN_Send_Byte(0x01);		//发送命令帧
    		UART_WLAN_Send_Byte(0x03);		//使从机进入自动扫频模式
    		UART_WLAN_Send_END();			//发送命令结束
			break;

		case 12:		//自动扫频，显示在示波器上
			enter = 1;
			mode = 4;
			OLED_Clear();

    		UART_WLAN_Send_Byte(0x01);		//发送命令帧
    		UART_WLAN_Send_Byte(0x03);		//使从机进入自动扫频模式
    		UART_WLAN_Send_END();			//发送命令结束
			break;

		case 15:		//返回主界面
			enter = 1;
			mode = 0;
			OLED_Clear();
			scan_count = 0;

			UART_Send_Str("page 0");
			UART_Send_END();

			UART_Send_Byte(0x01);		//发送命令帧
			UART_Send_Byte(0x00);		//初始化从机
			UART_Send_END();			//发送命令结束
			break;

		default:break;
		}
	}
}

unsigned char* Freq_MHz_to_Char(unsigned long d)
{
	unsigned char str[20];
    unsigned char ch[20];
    int i,n;
    unsigned long high,low;

    high = d/1000000;
    low = (d - high*1000000)/1000;

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

    for(i=0;i<3;i++)
    {
    	str[n+3-i] = low%10 + '0';
    	low /= 10;
    }
    str[n+4] = '\0';

    return str;
}

unsigned char* Freq_kHz_to_Char(unsigned long d)
{
	unsigned char str[20];
    unsigned char ch[20];
    int i,n;
    unsigned long high,low;

    high = d/1000;
    low = d - high*1000;

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

    for(i=0;i<3;i++)
    {
    	str[n+3-i] = low%10 + '0';
    	low /= 10;
    }
    str[n+4] = '\0';

    return str;
}
//***************************触摸屏通信响应程序
void screen_response()
{
	if(comm_len)
	{
		MEMCOPY(comm_dat,uart_dat,comm_len);
		switch(comm_dat[0])	//判断事件触发类型
		{
		case 0x65:			//触摸热区事件返回
			switch(comm_dat[1])	//判断页面
			{
			case 0:				//页面0——主页
				switch(comm_dat[2])		//判断控件
				{
				case 3:	//控制LED
					if(comm_dat[3] == 0x00)		//按下事件
					{
						ButtonFlag	= ~ButtonFlag;
						if(ButtonFlag)
						{
							UART_Send_Str("g0.en=1");
							UART_Send_END();
							UART_Send_Str("g0.txt=\"灯亮了\"");
							UART_Send_END();
							P1OUT ^= 0x01;
						}
						else
						{
							UART_Send_Str("g0.en=1");
							UART_Send_END();
							UART_Send_Str("g0.txt=\"灯已灭\"");
							UART_Send_END();
							P1OUT ^= 0x01;
						}
					}
					break;
				case 2:							//发起连接
					if(comm_dat[3] == 0x00)		//按下事件
					{
						UART_WLAN_Send_Byte(0x01);	//命令帧
						UART_WLAN_Send_Byte(0xaa);	//连接命令，让从机进入联机模式
						UART_WLAN_Send_END();		//结束符
						OLED_ShowString(0,4,"linking...",16);
					}
					break;
				case 1:							//信号源模式,可调节频率和幅度
					if(comm_dat[3] == 0x00)		//按下事件
					{
						//mode = 2;
						//OLED_Clear();
					}
					break;
				case 6: 						//自动扫频模式
					if(comm_dat[3] == 0x00)		//按下事件
					{
						enter = 1;
						mode = 0;
						//enter = 0;
						OLED_Clear();

						//UART_WLAN_Send_Byte(0x01);		//发送命令帧
						//UART_WLAN_Send_Byte(0x02);		//使从机进入自动扫频模式
						//UART_WLAN_Send_END();			//发送命令结束
					}
					break;

				default:break;
				}
				break;

			case 1:			//页面1——信号源模式,可调节频率和幅度
				switch(comm_dat[2])	//判断控件
				{
				case 0:				//返回主页
					enter = 1;
					mode = 0;
					OLED_Clear();

					_delay_ms(10);
					UART_WLAN_Send_Byte(0x01);		//发送命令帧
					UART_WLAN_Send_Byte(0x00);		//初始化从机
					UART_WLAN_Send_END();			//发送命令结束

					OLED_Clear();
					break;

				case 3:				//进行频率设置
					enter = 1;
					mode = 1;
					OLED_Clear();
					break;

				case 5:				//进行幅度设置
					enter = 1;
					mode = 2;
					OLED_Clear();
					break;

				default:break;
				}
				break;

			case 2:			//页面2——自动扫频波形显示界面
				switch(comm_dat[2])	//判断控件
				{
				case 0:				//返回主页
					enter = 1;
					mode = 0;
					OLED_Clear();

					_delay_ms(10);
					UART_WLAN_Send_Byte(0x01);		//发送命令帧
					UART_WLAN_Send_Byte(0x00);		//初始化从机
					UART_WLAN_Send_END();			//发送命令结束

					OLED_Clear();
					break;
				case 2:			//开始扫频
					mode = 3;
					enter = 0;
					OLED_Clear();

					UART_WLAN_Send_Byte(0x01);		//发送命令帧
					UART_WLAN_Send_Byte(0x03);		//使从机进入自动扫频模式
					UART_WLAN_Send_END();			//发送命令结束
					break;
				case 3:			//停止扫频
					mode = 0;
					enter = 0;
					OLED_Clear();

					UART_WLAN_Send_Byte(0x01);		//发送命令帧
					UART_WLAN_Send_Byte(0x04);		//使从机停止自动扫频模式
					UART_WLAN_Send_END();			//发送命令结束
					break;

				default:break;
				}
				break;

			default:break;
			}
			break;
		default:break;
		}
		comm_len = 0;	//将UART数组位置零
	}
}

//****************接收自wlan数据进行处理************
void wlan_deal_with()
{
	if(wlan_comm_len)
	{
		MEMCOPY(wlan_comm_dat,wlan_dat,wlan_comm_len);
		{
			switch(wlan_comm_dat[0])		//判断数据类型
			{
			case 0x00:						//数据帧
				break;

			case 0x01:						//命令帧
				switch(wlan_comm_dat[1])
				{
				case 0xaa:					//连接成功
					_delay_ms(10);
					UART_Send_Str("page 4");
					UART_Send_END();

					OLED_ShowString(0,6,"ling...",16);
					break;
				default:break;
				}
				break;

			case 0x03:						//幅度数据帧
				for(i=0;i<40;i++)
				{
					val_temp[i] = wlan_comm_dat[i*2+1];
					val_temp[i] = val_temp[i]<<8;
					val_temp[i] += wlan_comm_dat[i*2+2];
				}
				break;

				default:break;
			}
		}
		wlan_comm_len = 0;
	}
}

