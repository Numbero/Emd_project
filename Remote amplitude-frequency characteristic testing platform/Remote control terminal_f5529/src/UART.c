//****************from Numbero
#include "UART.h"


//UART串口初始化
void UART_Init()
{
	P3SEL = BIT3+BIT4;                        // P3.3/P3.4 = USCI_A0 TXD/RXD
    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**

    UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
    UCA0BR0 = 0x03;                           // 32kHz/9600=3.41 (see User's Guide)
    UCA0BR1 = 0x00;                           //
    UCA0MCTL = UCBRS_3+UCBRF_0;               // Modulation UCBRSx=3, UCBRFx=0
                                               // over sampling

    //UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    //UCA0BR0 = 6;                              // 1MHz 9600 (see User's Guide)
    //UCA0BR1 = 0;                              // 1MHz 9600
    //UCA0MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,

    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

    __bis_SR_register(GIE);       // interrupts enabled
}

//UART_WLAN串口初始化
void UART_WLAN_Init()
{
	P4SEL = BIT4+BIT5;                        // P4.4/P4.5 = USCI_A0 TXD/RXD
    UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**

    UCA1CTL1 |= UCSSEL_1;                     // CLK = ACLK
    UCA1BR0 = 0x03;                           // 32kHz/9600=3.41 (see User's Guide)
    UCA1BR1 = 0x00;                           //
    UCA1MCTL = UCBRS_3+UCBRF_0;               // Modulation UCBRSx=3, UCBRFx=0
                                               // over sampling

    //UCA1CTL1 |= UCSSEL_2;                     // SMCLK
    //UCA1BR0 = 6;                              // 1MHz 9600 (see User's Guide)
    //UCA1BR1 = 0;                              // 1MHz 9600
    //UCA1MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,

    UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UCA1IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

    __bis_SR_register(GIE);       // interrupts enabled
}
//UART发送一个字节
void UART_Send_Byte(unsigned char data)
{
    while (!(UCA0IFG&UCTXIFG));             // USCI_A0 TX buffer ready?
    UCA0TXBUF = data;
}
//UART_WLAN发送一个字节
void UART_WLAN_Send_Byte(unsigned char data)
{
    while (!(UCA1IFG&UCTXIFG));             // USCI_A0 TX buffer ready?
    UCA1TXBUF = data;
}
//UART发送一个字符串
void UART_Send_Str(char *s)
{
	int i=0;
	while(s[i]!=0)
	{
		UART_Send_Byte(s[i]);
		//_delay_ms(50);
		i++;
	}
}
//UART_WLAN发送一个字符串
void UART_WLAN_Send_Str(char *s)
{
	int i=0;
	while(s[i]!=0)
	{
		UART_WLAN_Send_Byte(s[i]);
		//_delay_ms(50);
		i++;
	}
}
//UART发送结束符
void UART_WLAN_Send_END(void)
{
	UART_WLAN_Send_Byte(0xFF);
	UART_WLAN_Send_Byte(0xFF);
	UART_WLAN_Send_Byte(0xFF);
}

//UART发送结束符
void UART_Send_END(void)
{
	 UART_Send_Byte(0xFF);
	 UART_Send_Byte(0xFF);
	 UART_Send_Byte(0xFF);
}
//UART开始发送
void UART_Send_start(void)
{
	 _delay_ms(10);
	 UART_Send_END();
	 _delay_ms(10);
}



