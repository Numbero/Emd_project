#include <msp430.h>
#include "tools.h"

/*
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

	sys_init(); //ȫ����ʼ��

	UART_Send_END();	  //��ʼ������
	UART_WLAN_Send_END(); //��ʼ������

	while (1)
	{
		key_test();		  //����ѡ��
		wlan_deal_with(); //wlan���ݴ���
		// TODO debug
		//adc_data = ADC_ave(6,0);
		//OLED_ShowString(0,4,"             ",16);
		//OLED_ShowLong(0,4,adc_data,16);
		//OLED_ShowString(0,6,"             ",16);
		//OLED_ShowDouble(0,6,Val_trans(adc_data),4,16);

		//****************����ģʽ����********************
		switch (mode)
		{
		case 0: //ģʽ0������λģʽ
			OLED_ShowString(16, 0, ">>Repeater<<", 16);
			OLED_ShowString(0, 2, "***************", 16);
			scan_count = 0;
			// TODO debug
			//adc_data = ADC_ave_pro(30,0);
			//OLED_ShowString(0,4,"             ",16);
			//OLED_ShowLong(0,4,adc_data,16);
			//OLED_ShowLong(0,4,Pack_Trans_1(adc_data),16);
			//OLED_ShowString(0,6,"             ",16);
			//OLED_ShowDouble(0,6,Pack_Trans(adc_data),3,16);

			break;
		case 1: //ģʽ1��������Ƶ��ģʽ
			OLED_ShowString(0, 0, ">>freq_set", 16);
			OLED_ShowString(0, 2, "***************", 16);
			//******************************* ����Ƶ��
			OLED_ShowString(0, 4, "input(MHz)", 16);
			sour_freq = Freq_Input();
			//******************************* ���͸��ӻ�
			UART_Send_Byte(0x00); //��������֡
			UART_Send_Byte(0x00); //����Ƶ��ֵ
			for (i = 4; i > 0; i--)
			{
				uart_temp = sour_freq >> ((i - 1) * 8);
				UART_Send_Byte(uart_temp);
			}
			UART_Send_END(); //�������ݽ���

			_delay_ms(20);		  //�������ʱ
			UART_Send_Byte(0x01); //��������֡
			UART_Send_Byte(0x01); //ʹ�ӻ������ź�Դģʽ
			UART_Send_END();	  //�����������

			//******************************* ���͸���ʾ��
			OLED_Clear();
			OLED_ShowString(0, 2, "complete!!!", 16);
			OLED_ShowString(0, 4, "freq:", 16);
			OLED_ShowString(0, 6, data_val, 16);
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

					case 14:
						UART_Send_Byte(0x00); //��������֡
						UART_Send_Byte(0x00); //����Ƶ��ֵ
						for (i = 4; i > 0; i--)
						{
							uart_temp = sour_freq >> ((i - 1) * 8);
							UART_Send_Byte(uart_temp);
						}
						UART_Send_END(); //�������ݽ���

						_delay_ms(20); //�������ʱ

						UART_Send_Byte(0x01); //��������֡
						UART_Send_Byte(0x01); //ʹ�ӻ������ź�Դģʽ
						UART_Send_END();	  //�����������
						break;

					case 15: //ȷ�����˳�
						enter = 1;
						mode = 0;
					default:
						break;
					}
				}
				strcpy(data_val, Freq_MHz_to_Char(sour_freq));
				strcat(data_val, " MHz    ");
				OLED_ShowString(0, 6, data_val, 16); //�ظ���ʾ��ǰƵ��ֵ
			}
			break;
		case 2: //ģʽ2�������÷���ģʽ
			OLED_ShowString(0, 0, ">>vpp_set", 16);
			OLED_ShowString(0, 2, "***************", 16);
			//******************************* ���÷���
			OLED_ShowString(0, 4, "input(mV)", 16);
			pack = Pack_Input();

			UART_Send_Byte(0x00); //��������֡
			UART_Send_Byte(0x01); //���ͷ���ֵ
			for (i = 3; i > 0; i--)
			{
				uart_temp = pack >> ((i - 1) * 8);
				UART_Send_Byte(uart_temp);
			}
			UART_Send_END(); //�������ݽ���

			_delay_ms(20); //�������ʱ

			UART_Send_Byte(0x01); //��������֡
			UART_Send_Byte(0x02); //���÷���
			UART_Send_END();	  //�����������

			OLED_Clear();
			OLED_ShowString(0, 2, "complete!!!", 16);
			OLED_ShowString(0, 4, "vpp:", 16);
			OLED_ShowString(0, 6, data_val, 16);
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
					case 3: //����Ϊ0.1mV
						step_sign = 1;
						break;
					case 7: //����Ϊ1mV
						step_sign = 2;
						break;
					case 11: //����Ϊ10mV
						step_sign = 3;
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
					case 14:				  //ȷ���������ǰ����
						UART_Send_Byte(0x00); //��������֡
						UART_Send_Byte(0x01); //���ͷ���ֵ
						for (i = 3; i > 0; i--)
						{
							uart_temp = pack >> ((i - 1) * 8);
							UART_Send_Byte(uart_temp);
						}
						UART_Send_END(); //�������ݽ���

						_delay_ms(20); //�������ʱ

						UART_Send_Byte(0x01); //��������֡
						UART_Send_Byte(0x02); //���÷���
						UART_Send_END();	  //�����������
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
				OLED_ShowString(0, 6, "             ", 16);
				OLED_ShowString(0, 6, data_val, 16); //�ظ���ʾ��ǰƵ��ֵ
			}
			break;
		case 3: //ģʽ3������ʼɨƵ
			OLED_ShowString(0, 0, ">>start_scan", 16);
			OLED_ShowString(0, 2, "***************", 16);

			if (scan_count < 40)
			{
				//_delay_ms(50);		//200ms		// TODO debug
				//����һ��ɨƵ���в���
				adc_data = ADC_ave_pro(20, 0);
				val_adc[scan_count] = Pack_Trans_1(adc_data);
				//val_adc[scan_count] = adc_data;
				//val_adc[scan_count] = 0xbbaa;

				OLED_ShowString(0, 4, "             ", 16);
				//OLED_ShowLong(0,4,adc_data,16);				 // TODO debug
				//OLED_ShowString(0,6,"             ",16);		 // TODO debug
				//OLED_ShowDouble(0,6,Pack_Trans(adc_data),3,16);// TODO debug
				OLED_ShowString(0, 6, "             ", 16);
				OLED_ShowLong(0, 6, val_adc[scan_count], 16);

				//����ɨƵ��
				OLED_ShowNum(0, 4, scan_count, 2, 16);
				UART_Send_Byte(0x01);		//��������֡
				UART_Send_Byte(0x03);		//ʹ�ӻ�����ɨƵģʽ
				UART_Send_Byte(scan_count); //���͵�ǰɨƵֵ
				UART_Send_END();			//�����������

				scan_count++;
			}
			else
			{
				scan_count = 0;
				//����ɨƵ��
				UART_Send_Byte(0x01);		//��������֡
				UART_Send_Byte(0x03);		//ʹ�ӻ�����ɨƵģʽ
				UART_Send_Byte(scan_count); //���͵�ǰɨƵֵ
				UART_Send_END();			//�����������

				for (i = 0; i < 40; i++)
				{
					val_temp[i] = (int)val_adc[i] + (Pack_Trans_3(i) / 10485.6) * (int)val_adc[19];
				}
			}
			break;

		case 4: //ģʽ4������������ģʽ
			OLED_ShowString(0, 0, ">>link_mode  ", 16);
			OLED_ShowString(0, 2, "***************", 16);

			switch (mode_pro)
			{
			case 0:
				OLED_ShowString(0, 4, "already_link  ", 16);
				sour_freq = 1000000; //��ʼ����Ƶ�ʵ������
				break;
			case 1: //����Ƶ��
				OLED_ShowString(0, 4, ">>freq_set   ", 16);

				trans_temp = 0;
				for (i = 0; i < 4; i++) //��Ƶ�����ݽ��г�������
				{
					trans_temp += wlan_freq[i];
					if (i < 3)
						trans_temp = trans_temp << 8;
				}

				strcpy(data_val, Freq_MHz_to_Char(trans_temp));
				strcat(data_val, " MHz    ");
				OLED_ShowString(0, 6, "               ", 16); //���һ��
				OLED_ShowString(0, 6, data_val, 16);		  //�ظ���ʾ��ǰƵ��ֵ
				//******************************* ���͸��ӻ�
				UART_Send_Byte(0x00); //��������֡
				UART_Send_Byte(0x00); //����Ƶ��ֵ
				for (i = 0; i < 3; i++)
				{
					UART_Send_Byte(wlan_freq[i]);
				}
				UART_Send_END(); //�������ݽ���

				_delay_ms(20);		  //�������ʱ
				UART_Send_Byte(0x01); //��������֡
				UART_Send_Byte(0x01); //ʹ�ӻ������ź�Դģʽ
				UART_Send_END();	  //�����������

				mode_pro = 32;
				break;
			case 2: //��������
				OLED_ShowString(0, 4, ">>vpp_set   ", 16);

				trans_temp = 0;
				for (i = 0; i < 3; i++) //��Ƶ�����ݽ��г�������
				{
					trans_temp += wlan_pack[i];
					if (i < 2)
						trans_temp = trans_temp << 8;
				}

				strcpy(data_val, Freq_kHz_to_Char(trans_temp));
				strcat(data_val, " mV    ");
				OLED_ShowString(0, 6, "               ", 16);
				OLED_ShowString(0, 6, data_val, 16); //�ظ���ʾ��ǰ����ֵ
				//******************************* ���͸��ӻ�
				UART_Send_Byte(0x00); //��������֡
				UART_Send_Byte(0x01); //���ͷ���ֵ
				for (i = 0; i < 3; i++)
				{
					UART_Send_Byte(wlan_pack[i]);
				}
				UART_Send_END(); //�������ݽ���

				_delay_ms(20);		  //�������ʱ
				UART_Send_Byte(0x01); //��������֡
				UART_Send_Byte(0x02); //ʹ�ӻ������������
				UART_Send_END();	  //�����������

				mode_pro = 32;
				break;
			case 3: //��ʼ�Զ�ɨƵ
				mode = 3;
				scan_count = 0;
				OLED_Clear();
				break;

			default:
				break;
			}
			break;

		default:
			break;
		} //��Ӧģʽѡ��switch

	} //��Ӧwhile��ѭ��
	return 0;
}

/**********************************************
 * �жϴ����������ڴ���UART���ܵ�����
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
		while (!(UCA0IFG & UCTXIFG)); // USCI_A0 TX buffer ready
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
/**********************************************
 * �жϴ����������ڴ���UART_WLAN���ܵ�����
 *********************************************/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR(void)
#else
#error Compiler not supported!
#endif
{
	switch (__even_in_range(UCA1IV, 4))
	{
	case 0:
		break; // Vector 0 - no interrupt
	case 2:	   // Vector 2 - RXIFG
		wlan_dat[wlan_num] = UCA1RXBUF;
		wlan_num++;
		if (wlan_dat[wlan_num - 1] == 0xff)
		{
			wlan_endflag++;
			if (wlan_endflag == 3)
			{
				wlan_comm_len = wlan_num;
				wlan_num = 0;
				wlan_endflag = 0;
			}
		}
		else
		{
			wlan_endflag = 0;
		}
		break;
	case 4:
		break; // Vector 4 - TXIFG
	default:
		break;
	}
}

/**********************************************
 * �����ж�
 *********************************************/
#pragma vector = PORT2_VECTOR
__interrupt void Key2_interrput()
{
	if (P2IFG & BIT1)
	{
		_delay_us(500);
		if (P2IFG & BIT1)
		{
			while ((P2IN & BIT1) == 0)
				;			//���ּ��
			P2IFG &= ~BIT1; //����жϱ�־λ

			send_sign++;
			if (send_sign == 2)
				send_sign = 0;
			if (send_sign)
			{
				TA0CTL = TASSEL_1 + TACLR + MC_1; //��ʱ����ʼ��������ʼ��������ʱ��������
			}
			else
			{
				TA0CTL = TASSEL_1 + TACLR + MC_0; //��ʱ��ֹͣ�����ݷ��ͽ���
			}
		}
	}
}

#pragma vector = PORT1_VECTOR
__interrupt void Key1_interrput()
{
	if (P1IFG & BIT1)
	{
		_delay_us(500);
		if (P1IFG & BIT1)
		{
			while ((P1IN & BIT1) == 0)
				;			//���ּ��
			P1IFG &= ~BIT1; //����жϱ�־λ

			UART_Send_Byte(0x01); //����֡
			UART_Send_Byte(0xaa); //��������ôӻ���������ģʽ
			UART_Send_END();	  //������

			scan_count = 0;
		}
	}
}

//******************************** ��ʱ���ж�
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
	P1OUT ^= 0x01;

	UART_WLAN_Send_Byte(0x03);
	for (i = 0; i < 40; i++)
	{
		wlan_temp = val_temp[i] >> 8;
		UART_WLAN_Send_Byte(wlan_temp);
		wlan_temp = val_temp[i];
		UART_WLAN_Send_Byte(wlan_temp);
	}
	UART_WLAN_Send_END();
}
