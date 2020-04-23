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
		0x14,//��//011
		0x13,//΢��//012
		0x12,//����//013
		0x11,//��//014
		0x10,//����//015
		0x0f,//ǧ����//016
		0x0e,//�׺���//017
		0x0d,//������ʾ//018
		0x0c,//������ʾ//019
		0x0b,//Ƶ����ʾ//020
		0x0a//ʱ������ʾ//021
};

//****************��ʼ������ʾ
void voice_start()
{
	P6OUT = voice_data[17];
	_delay_ms(500);
	P6OUT=0xff;
	_delay_ms(800);
}
//*****************���ڲ�����ʾ
void voice_T()
{
	P6OUT = voice_data[18];
	_delay_ms(500);
	P6OUT=0xff;
	_delay_ms(900);
}
//*****************Ƶ�ʲ�����ʾ
void voice_fre()
{
	P6OUT = voice_data[19];
	_delay_ms(500);
	P6OUT=0xff;
	_delay_ms(900);
}
//*****************���ʱ�䲥����ʾ
void voice_tim()
{
	P6OUT = voice_data[20];
	_delay_ms(500);
	P6OUT=0xff;
	_delay_ms(900);
}
//****************������������
void voice_trans(char data[14])
{
	unsigned int i = 0;
	int flag = -1;

	while(data[i] != ' ')//ֻѰ�ҿո�ǰ������
	{
		if(data[i] != '0')//�ҵ���һ����Ϊ�����
		{
			flag = i;
			//OLED_ShowString(0,0,"No.1           ");
			//OLED_ShowNum(30,0,flag,4,16);
			break;
		}
		i++;
	}
	//������ֵ����
	if(flag == -1)//���������Ϊ0,��ֻ����һ����
	{
		P6OUT = voice_data[0];
		_delay_ms(500);
		P6OUT=0xff;
		_delay_ms(500);
	}
	else
	{
		//�Ѿ�ȡ���˵�һ����Ϊ�����������Ҫһ��С����֮ǰ����
		i = 0;
		while(data[i] != ' ')
		{
			if(data[i] == '.')//�ҵ�С����
			{
				if(flag > i)
				{
					flag = i;
					//�ȶ���С����ǰ��һ����
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

		i = flag;//��С���㿪ʼ����

		while(data[i] != ' ')//ֻѰ�ҿո�ǰ������
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
			i++;//����һ������
		}
	}
	i++;
	//���е�λ��ȡ
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

