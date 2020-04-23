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
		key_test();		   //按键选择
		screen_response(); //触摸屏响应
		wlan_deal_with();  //从机通信响应
		//****************各种模式设置********************
		switch (mode)
		{
		case 0: //复位模式
			OLED_ShowString(24, 0, ">>Tester<<", 16);
			OLED_ShowString(0, 2, "***************", 16);
			break;
		case 1: //设置频率模式
			OLED_ShowString(0, 0, ">>freq_set", 16);
			OLED_ShowString(0, 2, "***************", 16);
			//******************************* 设置频率
			OLED_ShowString(0, 4, "input(MHz)", 16);
			sour_freq = Freq_Input();
			//******************************* 发送给从机
			UART_WLAN_Send_Byte(0x00); //发送数据帧
			UART_WLAN_Send_Byte(0x00); //发送频率值
			for (i = 4; i > 0; i--)
			{
				uart_temp = sour_freq >> ((i - 1) * 8);
				UART_WLAN_Send_Byte(uart_temp);
			}
			UART_WLAN_Send_END(); //发送数据结束

			_delay_ms(80);			   //命令间延时
			UART_WLAN_Send_Byte(0x01); //发送命令帧
			UART_WLAN_Send_Byte(0x01); //使从机进入信号源模式
			UART_WLAN_Send_END();	   //发送命令结束
			//******************************* 发送给显示屏
			strcpy(uart_buf, "t2.txt=\"");
			strcat(uart_buf, Freq_MHz_to_Char(sour_freq));
			strcat(uart_buf, " MHz");
			strcat(uart_buf, "\"");
			UART_Send_Str(uart_buf);
			UART_Send_END();

			OLED_Clear();
			OLED_ShowString(0, 2, "complete!!!", 16);
			OLED_ShowString(0, 4, "freq:", 16);
			OLED_ShowString(0, 6, data_val, 16);
			//****************************** 可步进微调
			enter = 0;
			while (enter == 0)
			{
				screen_response(); //触摸屏响应
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
					case 10: //步进减1
						long_temp = 1;
						for (i = 0; i < step_sign + 4; i++)
							long_temp = long_temp * 10;
						sour_freq -= long_temp;
						if (sour_freq > 60000000)
							sour_freq = 0;
						break;

					case 14:
						UART_WLAN_Send_Byte(0x00); //发送数据帧
						UART_WLAN_Send_Byte(0x00); //发送频率值
						for (i = 4; i > 0; i--)
						{
							uart_temp = sour_freq >> ((i - 1) * 8);
							UART_WLAN_Send_Byte(uart_temp);
						}
						UART_WLAN_Send_END(); //发送数据结束

						_delay_ms(80); //命令间延时

						UART_WLAN_Send_Byte(0x01); //发送命令帧
						UART_WLAN_Send_Byte(0x01); //使从机进入信号源模式
						UART_WLAN_Send_END();	   //发送命令结束
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
				OLED_ShowString(0, 6, "             ", 16);
				OLED_ShowString(0, 6, data_val, 16); //重复显示当前频率值

				//************************************** 发送给触摸屏
				strcpy(uart_buf, "t2.txt=\"");
				strcat(uart_buf, Freq_MHz_to_Char(sour_freq));
				strcat(uart_buf, " MHz");
				strcat(uart_buf, "\"");
				UART_Send_Str(uart_buf);
				UART_Send_END();
			}
			break;
		case 2: //设置幅度模式
			OLED_ShowString(0, 0, ">>vpp_set", 16);
			OLED_ShowString(0, 2, "***************", 16);
			//******************************* 设置幅度
			OLED_ShowString(0, 4, "input(mV)", 16);
			pack = Pack_Input();

			UART_WLAN_Send_Byte(0x00); //发送数据帧
			UART_WLAN_Send_Byte(0x01); //发送幅度值
			for (i = 3; i > 0; i--)
			{
				uart_temp = pack >> ((i - 1) * 8);
				UART_WLAN_Send_Byte(uart_temp);
			}
			UART_WLAN_Send_END(); //发送数据结束

			_delay_ms(80); //命令间延时

			UART_WLAN_Send_Byte(0x01); //发送命令帧
			UART_WLAN_Send_Byte(0x02); //设置幅度
			UART_WLAN_Send_END();	   //发送命令结束
			//****************************** 发送给显示屏
			strcpy(uart_buf, "t4.txt=\"");
			strcat(uart_buf, Freq_kHz_to_Char(pack));
			strcat(uart_buf, " mV");
			strcat(uart_buf, "\"");
			UART_Send_Str(uart_buf);
			UART_Send_END();

			OLED_Clear();
			OLED_ShowString(0, 2, "complete!!!", 16);
			OLED_ShowString(0, 4, "vpp:", 16);
			OLED_ShowString(0, 6, data_val, 16);
			//******************************* 可步进微调
			enter = 0;
			while (enter == 0)
			{
				screen_response(); //触摸屏响应
				//按键程序
				key_event();
				if (key_flag) //如果捕获到有按键事件的话，刷新键值，显示
				{
					key_flag = 0;
					switch (key_val)
					{
					case 3:
						step_sign = 1; //步进为0.1mV
						break;
					case 7:
						step_sign = 2; //步进为1mV
						break;
					case 11:
						step_sign = 3; //步进为10mV
						break;
					case 9: //步进加1
						long_temp = 1;
						for (i = 0; i < step_sign + 1; i++)
							long_temp = long_temp * 10;
						pack += long_temp;
						break;
					case 10: //步进减1
						long_temp = 1;
						for (i = 0; i < step_sign + 1; i++)
							long_temp = long_temp * 10;
						pack -= long_temp;
						if (pack > 60000000)
							pack = 0;
						break;

					case 14:					   //确定并输出当前幅度
						UART_WLAN_Send_Byte(0x00); //发送数据帧
						UART_WLAN_Send_Byte(0x01); //发送幅度值
						for (i = 3; i > 0; i--)
						{
							uart_temp = pack >> ((i - 1) * 8);
							UART_WLAN_Send_Byte(uart_temp);
						}
						UART_WLAN_Send_END(); //发送数据结束

						_delay_ms(80); //命令间延时

						UART_WLAN_Send_Byte(0x01); //发送命令帧
						UART_WLAN_Send_Byte(0x02); //设置幅度
						UART_WLAN_Send_END();	   //发送命令结束
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

				//******************************* 发送给显示屏
				strcpy(uart_buf, "t4.txt=\"");
				strcat(uart_buf, Freq_kHz_to_Char(pack));
				strcat(uart_buf, " mV");
				strcat(uart_buf, "\"");
				UART_Send_Str(uart_buf);
				UART_Send_END();
			}
			break;
		case 3: //开始扫频,显示在触摸屏上
			OLED_ShowString(0, 0, ">>start_scan_2", 16);
			OLED_ShowString(0, 2, "***************", 16);

			// 发送给触摸屏显示	// TODO debug
			// for(i=0;i<40;i++)
			// {
			// 	val_data[i] = val_temp[i]/256;
			// 	sprintf(uart_buf,"add 1,0,%d",val_data[i]);
			// 	UART_Send_Str(uart_buf);
			// 	UART_Send_END();
			// 	UART_Send_Str(uart_buf);
			// 	UART_Send_END();
			// 	UART_Send_Str(uart_buf);
			// 	UART_Send_END();
			// 	UART_Send_Str(uart_buf);
			// 	UART_Send_END();
			// }

			//发送给触摸屏显示
			for (i = 0; i < 39; i++)
			{
				draw_dat = (double)(val_temp[i + 1] - val_temp[i]) / 8.0; //计算出画图步进

				for (j = 0; j < 8; j++)
				{
					val_data[i] = (val_temp[i] + j * draw_dat) / 131.07;
					sprintf(uart_buf, "add 1,0,%d", val_data[i]);
					UART_Send_Str(uart_buf);
					UART_Send_END();
				}

				// TODO debug
				//DAC_12_translate_a(4095.0/40*(i+1));		//X轴，代表频率
				//DAC_12_translate_b(val_temp[i]/16);		//Y轴，代表幅度
			}

			val_data[39] = val_temp[39] / 256;
			sprintf(uart_buf, "add 1,0,%d", val_data[39]);
			UART_Send_Str(uart_buf);
			UART_Send_END();

			// TODO debug
			//DAC_12_translate_a(4095);				//X轴，代表频率
			//DAC_12_translate_b(val_temp[39]/16);	//Y轴，代表幅度

			//清屏
			UART_Send_Str("cle 1,0");
			UART_Send_END();
			break;

		case 4: //开始扫频,显示在示波器上
			OLED_ShowString(0, 0, ">>start_scan_1", 16);
			OLED_ShowString(0, 2, "***************", 16);

			for (i = 0; i < 39; i++)
			{
				DAC_12_translate_a(4095.0 / 40 * (i + 1)); //X轴，代表频率
				DAC_12_translate_b(val_temp[i] / 16);	   //Y轴，代表幅度
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

			enter = 1;
			mode = 0;
			OLED_Clear();
			scan_count = 0;

			UART_Send_Byte(0x01); //发送命令帧
			UART_Send_Byte(0x00); //初始化从机
			UART_Send_END();	  //发送命令结束
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
		}
	}
}

//********************************定时器中断
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
}
