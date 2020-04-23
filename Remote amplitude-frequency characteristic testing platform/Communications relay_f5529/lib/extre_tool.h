#include	<msp430.h>
#include "delay.h"

unsigned int const voice_data[]=
{
		0x1e,//0//001
		0x1d,//1//002
		0x1c,//2//003
		0x1b,//3//004
		0x1a,//4//005
		0x19,//5//006
		0x18,//6//007
		0x17,//7//008
		0x16,//8//009
		0x15,//9//010
		0x14,//点//011
		0x13,//微秒//012
		0x12,//毫秒//013
		0x11,//秒//014
		0x10,//赫兹//015
		0x0f,//千赫兹//016
		0x0e,//兆赫兹//017
		0x0d,//开机提示//018
		0x0c,//周期提示//019
		0x0b,//频率提示//020
		0x0a//时间间隔提示//021
};

//****************开始语音提示
void voice_start()
{
	P6OUT = voice_data[17];
	_delay_ms(500);
	P6OUT=0xff;
	_delay_ms(800);
}
//*****************周期播报提示
void voice_T()
{
	P6OUT = voice_data[18];
	_delay_ms(500);
	P6OUT=0xff;
	_delay_ms(900);
}
//*****************频率播报提示
void voice_fre()
{
	P6OUT = voice_data[19];
	_delay_ms(500);
	P6OUT=0xff;
	_delay_ms(900);
}
//*****************间隔时间播报提示
void voice_tim()
{
	P6OUT = voice_data[20];
	_delay_ms(500);
	P6OUT=0xff;
	_delay_ms(900);
}
//****************语音播报程序
void voice_trans(char data[14])
{
	unsigned int i = 0;
	int flag = -1;

	while(data[i] != ' ')//只寻找空格前的数据
	{
		if(data[i] != '0')//找到第一个不为零的数
		{
			flag = i;
			//OLED_ShowString(0,0,"No.1           ");
			//OLED_ShowNum(30,0,flag,4,16);
			break;
		}
		i++;
	}
	//进行数值播报
	if(flag == -1)//如果该数字为0,则只读第一个零
	{
		P6OUT = voice_data[0];
		_delay_ms(500);
		P6OUT=0xff;
		_delay_ms(500);
	}
	else
	{
		//已经取得了第一个不为零的数，我需要一个小数点之前的数
		i = 0;
		while(data[i] != ' ')
		{
			if(data[i] == '.')//找到小数点
			{
				if(flag > i)
				{
					flag = i;
					//先读出小数点前的一个零
					P6OUT = voice_data[0];
					//OLED_ShowNum(30,0,000,4,16);
					_delay_ms(500);
					P6OUT=0xff;
					_delay_ms(500);
				}
				break;
			}
			i++;
		}

		i = flag;//从小数点开始读数

		while(data[i] != ' ')//只寻找空格前的数据
		{
			switch(data[i])
			{
				case '0':P6OUT=voice_data[0];break;
				case '1':P6OUT=voice_data[1];break;
				case '2':P6OUT=voice_data[2];break;
				case '3':P6OUT=voice_data[3];break;
				case '4':P6OUT=voice_data[4];break;
				case '5':P6OUT=voice_data[5];break;
				case '6':P6OUT=voice_data[6];break;
				case '7':P6OUT=voice_data[7];break;
				case '8':P6OUT=voice_data[8];break;
				case '9':P6OUT=voice_data[9];break;
				case '.':P6OUT=voice_data[10];break;
				default:break;
			}
			_delay_ms(500);
			P6OUT=0xff;
			_delay_ms(600);
			i++;//读下一个数字
		}
	}
	i++;
	//进行单位读取
	switch(data[i])
	{
		case 'u':P6OUT=voice_data[11];break;
		case 'm':P6OUT=voice_data[12];break;
		case 's':P6OUT=voice_data[13];break;
		case 'H':P6OUT=voice_data[14];break;
		case 'k':P6OUT=voice_data[15];break;
		case 'M':P6OUT=voice_data[16];break;
		default:break;
	}
	_delay_ms(500);
	P6OUT=0xff;
	_delay_ms(800);
}

