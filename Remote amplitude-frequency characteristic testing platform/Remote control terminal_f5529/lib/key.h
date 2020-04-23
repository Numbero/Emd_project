#include <msp430.h>
#include "delay.h"

#ifndef key_H
#define key_H

char SCAN_IN();

extern unsigned char key_pressed;//�����Ƿ��±�־λ
extern unsigned char key_val;//����ֵ�洢����
extern unsigned char key_flag;//�����¼���־

void SCAN(int row);
extern void key_init();
extern void check_key();
extern void key_event();

#endif
