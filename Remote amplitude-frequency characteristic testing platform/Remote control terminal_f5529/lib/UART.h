#include <msp430.h>
#include "delay.h"

#ifndef UART_H
#define UART_H


//UART���ڳ�ʼ��
void UART_Init();
//UART_WLAN���ڳ�ʼ��
void UART_WLAN_Init();
//UART����һ���ֽ�
void UART_Send_Byte(unsigned char mydata);
//UART_WLAN����һ���ֽ�
void UART_WLAN_Send_Byte(unsigned char mydata);
//UART����һ���ַ���
void UART_Send_Str(char *s);
//UART_WLAN����һ���ַ���
void UART_WLAN_Send_Str(char *s);
//UART���ͽ�����
void UART_Send_END(void);
//UART���ͽ�����
void UART_WLAN_Send_END(void);
//UART��ʼ����
void UART_Send_start(void);

#endif
