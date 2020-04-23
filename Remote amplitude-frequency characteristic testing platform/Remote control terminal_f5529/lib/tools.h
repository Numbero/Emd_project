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
//����������
//*****************************
unsigned int i = 0;
unsigned int j = 0;
unsigned int scan_count = 0;

unsigned long trans_temp = 0;	//������ʱ����

unsigned long adc_data = 0;		//ADC����

unsigned char mode = 0;			//ģʽѡ��
unsigned char mode_pro = 0;		//��������ģʽѡ��
unsigned char enter = 0;		//ȷ��
unsigned char ButtonFlag = 0;
unsigned char send_sign = 0;


unsigned char uart_dat[32];			//UART���߻�����
unsigned char uart_num=0;			//UART���߻�����������
unsigned char endflag = 0;			//UARTֹͣλ��־
unsigned char comm_dat[32];			//UART���ݴ�����
unsigned char comm_len=0;			//UART���ݴ���������
unsigned char uart_buf[32];			//UART���ݷ��ͻ�����
unsigned char uart_temp;

unsigned char wlan_dat[300];		//uart_wlan���߻�����
unsigned char wlan_num=0;			//uart_wlan���߻�����������
unsigned char wlan_endflag = 0;		//uart_wlanֹͣλ��־
unsigned char wlan_comm_dat[300];	//uart_wlan���ݴ�����
unsigned char wlan_comm_len=0;		//uart_wlan���ݴ���������
unsigned char wlan_buf[32];			//uart_wlan���ݷ��ͻ�����
unsigned char wlan_temp;

//���������벢��ʾ����
unsigned char data_in[16];
unsigned char data_val[16];
unsigned char dot = 0;		//С����λ��
unsigned char unit = 0;		//��λ��־
unsigned char step_sign = 1;
unsigned long long_temp = 1;		//Ƶ��ת����ʱ����
unsigned long freq_temp;
unsigned long sour_freq;
unsigned long pack = 20000;

unsigned char val_data[40];	//��������ADC�ɼ���������
int val_temp[40];	//��������ADC�ɼ���������

double draw_dat;	//���߻�������



//*****************************
//��ʱ������
//*****************************

//ϵͳʱ�ӳ�ʼ������,����ʱ�ӱ�Ƶ��8M
void SysTime_Init()
{
	//**************************��ʱ��Ƶ������
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
	//**************************��ʱ������
    //TA0CCTL0 |= CCIE;						//�Ƚ��ж�ʹ��
    //TA0CCR0 = 16384;						//���ü���ֵ,500ms
    //TA0CTL = TASSEL_1 + TACLR;       		//AMCLK, upmode, clear TAR

	__bic_SR_register(SCG0);
	__delay_cycles(250000);
}
//ȫ����ʼ��
void sys_init()
{
	SysTime_Init();		//ϵͳʱ������
	UART_Init();		//UARTͨ�ų�ʼ��
	UART_WLAN_Init();	//UART_WLANͨ�ų�ʼ��
	key_init();			//������ʼ��
	//InitAD7705_ch0();	//16λADC��ʼ��
	//InitAD7705_ch1();
	DAC_12_Init();		//˫ͨ��DAC
	//************************************
	//LED��ʼ��
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;
	//�жϰ�����ʼ��
	P2IE |= BIT1; //����p2.1�����ж�
	P2IES |= BIT1; //����p2.1Ϊ�½����ж�
	P2IFG &= ~BIT1;//����p2.1Ϊ0 ���ж�����
	P2REN |= BIT1; //����p2.1Ϊ����������ʹ��
	P2OUT |= BIT1;
	//�жϰ�����ʼ��
	P1IE |= BIT1; //����p1.1�����ж�
	P1IES |= BIT1; //����p1.1Ϊ�½����ж�
	P1IFG &= ~BIT1;//����p1.1Ϊ0 ���ж�����
	P1REN |= BIT1; //����p1.1Ϊ����������ʹ��
	P1OUT |= BIT1;
	//LED��ʼ��
	P1DIR |= BIT0;
	P1OUT |= BIT0;
	//OLED��ʼ��
	OLED_Init();
	OLED_Clear();
	//OLED_ShowString(0,0,"I'm OLED.",16);
	//������ͨ�ų�ʼ��
	_delay_ms(80);//�ȴ���Ļ��ʼ��,����50ms����
	UART_Send_END();//����һ�ν�����������ϵ�����Ĵ����Ӳ�����
}

//�ڴ濽������
void MEMCOPY(unsigned char *dest,unsigned char *src,unsigned int len)
{
	while(len--)
	{
		*dest++ = *src++;
	}
}

//��Чֵ�任
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
//****************************Ƶ�����뺯������λΪMHz
unsigned long Freq_Input()
{
	memset(data_in,0,16);		//��ո��ֱ�־
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
	OLED_ShowString(0,6,data_val,16);		//��ʾ��_ _ _ _��
	while( enter == 0)
	{
		//��������
		key_event();
	    if(key_flag) //��������а����¼��Ļ���ˢ�¼�ֵ����ʾ
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
			 	 case 12:data_in[i]='.';dot = i;i++;break;		//ȷ��С����λ��

	  		 	 case 14:
	  		 		 unit = i;
	  		 		 data_in[i]='\0';
	  		 		 enter = 1;
	  		 		 i ++;break;		//ȷ��
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

//****************************�������뺯������λΪmV
unsigned long Pack_Input()
{
	memset(data_in,0,16);		//��ո��ֱ�־
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
		//��������
		key_event();
	    if(key_flag) //��������а����¼��Ļ���ˢ�¼�ֵ����ʾ
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
			 	 case 12:data_in[i]='.';dot = i;i++;break;		//ȷ��С����λ��

	  		 	 case 14:
	  		 		 unit = i;
	  		 		 data_in[i]='\0';
	  		 		 enter = 1;
	  		 		 i ++;break;		//ȷ��
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

//***************************�������Գ���
void key_test()
{
	//��������
	key_event();
	if(key_flag) //��������а����¼��Ļ���ˢ�¼�ֵ����ʾ
	{
		key_flag = 0;
		switch(key_val)
		{
		case 0:
			mode = 0;
			OLED_Clear();
			break;
		case 3:			//����Ƶ��
			enter = 1;
			mode = 1;
			OLED_Clear();
			break;
		case 7:			//���÷���
			enter = 1;
			mode = 2;
			OLED_Clear();
			break;
		case 11:		//�Զ�ɨƵ���ڴ���������ʾ
			enter = 1;
			mode = 3;
			OLED_Clear();

    		UART_WLAN_Send_Byte(0x01);		//��������֡
    		UART_WLAN_Send_Byte(0x03);		//ʹ�ӻ������Զ�ɨƵģʽ
    		UART_WLAN_Send_END();			//�����������
			break;

		case 12:		//�Զ�ɨƵ����ʾ��ʾ������
			enter = 1;
			mode = 4;
			OLED_Clear();

    		UART_WLAN_Send_Byte(0x01);		//��������֡
    		UART_WLAN_Send_Byte(0x03);		//ʹ�ӻ������Զ�ɨƵģʽ
    		UART_WLAN_Send_END();			//�����������
			break;

		case 15:		//����������
			enter = 1;
			mode = 0;
			OLED_Clear();
			scan_count = 0;

			UART_Send_Str("page 0");
			UART_Send_END();

			UART_Send_Byte(0x01);		//��������֡
			UART_Send_Byte(0x00);		//��ʼ���ӻ�
			UART_Send_END();			//�����������
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

    for(i=0;i<3;i++)
    {
    	str[n+3-i] = low%10 + '0';
    	low /= 10;
    }
    str[n+4] = '\0';

    return str;
}
//***************************������ͨ����Ӧ����
void screen_response()
{
	if(comm_len)
	{
		MEMCOPY(comm_dat,uart_dat,comm_len);
		switch(comm_dat[0])	//�ж��¼���������
		{
		case 0x65:			//���������¼�����
			switch(comm_dat[1])	//�ж�ҳ��
			{
			case 0:				//ҳ��0������ҳ
				switch(comm_dat[2])		//�жϿؼ�
				{
				case 3:	//����LED
					if(comm_dat[3] == 0x00)		//�����¼�
					{
						ButtonFlag	= ~ButtonFlag;
						if(ButtonFlag)
						{
							UART_Send_Str("g0.en=1");
							UART_Send_END();
							UART_Send_Str("g0.txt=\"������\"");
							UART_Send_END();
							P1OUT ^= 0x01;
						}
						else
						{
							UART_Send_Str("g0.en=1");
							UART_Send_END();
							UART_Send_Str("g0.txt=\"������\"");
							UART_Send_END();
							P1OUT ^= 0x01;
						}
					}
					break;
				case 2:							//��������
					if(comm_dat[3] == 0x00)		//�����¼�
					{
						UART_WLAN_Send_Byte(0x01);	//����֡
						UART_WLAN_Send_Byte(0xaa);	//��������ôӻ���������ģʽ
						UART_WLAN_Send_END();		//������
						OLED_ShowString(0,4,"linking...",16);
					}
					break;
				case 1:							//�ź�Դģʽ,�ɵ���Ƶ�ʺͷ���
					if(comm_dat[3] == 0x00)		//�����¼�
					{
						//mode = 2;
						//OLED_Clear();
					}
					break;
				case 6: 						//�Զ�ɨƵģʽ
					if(comm_dat[3] == 0x00)		//�����¼�
					{
						enter = 1;
						mode = 0;
						//enter = 0;
						OLED_Clear();

						//UART_WLAN_Send_Byte(0x01);		//��������֡
						//UART_WLAN_Send_Byte(0x02);		//ʹ�ӻ������Զ�ɨƵģʽ
						//UART_WLAN_Send_END();			//�����������
					}
					break;

				default:break;
				}
				break;

			case 1:			//ҳ��1�����ź�Դģʽ,�ɵ���Ƶ�ʺͷ���
				switch(comm_dat[2])	//�жϿؼ�
				{
				case 0:				//������ҳ
					enter = 1;
					mode = 0;
					OLED_Clear();

					_delay_ms(10);
					UART_WLAN_Send_Byte(0x01);		//��������֡
					UART_WLAN_Send_Byte(0x00);		//��ʼ���ӻ�
					UART_WLAN_Send_END();			//�����������

					OLED_Clear();
					break;

				case 3:				//����Ƶ������
					enter = 1;
					mode = 1;
					OLED_Clear();
					break;

				case 5:				//���з�������
					enter = 1;
					mode = 2;
					OLED_Clear();
					break;

				default:break;
				}
				break;

			case 2:			//ҳ��2�����Զ�ɨƵ������ʾ����
				switch(comm_dat[2])	//�жϿؼ�
				{
				case 0:				//������ҳ
					enter = 1;
					mode = 0;
					OLED_Clear();

					_delay_ms(10);
					UART_WLAN_Send_Byte(0x01);		//��������֡
					UART_WLAN_Send_Byte(0x00);		//��ʼ���ӻ�
					UART_WLAN_Send_END();			//�����������

					OLED_Clear();
					break;
				case 2:			//��ʼɨƵ
					mode = 3;
					enter = 0;
					OLED_Clear();

					UART_WLAN_Send_Byte(0x01);		//��������֡
					UART_WLAN_Send_Byte(0x03);		//ʹ�ӻ������Զ�ɨƵģʽ
					UART_WLAN_Send_END();			//�����������
					break;
				case 3:			//ֹͣɨƵ
					mode = 0;
					enter = 0;
					OLED_Clear();

					UART_WLAN_Send_Byte(0x01);		//��������֡
					UART_WLAN_Send_Byte(0x04);		//ʹ�ӻ�ֹͣ�Զ�ɨƵģʽ
					UART_WLAN_Send_END();			//�����������
					break;

				default:break;
				}
				break;

			default:break;
			}
			break;
		default:break;
		}
		comm_len = 0;	//��UART����λ����
	}
}

//****************������wlan���ݽ��д���************
void wlan_deal_with()
{
	if(wlan_comm_len)
	{
		MEMCOPY(wlan_comm_dat,wlan_dat,wlan_comm_len);
		{
			switch(wlan_comm_dat[0])		//�ж���������
			{
			case 0x00:						//����֡
				break;

			case 0x01:						//����֡
				switch(wlan_comm_dat[1])
				{
				case 0xaa:					//���ӳɹ�
					_delay_ms(10);
					UART_Send_Str("page 4");
					UART_Send_END();

					OLED_ShowString(0,6,"ling...",16);
					break;
				default:break;
				}
				break;

			case 0x03:						//��������֡
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

