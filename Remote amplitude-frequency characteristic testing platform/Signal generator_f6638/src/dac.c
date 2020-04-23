#include"dac.h"

void dac_init()
{
	//P6.6
	// AVcc is used as reference, calibration on, DAC on
	  DAC12_0CTL0 = DAC12IR + DAC12SREF_1 + DAC12AMP_5 + DAC12CALON;
	  DAC12_0CTL0 |= DAC12ENC;                  // Enable DAC12
}

void dac_init_0()
{
	//第二控制p6.7
	 DAC12_1CTL0 = DAC12IR + DAC12SREF_1 + DAC12AMP_5 + DAC12CALON;
	 DAC12_1CTL0 |= DAC12ENC;                  // Enable DAC12
}

void dac_start(unsigned int data)
{
	  DAC12_0DAT = data;                       // 12位有效值，例如：0x000。
}

void dac_start_0(unsigned int data)
{
	  DAC12_1DAT = data;                       // 12位有效值，例如：0x000。
}
