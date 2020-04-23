#include <msp430.h>
#include <math.h>
#include <string.h>
#include "AD9854_Parallel.h"
#include "general.h"
#include "p_lcd12864.h"
#include "delay.h"
#include "key.h"
#include "AD_7705.h"
#include "dac.h"
#include "UART.h"
#include "MY_SPI.h"
#include "deal_with.h"

/* MSP430ϵͳʱ��8M��ʼ�� */
void SysTime_Init();
/* �ڴ濽������ */
void MEMCOPY(unsigned char *dest, unsigned char *src, unsigned int len);
/* ��ӭ���� */
void LCD_start(void);
/* DDSɨƵ�������� */
void DDS_Scan(ulong High_Freq, ulong Low_Freq, ulong Freq_Temp);
/* Ƶ�����뺯�� */
unsigned long Freq_Input();
/* �������뺯������λΪmV */
unsigned long Pack_Input();

//********************************** ����������
unsigned int i = 0;
unsigned int j = 0;
unsigned long long_temp = 1;  //Ƶ��ת����ʱ����
unsigned long trans_temp = 0; //SPI������ʱ����
float display_temp = 0;

unsigned long adc_data = 0;
unsigned char step_sign = 1; //����Ȩֵ��0����0.1k,1����1k,2����10k,3����100k,

unsigned char mode = 0;		//ģʽѡ��
unsigned char mode_pro = 0; //��������ģʽѡ��
unsigned char enter = 0;	//ȷ��

unsigned int add_data = 0;	   //ADC�Ĳ���ֵ
unsigned long sour_freq = 0;   //�ź�ԴƵ��
unsigned int shape = 750;	   //����
unsigned int pack_test = 1000; //���ȿ���
unsigned long pack = 20000;	   //Ĭ��5mV���ֵ

unsigned long centre_freq = 0; //����Ƶ��
unsigned long bw = 0;		   //ɨƵ���
unsigned long high_freq = 0;   //ɨƵ����
unsigned long low_freq = 0;	   //ɨƵ����
unsigned long std_freq = 0;
unsigned int std_shape = 8000; //ɨƵ��׼����
unsigned long dev = 1000000;   //��������
unsigned int p_count = 0;	   //ɨƵ��ֵ����

unsigned char uart_dat[32]; //UART���߻�����
unsigned char uart_num = 0; //UART���߻�����������
unsigned char endflag = 0;	//UARTֹͣλ��־
unsigned char comm_dat[32]; //UART���ݴ�����
unsigned char comm_len = 0; //UART���ݴ���������
unsigned char uart_buf[32]; //UART���ݷ��ͻ�����
unsigned char uart_temp;

unsigned char uart_freq[4]; //UART����Ƶ��ֵ
unsigned char uart_pack[3]; //UART���շ���ֵ

unsigned char spi_dat[300];			//SPI���߻�����
unsigned char spi_num = 0;			//SPI���߻�����������
unsigned char spi_endflag = 0;		//SPIֹͣλ��־
unsigned char spi_comm_dat[5][300]; //SPI���ݴ�����
unsigned char spi_comm_len = 0;		//SPI���ݴ���������
unsigned char spi_temp = 0;

unsigned char data_sign; //SPI���ݴ����־

unsigned char spi_phase[3]; //SPI������λֵ
unsigned char spi_AD[2];	//SPI����ADֵ

unsigned char rank = 0; //������λ��0��ӦС��λ��1��Ӧ��λ

uint vpp[20];
uint vpp_freq[20];
uchar data_in[16];
uchar data_val[16];
uchar dot = 0;	//С����λ��
uchar unit = 0; //��λ��־

//****************************************** ������
void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;

	SysTime_Init(); //MSP430ϵͳʱ�ӳ�ʼ��
	init12864();	//��ʾ��Ļ��ʼ��
	key_init();		//���̳�ʼ��
	UART_Init();	//UART��ʼ��
	AD9854_Init();	//DDS��ʼ��
	// TODO debug
	//InitAD7705_ch0();	//ADC��ʼ��
	//InitAD7705_ch1();
	dac_init(); //DAC��ʼ����P6.6

	dac_start(pack_trans(pack)); //���ó�ʼ����20mV
	key_val = 0;
	display_line(0, 1, ">>�����ź�Դ<<");
	display_line(0, 2, "��ѡ��ģʽ>>");

	// ��ѭ��
	while (1)
	{
		//��������
		key_event();
		if (key_flag) //��������а����¼��Ļ���ˢ�¼�ֵ����ʾ
		{
			key_flag = 0; // ����
			switch (key_val)
			{
			case 0:
				mode = 1;
				display_line(0, 3, "����ģʽ_30M");
				AD9854_Init(); //DDS��ʼ��
				AD9854_SetSine1(30000000, shape_ctrl(30000000));
				break;
			case 1:
				mode = 1;
				display_line(0, 3, "����ģʽ_20M");
				AD9854_Init(); //DDS��ʼ��
				AD9854_SetSine1(20000000, shape_ctrl(20000000));
				break;
			case 2:
				mode = 1;
				display_line(0, 3, "����ģʽ_1M");
				AD9854_Init(); //DDS��ʼ��
				AD9854_SetSine1(1000000, shape_ctrl(1000000));
				break;
			// TODO debug
			// case 3:UART_Send_Byte(0xaa);break;
			// case 5:UART_Send_Str("page 1");break;
			// case 6:UART_Send_Str("page 2");break;
			// case 7:
			// 	mode_pro = 3;
			// 	clear_GDRAM();
			// 	clr_screen();
			// 	break;
			case 12: //�趨�ź�ԴƵ��
				mode = 2;
				clear_GDRAM();
				clr_screen();
				break;
			case 13: //�����ź�Դ����
				mode = 3;
				clear_GDRAM();
				clr_screen();
				break;
			case 14: //��ʼɨƵ����ʾ
				mode = 4;
				clear_GDRAM();
				clr_screen();
				break;
			case 15: //����������
				mode = 0;
				clear_GDRAM();
				clr_screen();
				break;
			default:
				break;
			}
		} //��Ӧ��������if

		//****************����ģʽ����********************
		switch (mode)
		{
		case 0: //������
			clear_GDRAM();
			clr_screen();
			display_line(0, 1, ">>�ź�Դ<<");
			display_line(0, 2, "��ѡ��ģʽ>>");
			mode = 32;
			break;
		case 1: //ģʽ1������Ƶģʽ
			mode = 32;
			break;
		case 2: //ģʽ2�����ź�Դģʽ
			display_line(0, 1, ">>Ƶ������");
			display_line(0, 2, "***************");
			//******************************* ����Ƶ��
			display_line(0, 3, "����Ƶ�� (MHz)");
			sour_freq = Freq_Input();
			AD9854_SetSine1(sour_freq, shape_ctrl(sour_freq));
			// AD9854_SetSine1(sour_freq,shape);	// TODO debug
			clear_GDRAM();
			clr_screen();
			display_line(0, 2, "�������!!!");
			display_line(0, 3, "��ǰƵ��Ϊ��");
			display_line(0, 4, data_val);
			//******************************* �ɲ���΢��
			enter = 0;
			while (enter == 0)
			{
				//��������
				key_event();
				if (key_flag) //��������а����¼��Ļ���ˢ�¼�ֵ����ʾ
				{
					key_flag = 0;
					switch (key_val)
					{
					case 3:
						step_sign = 1; //����Ϊ0.1M
						break;
					case 7:
						step_sign = 2; //����Ϊ1M
						break;
					case 11:
						step_sign = 3; //����Ϊ10M
						break;
					case 9: //������1
						long_temp = 1;
						for (i = 0; i < step_sign + 4; i++)
							long_temp = long_temp * 10;
						sour_freq += long_temp;
						break;
					case 13: //������1
						long_temp = 1;
						for (i = 0; i < step_sign + 4; i++)
							long_temp = long_temp * 10;
						sour_freq -= long_temp;
						if (sour_freq > 60000000)
							sour_freq = 0;
						break;
					case 8: //������1
						shape = shape + 5;
						break;
					case 12: //������1
						shape = shape - 5;
						break;
					case 14: //ȷ���������ǰƵ��
						AD9854_SetSine1(sour_freq, shape_ctrl(sour_freq));
						//AD9854_SetSine1(sour_freq,shape);
						break;
					case 15: //�˳�
						enter = 1;
						mode = 0;
						break;
					default:
						break;
					}
				}
				strcpy(data_val, Freq_MHz_to_Char(sour_freq));
				strcat(data_val, " MHz    ");
				display_line(0, 4, "               ");
				display_line(0, 4, data_val); //�ظ���ʾ��ǰƵ��ֵ
											  //display_line(0,1,"               ");
											  //display_long(0,1,shape);
			}
			break;
		case 3: //ģʽ3���������������
			display_line(0, 1, ">>��������");
			display_line(0, 2, "***************");
			//******************************* ����Ƶ��
			display_line(0, 3, "������� (mV)");
			pack = Pack_Input();
			dac_start(pack_trans(pack));
			//dac_start(pack_test);	// TODO debug

			clear_GDRAM();
			clr_screen();
			display_line(0, 2, "�������!!!");
			display_line(0, 3, "��ǰ����Ϊ��");
			display_line(0, 4, data_val);
			//******************************* �ɲ���΢��
			enter = 0;
			while (enter == 0)
			{
				//��������
				key_event();
				if (key_flag) //��������а����¼��Ļ���ˢ�¼�ֵ����ʾ
				{
					key_flag = 0;
					switch (key_val)
					{
					case 3:
						step_sign = 1; //����Ϊ0.1mV
						break;
					case 7:
						step_sign = 2; //����Ϊ1mV
						break;
					case 11:
						step_sign = 3; //����Ϊ10mV
						break;
					case 9: //������1
						long_temp = 1;
						for (i = 0; i < step_sign + 1; i++)
							long_temp = long_temp * 10;
						pack += long_temp;
						break;
					case 13: //������1
						long_temp = 1;
						for (i = 0; i < step_sign + 1; i++)
							long_temp = long_temp * 10;
						pack -= long_temp;
						if (pack > 60000000)
							pack = 0;
						break;
					case 8: //������1
						pack_test += 5;
						break;
					case 12: //������1
						pack_test -= 5;
						break;
					case 14: //ȷ���������ǰ����
						dac_start(pack_trans(pack));
						//dac_start(pack_test);
						break;
					case 15: //�˳�
						enter = 1;
						mode = 0;
						break;
					default:
						break;
					}
				}
				strcpy(data_val, Freq_kHz_to_Char(pack));
				strcat(data_val, " mV    ");
				display_line(0, 4, "               ");
				display_line(0, 4, data_val); //�ظ���ʾ��ǰƵ��ֵ
				display_line(0, 1, "               ");
				//display_long(0,1,pack_test);	// TODO debug
				display_long(0, 1, pack_trans(pack));
			}
			break;
		case 4:			   //ģʽ4����ɨƵģʽ//
			AD9854_Init(); //DDS��ʼ��
			low_freq = 1000000;
			high_freq = 40000000;
			dev = 1000000;
			DDS_Scan(low_freq, high_freq, dev); //ɨƵ����
			// TODO debug
			//display_line(0,2,"ɨƵ���");
			//mode = 32;
			break;
		case 5: //ģʽ5������������ģʽ//
			display_line(0, 1, ">>������������");
			switch (mode_pro)
			{
			case 0:
				display_line(0, 2, ">>������");
				sour_freq = 1000000; //��ʼ����Ƶ�ʵ������
				break;
			case 1: //����Ƶ��
				display_line(0, 2, ">>�����źŷ���");

				trans_temp = 0;
				for (i = 0; i < 4; i++) //��Ƶ�����ݽ��г�������
				{
					trans_temp += uart_freq[i];
					if (i < 3)
						trans_temp = trans_temp << 8;
				}
				sour_freq = trans_temp;
				AD9854_SetSine1(sour_freq, shape_ctrl(sour_freq));

				display_line(0, 3, "��ǰƵ��Ϊ��");
				strcpy(data_val, Freq_MHz_to_Char(sour_freq));
				strcat(data_val, " MHz    ");
				display_line(0, 4, "               "); //���һ��
				display_line(0, 4, data_val);		   //�ظ���ʾ��ǰƵ��ֵ

				mode_pro = 32;
				break;
			case 2: //��������
				display_line(0, 2, ">>���ȵ���");

				trans_temp = 0;
				for (i = 0; i < 3; i++) //��Ƶ�����ݽ��г�������
				{
					trans_temp += uart_pack[i];
					if (i < 2)
						trans_temp = trans_temp << 8;
				}
				pack = trans_temp;
				dac_start(pack_trans(pack));

				display_line(0, 3, "��ǰ����Ϊ��");
				strcpy(data_val, Freq_kHz_to_Char(pack));
				strcat(data_val, " mV    ");
				display_line(0, 4, "               ");
				display_line(0, 4, data_val); //�ظ���ʾ��ǰƵ��ֵ

				mode_pro = 32;
				break;
			case 3: //��ʼ�Զ�ɨƵ
				display_line(0, 2, ">>��ʼɨƵ");
				AD9854_SetSine1(sour_freq, shape_ctrl(sour_freq));
				display_line(0, 4, "               ");
				display_long(0, 4, sour_freq);
				mode_pro = 32;
				break;
			case 4: //ֹͣ�Զ�ɨƵ
				mode_pro = 32;
				break;
			default:
				break;
			}
		default:
			break;
		} // ��Ӧģʽѡ��switch

		//***************************************************************
		if (comm_len) //�˴����ڶ�UART�������ݽ��д���
		{
			MEMCOPY(comm_dat, uart_dat, comm_len);
			switch (comm_dat[0]) //������Ӧ
			{
			case 0x00: //����֡
				switch (comm_dat[1])
				{
				case 0x00: //Ƶ������
					for (i = 2; i < 6; i++)
						uart_freq[i - 2] = comm_dat[i]; //����Ƶ����Ϣ
					break;
				case 0x01: //��������
					for (i = 2; i < 5; i++)
						uart_pack[i - 2] = comm_dat[i]; //���շ�����Ϣ
					break;

				default:
					break;
				}
				break;
			case 0x01: //����֡
				switch (comm_dat[1])
				{
				case 0: //���Ӳ���ʼ������
					mode_pro = 0;
					clear_GDRAM();
					clr_screen();
					break;
				case 1: //����Ƶ��
					mode_pro = 1;
					break;
				case 2: //���÷���
					mode_pro = 2;
					break;
				case 3: //��ʼ�Զ�ɨƵ
					mode_pro = 3;
					sour_freq = (comm_dat[2] + 1) * 1000000;
					break;
				case 4: //ֹͣ�Զ�ɨƵ
					mode_pro = 4;
					break;
				case 0xaa: //��������֡
					mode = 5;
					sour_freq = 1000000;
					AD9854_SetSine1(sour_freq, shape_ctrl(sour_freq));
					mode_pro = 0;
					break;

				default:
					break;
				}
				break;

			default:
				break;
			} //����/����֡�ж�
			comm_len = 0;
		}
	} //��Ӧwhile��ѭ��
}

//***************************END*********************************
//***************************************************************
/* ϵͳʱ�ӳ�ʼ������,����ʱ�ӱ�Ƶ��8M */
void SysTime_Init()
{
	//************************** ��ʱ��Ƶ������
	volatile unsigned int i;

	UCSCTL3 = SELREF_2;
	UCSCTL4 |= SELA_2;
	UCSCTL0 = 0x0000;

	do
	{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
		SFRIFG1 &= ~OFIFG;
	} while (SFRIFG1 & OFIFG);

	__bis_SR_register(SCG0);
	UCSCTL1 = DCORSEL_5;
	UCSCTL2 |= 249;
	//************************** ��ʱ������
	//TA0CCTL0 |= CCIE;						//�Ƚ��ж�ʹ��
	//TA0CCR0 = 16384;						//���ü���ֵ,500ms
	//TA0CTL = TASSEL_1 + TACLR;       		//AMCLK, upmode, clear TAR

	__bic_SR_register(SCG0);
	__delay_cycles(250000);
}

/* �ڴ濽������ */
void MEMCOPY(unsigned char *dest, unsigned char *src, unsigned int len)
{
	while (len--)
	{
		*dest++ = *src++;
	}
}

/* DDSɨƵ�������� */
void DDS_Scan(ulong Low_Freq, ulong High_Freq, ulong Freq_Temp)
{
	ulong current_freq = Low_Freq;
	while (current_freq <= High_Freq)
	{
		display_line(0, 3, "            ");
		display_long(0, 3, current_freq);
		//������ʼɨƵƵ��
		AD9854_SetSine1(current_freq, shape_ctrl(current_freq));
		current_freq += Freq_Temp;

		_delay_ms(1000);
		//1500 ���Ե����Ȳ���
		//1000 ���Ե�����
		//700  ̫����
	}
}

/* Ƶ�����뺯������λΪMHz */
unsigned long Freq_Input()
{
	memset(data_in, 0, 16); //��ո��ֱ�־
	memset(data_val, 0, 16);
	long_temp = 1;
	enter = 0;
	dot = 0;
	unit = 0;
	ulong Freq = 0;

	for (i = 0; i < 8; i++)
		data_val[i] = '_';
	data_val[i] = '\0';

	i = 0;
	display_line(0, 4, data_val); //��ʾ��_ _ _ _��
	while (enter == 0)
	{
		//��������
		key_event();
		if (key_flag) //��������а����¼��Ļ���ˢ�¼�ֵ����ʾ
		{
			key_flag = 0;
			switch (key_val)
			{
			case 0:
				data_in[i] = '1';
				i++;
				break;
			case 1:
				data_in[i] = '2';
				i++;
				break;
			case 2:
				data_in[i] = '3';
				i++;
				break;
			case 4:
				data_in[i] = '4';
				i++;
				break;
			case 5:
				data_in[i] = '5';
				i++;
				break;
			case 6:
				data_in[i] = '6';
				i++;
				break;
			case 8:
				data_in[i] = '7';
				i++;
				break;
			case 9:
				data_in[i] = '8';
				i++;
				break;
			case 10:
				data_in[i] = '9';
				i++;
				break;
			case 13:
				data_in[i] = '0';
				i++;
				break;
			case 12:
				data_in[i] = '.';
				dot = i;
				i++;
				break; //ȷ��С����λ��

			case 14:
				unit = i;
				data_in[i] = '\0';
				enter = 1;
				i++;
				break; //ȷ��
			default:
				break;
			}
			data_val[i - 1] = data_in[i - 1];
		}
		display_line(0, 4, data_val);
	}
	strcat(data_val, " MHz");
	// TODO debug
	//display_2digit(0,1,dot);
	//display_2digit(4,1,unit);
	enter = 0;
	for (i = 0; i < unit; i++)
	{
		if (i < dot)
		{
			for (j = 0; j < (dot - i + 5); j++)
				long_temp = long_temp * 10;
			Freq += (data_in[i] - '0') * long_temp;
			long_temp = 1;
		}
		else if (i > dot)
		{
			for (j = 0; j < 6 - i + dot; j++)
				long_temp = long_temp * 10;
			Freq += (data_in[i] - '0') * long_temp;
			long_temp = 1;
		}
	}
	return Freq;
}

/* �������뺯������λΪmV */
unsigned long Pack_Input()
{
	memset(data_in, 0, 16); //��ո��ֱ�־
	memset(data_val, 0, 16);
	long_temp = 1;
	enter = 0;
	dot = 0;
	unit = 0;
	ulong Pack = 0;

	for (i = 0; i < 8; i++)
		data_val[i] = '_';
	data_val[i] = '\0';

	i = 0;
	display_line(0, 4, data_val); //��ʾ��_ _ _ _��
	while (enter == 0)
	{
		//��������
		key_event();
		if (key_flag) //��������а����¼��Ļ���ˢ�¼�ֵ����ʾ
		{
			key_flag = 0;
			switch (key_val)
			{
			case 0:
				data_in[i] = '1';
				i++;
				break;
			case 1:
				data_in[i] = '2';
				i++;
				break;
			case 2:
				data_in[i] = '3';
				i++;
				break;
			case 4:
				data_in[i] = '4';
				i++;
				break;
			case 5:
				data_in[i] = '5';
				i++;
				break;
			case 6:
				data_in[i] = '6';
				i++;
				break;
			case 8:
				data_in[i] = '7';
				i++;
				break;
			case 9:
				data_in[i] = '8';
				i++;
				break;
			case 10:
				data_in[i] = '9';
				i++;
				break;
			case 13:
				data_in[i] = '0';
				i++;
				break;
			case 12:
				data_in[i] = '.';
				dot = i;
				i++;
				break; //ȷ��С����λ��

			case 14:
				unit = i;
				data_in[i] = '\0';
				enter = 1;
				i++;
				break; //ȷ��
			default:
				break;
			}
			data_val[i - 1] = data_in[i - 1];
		}
		display_line(0, 4, data_val);
	}
	strcat(data_val, " mV");
	// TODO debug
	//display_2digit(0,1,dot);
	//display_2digit(4,1,unit);
	enter = 0;
	for (i = 0; i < unit; i++)
	{
		if (i < dot)
		{
			for (j = 0; j < (dot - i + 2); j++)
				long_temp = long_temp * 10;
			Pack += (data_in[i] - '0') * long_temp;
			long_temp = 1;
		}
		else if (i > dot)
		{
			for (j = 0; j < 3 - i + dot; j++)
				long_temp = long_temp * 10;
			Pack += (data_in[i] - '0') * long_temp;
			long_temp = 1;
		}
	}
	return Pack;
}

/**********************************************
 * �жϴ����������ڴ���UART����
 *********************************************/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR(void)
#else
#error Compiler not supported!
#endif
{
	switch (__even_in_range(UCA0IV, 4))
	{
	case 0:
		break; // Vector 0 - no interrupt
	case 2:	   // Vector 2 - RXIFG
		while (!(UCA0IFG&UCTXIFG));	// USCI_A0 TX buffer ready
		uart_dat[uart_num] = UCA0RXBUF;
		uart_num++;
		if (uart_dat[uart_num - 1] == 0xff)
		{
			endflag++;
			if (endflag == 3)
			{
				comm_len = uart_num;
				uart_num = 0;
				endflag = 0;
			}
		}
		else
		{
			endflag = 0;
		}
		break;
	case 4:
		break; // Vector 4 - TXIFG
	default:
		break;
	}
}
//******************************** ��ʱ���ж�
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
}
