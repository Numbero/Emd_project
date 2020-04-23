#include "DAC_12.h"


//DAC��ʼ��
void DAC_12_Init()
{
	P6DIR |= BIT5 + BIT6;
	P7DIR |= BIT0;
	P6OUT &= ~(BIT5 + BIT6);
	P7OUT &= ~BIT0;
}

//����Aͨ��
void DAC_12_translate_a(unsigned int n)
{
	unsigned int i;
	unsigned int data_temp;
	data_temp=n;
	data_temp=data_temp|0x8000;
	DAC_CS_0;
	for(i=0;i<16;i++)
	{
		if(data_temp&0x8000)
			DAC_DI_1;
		else
			DAC_DI_0;
		DAC_SCK_0;
		data_temp<<=1;
		DAC_SCK_1;
	}
	DAC_CS_1;
}
//����Bͨ��
void DAC_12_translate_b(unsigned int n)
{
	unsigned int i;
	unsigned int data_temp;
	data_temp=n;
	data_temp=data_temp|0x0000;
	DAC_CS_0;
	for(i=0;i<16;i++)
	{
		if(data_temp&0x8000)
			DAC_DI_1;
		else
			DAC_DI_0;
		DAC_SCK_0;
		data_temp<<=1;
		DAC_SCK_1;
	}
	DAC_CS_1;
}
