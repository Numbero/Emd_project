#include <msp430.h>
#include "delay.h"

#ifndef UART_H
#define UART_H


//UART串口初始化
void UART_Init();
//UART_WLAN串口初始化
void UART_WLAN_Init();
//UART发送一个字节
void UART_Send_Byte(unsigned char mydata);
//UART_WLAN发送一个字节
void UART_WLAN_Send_Byte(unsigned char mydata);
//UART发送一个字符串
void UART_Send_Str(char *s);
//UART_WLAN发送一个字符串
void UART_WLAN_Send_Str(char *s);
//UART发送结束符
void UART_Send_END(void);
//UART发送结束符
void UART_WLAN_Send_END(void);
//UART开始发送
void UART_Send_start(void);

#endif
