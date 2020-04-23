//****************from Numbero
//该程序只能用于MSP430F6638
#include <msp430.h>
#include "UART.h"


void Port_Mapping(void)
{
  // Disable Interrupts before altering Port Mapping registers
  __disable_interrupt();
  // Enable Write-access to modify port mapping registers
  PMAPPWD = 0x02D52;

  #ifdef PORT_MAP_RECFG
  // Allow reconfiguration during runtime
  PMAPCTL = PMAPRECFG;
  #endif

  P2MAP0 = PM_UCA0TXD;
  P2MAP1 = PM_UCA0RXD;

  // Disable Write-Access to modify port mapping registers
  PMAPPWD = 0;
  #ifdef PORT_MAP_EINT
  __enable_interrupt();                     // Re-enable all interrupts
  #endif
}
//UART串口初始化
void UART_Init()
{
	Port_Mapping();

	P2SEL |= 0x03;                            // Assign P2.0 to UCA0TXD and...
	P2DIR |= 0x03;                            // P2.1 to UCA0RXD
    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**

    UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
    UCA0BR0 = 0x03;                           // 32kHz/9600=3.41 (see User's Guide)
    UCA0BR1 = 0x00;                           //
    UCA0MCTL = UCBRS_3+UCBRF_0;               // Modulation UCBRSx=3, UCBRFx=0

    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

    __bis_SR_register(GIE);       // interrupts enabled
}
//UART发送一个字节
void UART_Send_Byte(unsigned char data)
{
    while (!(UCA0IFG&UCTXIFG));             // USCI_A0 TX buffer ready?
    UCA0TXBUF = data;
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



