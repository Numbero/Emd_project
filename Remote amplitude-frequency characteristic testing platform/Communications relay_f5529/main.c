#include <msp430.h>
#include "tools.h"

/*
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

	sys_init(); //全部初始化

	UART_Send_END();	  //初始化串口
	UART_WLAN_Send_END(); //初始化串口

	while (1)
	{
		key_test();		  //按键选择
		wlan_deal_with(); //wlan数据处理
		// TODO debug
		//adc_data = ADC_ave(6,0);
		//OLED_ShowString(0,4,"             ",16);
		//OLED_ShowLong(0,4,adc_data,16);
		//OLED_ShowString(0,6,"             ",16);
		//OLED_ShowDouble(0,6,Val_trans(adc_data),4,16);

		//****************各种模式设置********************
		switch (mode)
		{
		case 0: //模式0——复位模式
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
		case 1: //模式1——设置频率模式
			OLED_ShowString(0, 0, ">>freq_set", 16);
			OLED_ShowString(0, 2, "***************", 16);
			//******************************* 设置频率
			OLED_ShowString(0, 4, "input(MHz)", 16);
			sour_freq = Freq_Input();
			//******************************* 发送给从机
			UART_Send_Byte(0x00); //发送数据帧
			UART_Send_Byte(0x00); //发送频率值
			for (i = 4; i > 0; i--)
			{
				uart_temp = sour_freq >> ((i - 1) * 8);
				UART_Send_Byte(uart_temp);
			}
			UART_Send_END(); //发送数据结束

			_delay_ms(20);		  //命令间延时
			UART_Send_Byte(0x01); //发送命令帧
			UART_Send_Byte(0x01); //使从机进入信号源模式
			UART_Send_END();	  //发送命令结束

			//******************************* 发送给显示屏
			OLED_Clear();
			OLED_ShowString(0, 2, "complete!!!", 16);
			OLED_ShowString(0, 4, "freq:", 16);
			OLED_ShowString(0, 6, data_val, 16);
			//******************************* 可步进微调
			enter = 0;
			while (enter == 0)
			{
				//按键程序
				key_event();
				if (key_flag) //如果捕获到有按键事件的话，刷新键值，显示
				{
					key_flag = 0;
					switch (key_val)
					{
					case 3:
						step_sign = 1; //步进为0.1M
						break;
					case 7:
						step_sign = 2; //步进为1M
						break;
					case 11:
						step_sign = 3; //步进为10M
						break;
					case 9: //步进加1
						long_temp = 1;
						for (i = 0; i < step_sign + 4; i++)
							long_temp = long_temp * 10;
						sour_freq += long_temp;
						break;
					case 13: //步进减1
						long_temp = 1;
						for (i = 0; i < step_sign + 4; i++)
							long_temp = long_temp * 10;
						sour_freq -= long_temp;
						if (sour_freq > 60000000)
							sour_freq = 0;
						break;

					case 14:
						UART_Send_Byte(0x00); //发送数据帧
						UART_Send_Byte(0x00); //发送频率值
						for (i = 4; i > 0; i--)
						{
							uart_temp = sour_freq >> ((i - 1) * 8);
							UART_Send_Byte(uart_temp);
						}
						UART_Send_END(); //发送数据结束

						_delay_ms(20); //命令间延时

						UART_Send_Byte(0x01); //发送命令帧
						UART_Send_Byte(0x01); //使从机进入信号源模式
						UART_Send_END();	  //发送命令结束
						break;

					case 15: //确定并退出
						enter = 1;
						mode = 0;
					default:
						break;
					}
				}
				strcpy(data_val, Freq_MHz_to_Char(sour_freq));
				strcat(data_val, " MHz    ");
				OLED_ShowString(0, 6, data_val, 16); //重复显示当前频率值
			}
			break;
		case 2: //模式2——设置幅度模式
			OLED_ShowString(0, 0, ">>vpp_set", 16);
			OLED_ShowString(0, 2, "***************", 16);
			//******************************* 设置幅度
			OLED_ShowString(0, 4, "input(mV)", 16);
			pack = Pack_Input();

			UART_Send_Byte(0x00); //发送数据帧
			UART_Send_Byte(0x01); //发送幅度值
			for (i = 3; i > 0; i--)
			{
				uart_temp = pack >> ((i - 1) * 8);
				UART_Send_Byte(uart_temp);
			}
			UART_Send_END(); //发送数据结束

			_delay_ms(20); //命令间延时

			UART_Send_Byte(0x01); //发送命令帧
			UART_Send_Byte(0x02); //设置幅度
			UART_Send_END();	  //发送命令结束

			OLED_Clear();
			OLED_ShowString(0, 2, "complete!!!", 16);
			OLED_ShowString(0, 4, "vpp:", 16);
			OLED_ShowString(0, 6, data_val, 16);
			//******************************* 可步进微调
			enter = 0;
			while (enter == 0)
			{
				//按键程序
				key_event();
				if (key_flag) //如果捕获到有按键事件的话，刷新键值，显示
				{
					key_flag = 0;
					switch (key_val)
					{
					case 3: //步进为0.1mV
						step_sign = 1;
						break;
					case 7: //步进为1mV
						step_sign = 2;
						break;
					case 11: //步进为10mV
						step_sign = 3;
						break;
					case 9: //步进加1
						long_temp = 1;
						for (i = 0; i < step_sign + 1; i++)
							long_temp = long_temp * 10;
						pack += long_temp;
						break;
					case 13: //步进减1
						long_temp = 1;
						for (i = 0; i < step_sign + 1; i++)
							long_temp = long_temp * 10;
						pack -= long_temp;
						if (pack > 60000000)
							pack = 0;
						break;
					case 14:				  //确定并输出当前幅度
						UART_Send_Byte(0x00); //发送数据帧
						UART_Send_Byte(0x01); //发送幅度值
						for (i = 3; i > 0; i--)
						{
							uart_temp = pack >> ((i - 1) * 8);
							UART_Send_Byte(uart_temp);
						}
						UART_Send_END(); //发送数据结束

						_delay_ms(20); //命令间延时

						UART_Send_Byte(0x01); //发送命令帧
						UART_Send_Byte(0x02); //设置幅度
						UART_Send_END();	  //发送命令结束
						break;
					case 15: //退出
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
				OLED_ShowString(0, 6, data_val, 16); //重复显示当前频率值
			}
			break;
		case 3: //模式3——开始扫频
			OLED_ShowString(0, 0, ">>start_scan", 16);
			OLED_ShowString(0, 2, "***************", 16);

			if (scan_count < 40)
			{
				//_delay_ms(50);		//200ms		// TODO debug
				//对上一次扫频进行采样
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

				//更改扫频点
				OLED_ShowNum(0, 4, scan_count, 2, 16);
				UART_Send_Byte(0x01);		//发送命令帧
				UART_Send_Byte(0x03);		//使从机进入扫频模式
				UART_Send_Byte(scan_count); //发送当前扫频值
				UART_Send_END();			//发送命令结束

				scan_count++;
			}
			else
			{
				scan_count = 0;
				//更改扫频点
				UART_Send_Byte(0x01);		//发送命令帧
				UART_Send_Byte(0x03);		//使从机进入扫频模式
				UART_Send_Byte(scan_count); //发送当前扫频值
				UART_Send_END();			//发送命令结束

				for (i = 0; i < 40; i++)
				{
					val_temp[i] = (int)val_adc[i] + (Pack_Trans_3(i) / 10485.6) * (int)val_adc[19];
				}
			}
			break;

		case 4: //模式4——主机控制模式
			OLED_ShowString(0, 0, ">>link_mode  ", 16);
			OLED_ShowString(0, 2, "***************", 16);

			switch (mode_pro)
			{
			case 0:
				OLED_ShowString(0, 4, "already_link  ", 16);
				sour_freq = 1000000; //初始化将频率调到最低
				break;
			case 1: //调整频率
				OLED_ShowString(0, 4, ">>freq_set   ", 16);

				trans_temp = 0;
				for (i = 0; i < 4; i++) //对频率数据进行初步处理
				{
					trans_temp += wlan_freq[i];
					if (i < 3)
						trans_temp = trans_temp << 8;
				}

				strcpy(data_val, Freq_MHz_to_Char(trans_temp));
				strcat(data_val, " MHz    ");
				OLED_ShowString(0, 6, "               ", 16); //清空一行
				OLED_ShowString(0, 6, data_val, 16);		  //重复显示当前频率值
				//******************************* 发送给从机
				UART_Send_Byte(0x00); //发送数据帧
				UART_Send_Byte(0x00); //发送频率值
				for (i = 0; i < 3; i++)
				{
					UART_Send_Byte(wlan_freq[i]);
				}
				UART_Send_END(); //发送数据结束

				_delay_ms(20);		  //命令间延时
				UART_Send_Byte(0x01); //发送命令帧
				UART_Send_Byte(0x01); //使从机进入信号源模式
				UART_Send_END();	  //发送命令结束

				mode_pro = 32;
				break;
			case 2: //调整幅度
				OLED_ShowString(0, 4, ">>vpp_set   ", 16);

				trans_temp = 0;
				for (i = 0; i < 3; i++) //对频率数据进行初步处理
				{
					trans_temp += wlan_pack[i];
					if (i < 2)
						trans_temp = trans_temp << 8;
				}

				strcpy(data_val, Freq_kHz_to_Char(trans_temp));
				strcat(data_val, " mV    ");
				OLED_ShowString(0, 6, "               ", 16);
				OLED_ShowString(0, 6, data_val, 16); //重复显示当前幅度值
				//******************************* 发送给从机
				UART_Send_Byte(0x00); //发送数据帧
				UART_Send_Byte(0x01); //发送幅度值
				for (i = 0; i < 3; i++)
				{
					UART_Send_Byte(wlan_pack[i]);
				}
				UART_Send_END(); //发送数据结束

				_delay_ms(20);		  //命令间延时
				UART_Send_Byte(0x01); //发送命令帧
				UART_Send_Byte(0x02); //使从机进入幅度设置
				UART_Send_END();	  //发送命令结束

				mode_pro = 32;
				break;
			case 3: //开始自动扫频
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
		} //对应模式选择switch

	} //对应while大循环
	return 0;
}

/**********************************************
 * 中断处理函数，用于处理UART接受的数据
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
 * 中断处理函数，用于处理UART_WLAN接受的数据
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
 * 按键中断
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
				;			//松手检测
			P2IFG &= ~BIT1; //清除中断标志位

			send_sign++;
			if (send_sign == 2)
				send_sign = 0;
			if (send_sign)
			{
				TA0CTL = TASSEL_1 + TACLR + MC_1; //定时器开始计数，开始向主机定时发送数据
			}
			else
			{
				TA0CTL = TASSEL_1 + TACLR + MC_0; //定时器停止，数据发送结束
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
				;			//松手检测
			P1IFG &= ~BIT1; //清除中断标志位

			UART_Send_Byte(0x01); //命令帧
			UART_Send_Byte(0xaa); //连接命令，让从机进入联机模式
			UART_Send_END();	  //结束符

			scan_count = 0;
		}
	}
}

//******************************** 定时器中断
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
