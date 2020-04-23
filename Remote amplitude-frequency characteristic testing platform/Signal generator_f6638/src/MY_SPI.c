/****************spi.c*************************************/
#include "MY_SPI.h"

//SPI初始化
void Spi_Init(void)
{
	P5DIR |=  BIT0+BIT1+BIT6;
	P5DIR &=~ BIT7;

	CS_1;
	CLK_0;
	_delay_ms(10);
}

//开始传输
void Spi_Start()
{
	CS_0;
	CLK_0;
	_delay_us(100);
}

//停止传输
void Spi_Stop()
{
	CS_1;
	CLK_0;
	_delay_us(100);
}

//继续发送
void Spi_Repit()
{
	CS_1;
	CLK_0;
	_delay_us(1); //1us
	CS_0;
}

//写入一个字节
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

//读取一个字节
uchar Spi_ReadByte(void)
{
	uchar i,temp=0;
	CLK_0;               //模式0
	for(i=0;i<8;i++)
    {
		temp<<=1;
		CLK_1;
		CLK_0;
		if(DO_IN)        //读取最高位，保存至最末尾，通过左移位完成整个字节
			temp |= 0x01;
    }
	CLK_0;
	return temp;
}

//并行输入输出,实现数据交换
extern uchar Spi_TransByte(uchar data)
{
	uchar i,temp=0;
	CLK_0;               //模式0

	for(i=0;i<8;i++)
    {
		if(0x80&data)	//上升沿之前将发送数据放上
			DIO_1;
		else
			DIO_0;
		data<<=1;
		temp<<=1;
		CLK_1;			//上升沿发送数据
		CLK_0;			//下降沿接收数据
		if(DO_IN)       //读取最高位，保存至最末尾，通过左移位完成整个字节
			temp |= 0x01;
    }
	return temp;
}
