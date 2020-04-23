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

/* MSP430系统时钟8M初始化 */
void SysTime_Init();
/* 内存拷贝函数 */
void MEMCOPY(unsigned char *dest, unsigned char *src, unsigned int len);
/* 欢迎界面 */
void LCD_start(void);
/* DDS扫频及处理函数 */
void DDS_Scan(ulong High_Freq, ulong Low_Freq, ulong Freq_Temp);
/* 频率输入函数 */
unsigned long Freq_Input();
/* 幅度输入函数，单位为mV */
unsigned long Pack_Input();

//********************************** 变量声明区
unsigned int i = 0;
unsigned int j = 0;
unsigned long long_temp = 1;  //频率转换临时变量
unsigned long trans_temp = 0; //SPI传输临时变量
float display_temp = 0;

unsigned long adc_data = 0;
unsigned char step_sign = 1; //步进权值：0——0.1k,1——1k,2——10k,3——100k,

unsigned char mode = 0;		//模式选择
unsigned char mode_pro = 0; //主从连接模式选择
unsigned char enter = 0;	//确定

unsigned int add_data = 0;	   //ADC的测量值
unsigned long sour_freq = 0;   //信号源频率
unsigned int shape = 750;	   //幅度
unsigned int pack_test = 1000; //幅度控制
unsigned long pack = 20000;	   //默认5mV峰峰值

unsigned long centre_freq = 0; //中心频率
unsigned long bw = 0;		   //扫频宽度
unsigned long high_freq = 0;   //扫频上限
unsigned long low_freq = 0;	   //扫频下限
unsigned long std_freq = 0;
unsigned int std_shape = 8000; //扫频标准幅度
unsigned long dev = 1000000;   //步进调整
unsigned int p_count = 0;	   //扫频峰值个数

unsigned char uart_dat[32]; //UART总线缓冲区
unsigned char uart_num = 0; //UART总线缓冲区计数器
unsigned char endflag = 0;	//UART停止位标志
unsigned char comm_dat[32]; //UART数据处理区
unsigned char comm_len = 0; //UART数据处理区长度
unsigned char uart_buf[32]; //UART数据发送缓存区
unsigned char uart_temp;

unsigned char uart_freq[4]; //UART接收频率值
unsigned char uart_pack[3]; //UART接收幅度值

unsigned char spi_dat[300];			//SPI总线缓冲区
unsigned char spi_num = 0;			//SPI总线缓冲区计数器
unsigned char spi_endflag = 0;		//SPI停止位标志
unsigned char spi_comm_dat[5][300]; //SPI数据处理区
unsigned char spi_comm_len = 0;		//SPI数据处理区长度
unsigned char spi_temp = 0;

unsigned char data_sign; //SPI数据传输标志

unsigned char spi_phase[3]; //SPI接收相位值
unsigned char spi_AD[2];	//SPI接收AD值

unsigned char rank = 0; //测量档位，0对应小档位，1对应大档位

uint vpp[20];
uint vpp_freq[20];
uchar data_in[16];
uchar data_val[16];
uchar dot = 0;	//小数点位置
uchar unit = 0; //单位标志

//****************************************** 主函数
void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;

	SysTime_Init(); //MSP430系统时钟初始化
	init12864();	//显示屏幕初始化
	key_init();		//键盘初始化
	UART_Init();	//UART初始化
	AD9854_Init();	//DDS初始化
	// TODO debug
	//InitAD7705_ch0();	//ADC初始化
	//InitAD7705_ch1();
	dac_init(); //DAC初始化，P6.6

	dac_start(pack_trans(pack)); //设置初始幅度20mV
	key_val = 0;
	display_line(0, 1, ">>正弦信号源<<");
	display_line(0, 2, "请选择模式>>");

	// 主循环
	while (1)
	{
		//按键程序
		key_event();
		if (key_flag) //如果捕获到有按键事件的话，刷新键值，显示
		{
			key_flag = 0; // 防抖
			switch (key_val)
			{
			case 0:
				mode = 1;
				display_line(0, 3, "测试模式_30M");
				AD9854_Init(); //DDS初始化
				AD9854_SetSine1(30000000, shape_ctrl(30000000));
				break;
			case 1:
				mode = 1;
				display_line(0, 3, "测试模式_20M");
				AD9854_Init(); //DDS初始化
				AD9854_SetSine1(20000000, shape_ctrl(20000000));
				break;
			case 2:
				mode = 1;
				display_line(0, 3, "测试模式_1M");
				AD9854_Init(); //DDS初始化
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
			case 12: //设定信号源频率
				mode = 2;
				clear_GDRAM();
				clr_screen();
				break;
			case 13: //设置信号源幅度
				mode = 3;
				clear_GDRAM();
				clr_screen();
				break;
			case 14: //开始扫频并显示
				mode = 4;
				clear_GDRAM();
				clr_screen();
				break;
			case 15: //返回主界面
				mode = 0;
				clear_GDRAM();
				clr_screen();
				break;
			default:
				break;
			}
		} //对应按键程序if

		//****************各种模式设置********************
		switch (mode)
		{
		case 0: //主界面
			clear_GDRAM();
			clr_screen();
			display_line(0, 1, ">>信号源<<");
			display_line(0, 2, "请选择模式>>");
			mode = 32;
			break;
		case 1: //模式1——定频模式
			mode = 32;
			break;
		case 2: //模式2——信号源模式
			display_line(0, 1, ">>频率设置");
			display_line(0, 2, "***************");
			//******************************* 设置频率
			display_line(0, 3, "输入频率 (MHz)");
			sour_freq = Freq_Input();
			AD9854_SetSine1(sour_freq, shape_ctrl(sour_freq));
			// AD9854_SetSine1(sour_freq,shape);	// TODO debug
			clear_GDRAM();
			clr_screen();
			display_line(0, 2, "设置完成!!!");
			display_line(0, 3, "当前频率为：");
			display_line(0, 4, data_val);
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
					case 8: //步进加1
						shape = shape + 5;
						break;
					case 12: //步进减1
						shape = shape - 5;
						break;
					case 14: //确定并输出当前频率
						AD9854_SetSine1(sour_freq, shape_ctrl(sour_freq));
						//AD9854_SetSine1(sour_freq,shape);
						break;
					case 15: //退出
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
				display_line(0, 4, data_val); //重复显示当前频率值
											  //display_line(0,1,"               ");
											  //display_long(0,1,shape);
			}
			break;
		case 3: //模式3——设置输出幅度
			display_line(0, 1, ">>幅度设置");
			display_line(0, 2, "***************");
			//******************************* 设置频率
			display_line(0, 3, "输入幅度 (mV)");
			pack = Pack_Input();
			dac_start(pack_trans(pack));
			//dac_start(pack_test);	// TODO debug

			clear_GDRAM();
			clr_screen();
			display_line(0, 2, "设置完成!!!");
			display_line(0, 3, "当前幅度为：");
			display_line(0, 4, data_val);
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
					case 13: //步进减1
						long_temp = 1;
						for (i = 0; i < step_sign + 1; i++)
							long_temp = long_temp * 10;
						pack -= long_temp;
						if (pack > 60000000)
							pack = 0;
						break;
					case 8: //步进加1
						pack_test += 5;
						break;
					case 12: //步进减1
						pack_test -= 5;
						break;
					case 14: //确定并输出当前幅度
						dac_start(pack_trans(pack));
						//dac_start(pack_test);
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
				display_line(0, 4, "               ");
				display_line(0, 4, data_val); //重复显示当前频率值
				display_line(0, 1, "               ");
				//display_long(0,1,pack_test);	// TODO debug
				display_long(0, 1, pack_trans(pack));
			}
			break;
		case 4:			   //模式4——扫频模式//
			AD9854_Init(); //DDS初始化
			low_freq = 1000000;
			high_freq = 40000000;
			dev = 1000000;
			DDS_Scan(low_freq, high_freq, dev); //扫频设置
			// TODO debug
			//display_line(0,2,"扫频完成");
			//mode = 32;
			break;
		case 5: //模式5——主机连接模式//
			display_line(0, 1, ">>已与主机连接");
			switch (mode_pro)
			{
			case 0:
				display_line(0, 2, ">>已连接");
				sour_freq = 1000000; //初始化将频率调到最低
				break;
			case 1: //调整频率
				display_line(0, 2, ">>正弦信号发生");

				trans_temp = 0;
				for (i = 0; i < 4; i++) //对频率数据进行初步处理
				{
					trans_temp += uart_freq[i];
					if (i < 3)
						trans_temp = trans_temp << 8;
				}
				sour_freq = trans_temp;
				AD9854_SetSine1(sour_freq, shape_ctrl(sour_freq));

				display_line(0, 3, "当前频率为：");
				strcpy(data_val, Freq_MHz_to_Char(sour_freq));
				strcat(data_val, " MHz    ");
				display_line(0, 4, "               "); //清空一行
				display_line(0, 4, data_val);		   //重复显示当前频率值

				mode_pro = 32;
				break;
			case 2: //调整幅度
				display_line(0, 2, ">>幅度调整");

				trans_temp = 0;
				for (i = 0; i < 3; i++) //对频率数据进行初步处理
				{
					trans_temp += uart_pack[i];
					if (i < 2)
						trans_temp = trans_temp << 8;
				}
				pack = trans_temp;
				dac_start(pack_trans(pack));

				display_line(0, 3, "当前幅度为：");
				strcpy(data_val, Freq_kHz_to_Char(pack));
				strcat(data_val, " mV    ");
				display_line(0, 4, "               ");
				display_line(0, 4, data_val); //重复显示当前频率值

				mode_pro = 32;
				break;
			case 3: //开始自动扫频
				display_line(0, 2, ">>开始扫频");
				AD9854_SetSine1(sour_freq, shape_ctrl(sour_freq));
				display_line(0, 4, "               ");
				display_long(0, 4, sour_freq);
				mode_pro = 32;
				break;
			case 4: //停止自动扫频
				mode_pro = 32;
				break;
			default:
				break;
			}
		default:
			break;
		} // 对应模式选择switch

		//***************************************************************
		if (comm_len) //此处用于对UART接收数据进行处理
		{
			MEMCOPY(comm_dat, uart_dat, comm_len);
			switch (comm_dat[0]) //命令响应
			{
			case 0x00: //数据帧
				switch (comm_dat[1])
				{
				case 0x00: //频率数据
					for (i = 2; i < 6; i++)
						uart_freq[i - 2] = comm_dat[i]; //接收频率信息
					break;
				case 0x01: //幅度数据
					for (i = 2; i < 5; i++)
						uart_pack[i - 2] = comm_dat[i]; //接收幅度信息
					break;

				default:
					break;
				}
				break;
			case 0x01: //命令帧
				switch (comm_dat[1])
				{
				case 0: //连接并初始化命令
					mode_pro = 0;
					clear_GDRAM();
					clr_screen();
					break;
				case 1: //设置频率
					mode_pro = 1;
					break;
				case 2: //设置幅度
					mode_pro = 2;
					break;
				case 3: //开始自动扫频
					mode_pro = 3;
					sour_freq = (comm_dat[2] + 1) * 1000000;
					break;
				case 4: //停止自动扫频
					mode_pro = 4;
					break;
				case 0xaa: //连接命令帧
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
			} //数据/命令帧判断
			comm_len = 0;
		}
	} //对应while大循环
}

//***************************END*********************************
//***************************************************************
/* 系统时钟初始化函数,将主时钟倍频到8M */
void SysTime_Init()
{
	//************************** 主时钟频率设置
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
	//************************** 定时器设置
	//TA0CCTL0 |= CCIE;						//比较中断使能
	//TA0CCR0 = 16384;						//设置计数值,500ms
	//TA0CTL = TASSEL_1 + TACLR;       		//AMCLK, upmode, clear TAR

	__bic_SR_register(SCG0);
	__delay_cycles(250000);
}

/* 内存拷贝函数 */
void MEMCOPY(unsigned char *dest, unsigned char *src, unsigned int len)
{
	while (len--)
	{
		*dest++ = *src++;
	}
}

/* DDS扫频及处理函数 */
void DDS_Scan(ulong Low_Freq, ulong High_Freq, ulong Freq_Temp)
{
	ulong current_freq = Low_Freq;
	while (current_freq <= High_Freq)
	{
		display_line(0, 3, "            ");
		display_long(0, 3, current_freq);
		//设置起始扫频频率
		AD9854_SetSine1(current_freq, shape_ctrl(current_freq));
		current_freq += Freq_Temp;

		_delay_ms(1000);
		//1500 可以但精度不够
		//1000 可以但很慢
		//700  太慢了
	}
}

/* 频率输入函数，单位为MHz */
unsigned long Freq_Input()
{
	memset(data_in, 0, 16); //清空各种标志
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
	display_line(0, 4, data_val); //显示“_ _ _ _”
	while (enter == 0)
	{
		//按键程序
		key_event();
		if (key_flag) //如果捕获到有按键事件的话，刷新键值，显示
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
				break; //确定小数点位置

			case 14:
				unit = i;
				data_in[i] = '\0';
				enter = 1;
				i++;
				break; //确定
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

/* 幅度输入函数，单位为mV */
unsigned long Pack_Input()
{
	memset(data_in, 0, 16); //清空各种标志
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
	display_line(0, 4, data_val); //显示“_ _ _ _”
	while (enter == 0)
	{
		//按键程序
		key_event();
		if (key_flag) //如果捕获到有按键事件的话，刷新键值，显示
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
				break; //确定小数点位置

			case 14:
				unit = i;
				data_in[i] = '\0';
				enter = 1;
				i++;
				break; //确定
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
 * 中断处理函数，用于处理UART数据
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
//******************************** 定时器中断
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
}
