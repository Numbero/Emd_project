/****************spi.c*************************************/
#include "MY_SPI.h"

//SPI��ʼ��
void Spi_Init(void)
{
	P5DIR |=  BIT0+BIT1+BIT6;
	P5DIR &=~ BIT7;

	CS_1;
	CLK_0;
	_delay_ms(10);
}

//��ʼ����
void Spi_Start()
{
	CS_0;
	CLK_0;
	_delay_us(100);
}

//ֹͣ����
void Spi_Stop()
{
	CS_1;
	CLK_0;
	_delay_us(100);
}

//��������
void Spi_Repit()
{
	CS_1;
	CLK_0;
	_delay_us(1); //1us
	CS_0;
}

//д��һ���ֽ�
void Spi_WriteByte(uchar data)
{
	uchar i;
	CLK_0;
	for(i=0;i<8;i++)
    {
		CLK_0;
		if(0x80&data)
			DIO_1;
		else
			DIO_0;
		CLK_1;
		data<<=1;
    }
	CLK_0;
}

//��ȡһ���ֽ�
uchar Spi_ReadByte(void)
{
	uchar i,temp=0;
	CLK_0;               //ģʽ0
	for(i=0;i<8;i++)
    {
		temp<<=1;
		CLK_1;
		CLK_0;
		if(DO_IN)        //��ȡ���λ����������ĩβ��ͨ������λ��������ֽ�
			temp |= 0x01;
    }
	CLK_0;
	return temp;
}

//�����������,ʵ�����ݽ���
extern uchar Spi_TransByte(uchar data)
{
	uchar i,temp=0;
	CLK_0;               //ģʽ0

	for(i=0;i<8;i++)
    {
		if(0x80&data)	//������֮ǰ���������ݷ���
			DIO_1;
		else
			DIO_0;
		data<<=1;
		temp<<=1;
		CLK_1;			//�����ط�������
		CLK_0;			//�½��ؽ�������
		if(DO_IN)       //��ȡ���λ����������ĩβ��ͨ������λ��������ֽ�
			temp |= 0x01;
    }
	return temp;
}
