#include<msp430.h>
#include"delay.h"

#ifndef dac_H
#define dac_H

extern void dac_init();
extern void dac_init_0();
extern void dac_start(unsigned int data);
extern void dac_start_0(unsigned int data);

#endif
