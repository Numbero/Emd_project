#include <msp430.h>
#include "delay.h"

#ifndef key_H
#define key_H

char SCAN_IN();

extern unsigned char key_pressed;//按键是否按下标志位
extern unsigned char key_val;//按键值存储变量
extern unsigned char key_flag;//按键事件标志

void SCAN(int row);
extern void key_init();
extern void check_key();
extern void key_event();

#endif
