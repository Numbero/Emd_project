#include"AD9854_SPI.h"
#include  <msp430.h>
#include "general.h"

//******************************************************************************

//*********************************
//全局频率控制字
//*********************************
uchar Freq_Word[6];
uchar Shape_Word[2];

#define			CLK_Set	0x47	//7倍频，外置30MHz,得到210MHz
const ulong		Freq_Set_ulong = 1340357;
const double	Freq_Set_double = 1340357.031955505;
//******************************************************************************

unsigned char AD9854_Reg_PHA1[2]	={0x00,0x00};
unsigned char AD9854_Reg_PHA2[2]	={0x00,0x00};
unsigned char AD9854_Reg_FRE1[6]	={0x00,0xF0,0x00,0x00,0x00,0x00};
unsigned char AD9854_Reg_FRE2[6]	={0x80,0x00,0x00,0x00,0x00,0x00};
unsigned char AD9854_Reg_DELTA[6]	={0x00,0x00,0x00,0x10,0x00,0x00};
unsigned char AD9854_Reg_UPDATA[4]	={0x00,0x00,0x00,0x00};
unsigned char AD9854_Reg_RAMP_CLK[3]={0x00,0x00,0x01};
unsigned char AD9854_Reg_CTR_REG[4]	={	0x00,       //写控制寄存器
										CLK_Set,   	//设置系统时钟倍频为7倍频，系统时钟为210MHz
			   							0x00,       //设置系统为模式0，由外部更新
										0x60};     	//设置为可调节幅度，取消插值补偿
unsigned char AD9854_Reg_I_MUL[2]	={0x00,0x00};
unsigned char AD9854_Reg_Q_MUL[2]	={0x00,0x00};
unsigned char AD9854_Reg_SHAPED[1]	={0x00};
unsigned char AD9854_Reg_Q_DAC[2]	={0x00,0x00};

////////////////////////////////////////////////////////////////////
//                                                                //
//                             BASIC FUNCTION                     //
//                                                                //
////////////////////////////////////////////////////////////////////

//******************************************************************************
//函数名：SpisendByte(uchar dat)
//输入：发送的数据
//输出：无
//功能描述：SPI发送一个字节
//******************************************************************************
uchar SpiTxRxByte(uchar dat)
{
//----------------------以下是模拟SPI时序方式-----------------------------------
	uchar i,temp;
	temp = 0;	
	AD9854_SCLK_DOWN;
	for(i=0; i<8; i++)
	{
		if(dat & 0x80)
		{
	     AD9854_SDIO_UP;
		}
		else 
		{
		 AD9854_SDIO_DOWN;
		}
		dat <<= 1;
	        AD9854_SCLK_UP; 
		temp <<= 1;
	if(AD9854_SDO_IN)temp++;  //读取MISO状态
		AD9854_SCLK_DOWN;
	}
	return temp;
}

//******************************************************************************
//函数名：AD9854_SPIWriteBurstReg(uchar addr, uchar *buffer, uchar count)
//输入：地址，写入缓冲区，写入个数
//输出：无
//功能描述：SPI连续写配置寄存器
//******************************************************************************
void AD9854_SPIWriteBurstReg(uchar addr, uchar *buffer, uchar count)
{
    uchar i,temp;
    temp = (addr & 0x0F); 				// Send Write address
	AD9854_CS_DOWN;
	while (AD9854_SDO_IN);
	SpiTxRxByte(temp);
    for (i = 0; i < count; i++)
    {
        SpiTxRxByte(buffer[i]);
    }
    AD9854_CS_UP;
}

//******************************************************************************
//函数名：void AD9854_SPIReadBurstReg(uchar addr, uchar *buffer, uchar count)
//输入：地址，读出数据后暂存的缓冲区，读出配置个数
//输出：无
//功能描述：SPI连续读配置寄存器
//******************************************************************************
void AD9854_SPIReadBurstReg(uchar addr, uchar *buffer, uchar count)
{
  unsigned int i,temp;
  temp = ((addr & 0x0F) | 0x80);  		// Send Read address
  AD9854_CS_DOWN;
  while (AD9854_SDO_IN);
  SpiTxRxByte(temp); 
  for (i = 0; i < count; i++) 
	{
      buffer[i] = SpiTxRxByte(0);
    }
  AD9854_CS_UP;
}

//函数功能:更新设置
//输入参数:无
//******************************************************************
void AD9854_Update(void)
{
	AD9854_UPDATE_UP;      //原来肯定是低电平,所以我觉得先置高好
	_NOP() ;
	_NOP() ;
	_NOP() ;
	AD9854_UPDATE_DOWN;
}
//******************************************************************

//函数功能:复位
//输入参数:无
//******************************************************************
void AD9854_Io_Reset(void)
{
	unsigned i=100 ;        //
	AD9854_IO_RESET_UP ;
	while(i--) ;
	AD9854_IO_RESET_DOWN ;
}
//******************************************************************

//函数功能:AD9854初始化 写控制寄存器
//输入参数:无
//硬件说明：
//******************************************************************
void AD9854_Init(void)
{
	P6DIR |= 0x3f;
	P6DIR &= ~BIT6;       //初始化管脚类型

	AD9854_CS_DOWN ;        //片选	
	AD9854_UPDATE_DOWN;		//拉低更新时钟，进行设置

	AD9854_RESET_DOWN;
	AD9854_RESET_UP;	
	DelayMs(10);
	AD9854_RESET_DOWN;		//主复位
	AD9854_Io_Reset();		//IO复位

	AD9854_SPIWriteBurstReg(AD9854_Addr_CTR_REG,		//写控制寄存器
						AD9854_Reg_CTR_REG, AD9854_Length_CTR_REG);			

	AD9854_UPDATE_UP;		//提高更新时钟，更新配置
	DelayMs(10);
	AD9854_UPDATE_DOWN;		//配置更新时钟为输出
}
//******************************************************************
//幅度转换函数
//******************************************************************
void AD9854_Shape_Convey(uint Shape)
{
	Shape_Word[0] = (uchar)(Shape>>8);	//最低位8位
	Shape_Word[1] = (uchar)(Shape&0xff);
}
//******************************************************************
//函数功能：正弦信号频率数据转换
//说明：需要转换的频率值
//	 系统时钟为300MHz
//	该算法位多字节相乘算法，有公式FTW = (Desired Output Frequency × 2N)/SYSCLK
// 得到该算法，其中N=48，Desired Output Frequency 为所需要的频率，即Freq，SYSCLK
// 为可编程的系统时钟，FTW为48Bit的频率控制字，即Freq_Word[6]
//******************************************************************
void AD9854_Freq_Convey1(ulong Freq)
{
	ulong FreqBuf;
	ulong Temp = Freq_Set_ulong;

	uchar Array_Freq[4];	//将输入频率因子分为四个字节
	Array_Freq[0] = (uchar)Freq;	//最低位四位
	Array_Freq[1] = (uchar)(Freq>>8);
	Array_Freq[2] = (uchar)(Freq>>16);
	Array_Freq[3] = (uchar)(Freq>>24);	//最高四位

	FreqBuf = Temp * Array_Freq[0];
    Freq_Word[0] = FreqBuf;
    FreqBuf >>= 8;

    FreqBuf += (Temp * Array_Freq[1]);
	Freq_Word[1] = FreqBuf;
	FreqBuf >>= 8;

	FreqBuf += (Temp * Array_Freq[2]);
	Freq_Word[2] = FreqBuf;
	FreqBuf >>= 8;

	FreqBuf += (Temp * Array_Freq[3]);
	Freq_Word[3] = FreqBuf;
	FreqBuf >>= 8;

	Freq_Word[4] = FreqBuf;
	Freq_Word[5] = FreqBuf>>8;
}
//******************************************************************
//函数名称:void Freq_doublt_convert(double Freq)
//函数功能:正弦信号频率数据转换
//入口参数:Freq   需要转换的频率，取值从0~SYSCLK/2
//出口参数:无   但是影响全局变量Freq_Word[6]的值
//说明：   有公式FTW = (Desired Output Frequency × 2N)/SYSCLK得到该函数，
//         其中N=48，Desired Output Frequency 为所需要的频率，即Freq，SYSCLK
//         为可编程的系统时钟，FTW为48Bit的频率控制字，即Freq_Word[6]
//注意：   该函数与上面函数的区别为该函数的入口参数为double，可使信号的频率更精确
//         谷雨建议在100HZ以下用本函数，在高于100HZ的情况下用函数void Freq_convert(long Freq)
//******************************************************************
void AD9854_double_Freq_Convey(double Freq)
{
	ulong Low32;
	uint  High16;
    double Temp = Freq_Set_double;
	Freq*=(double)(Temp);
//	1 0000 0000 0000 0000 0000 0000 0000 0000 = 4294967295
	High16 = (int)(Freq/4294967295);                 //2^32 = 4294967295
	Freq -= (double)High16*4294967295;
	Low32 = (ulong)Freq;

    Freq_Word[0]=Low32;
    Freq_Word[1]=Low32>>8;
    Freq_Word[2]=Low32>>16;
    Freq_Word[3]=Low32>>24;
    Freq_Word[4]=High16;
    Freq_Word[5]=High16>>8;
}
//******************************************************************
//函数功能：正弦信号产生函数
//说明：Freq   频率设置，取值范围为0~(1/2)*SYSCLK
//    Shape  幅度设置. 为12 Bit,取值范围为(0~4095) ,取值越大,幅度越大
//******************************************************************
void AD9854_SetSine1(ulong Freq,uint Shape)
{
	AD9854_Freq_Convey1(Freq);        //频率转换
	AD9854_SPIWriteBurstReg(AD9854_Addr_FRE1,		//设置I通道频率
								Freq_Word, AD9854_Length_FRE1);

	AD9854_Shape_Convey(Shape);				   		   //幅度转换
	AD9854_SPIWriteBurstReg(AD9854_Addr_I_MUL,		//设置I通道幅度
								Shape_Word, AD9854_Length_I_MUL);

	AD9854_Update();	 			  //更新AD9854输出
}
//******************************************************************
//函数名称:void AD9854_SetSine_double(double Freq,uint Shape)
//函数功能:AD9854正弦波产生程序
//入口参数:Freq   频率设置，取值范围为0~1/2*SYSCLK
//         Shape  幅度设置. 为12 Bit,取值范围为(0~4095)
//出口参数:无
//******************************************************************
void AD9854_SetSine_double(double Freq,uint Shape)
{

	AD9854_double_Freq_Convey(Freq);		           //频率转换
	AD9854_SPIWriteBurstReg(AD9854_Addr_FRE1,		//设置I通道频率
								Freq_Word, AD9854_Length_FRE1);

	AD9854_Shape_Convey(Shape);				   		   //幅度转换
	AD9854_SPIWriteBurstReg(AD9854_Addr_I_MUL,		//设置I通道幅度
								Shape_Word, AD9854_Length_I_MUL);

	AD9854_Update();	 			  //更新AD9854输出
}

//******************************************************************
//函数功能：设置三角扫频
//输入参数:
//Serial Register Address :2，3，4，6
//写入地址字节数:1 Byte
//写入数据字节数:6 Bytes 
//******************************************************************
void Write_AD9854_FrqSW(void)
{ 	
	AD9854_SPIWriteBurstReg(AD9854_Addr_FRE1,		//写频率控制寄存器1
						AD9854_Reg_FRE1, AD9854_Length_FRE1);
		  AD9854_Io_Reset();
	AD9854_SPIWriteBurstReg(AD9854_Addr_FRE2,		//写频率控制寄存器2
						AD9854_Reg_FRE2, AD9854_Length_FRE2);
		  AD9854_Io_Reset();
	AD9854_SPIWriteBurstReg(AD9854_Addr_DELTA,		//写频率增量寄存器
						AD9854_Reg_DELTA, AD9854_Length_DELTA);
		  AD9854_Io_Reset();
	AD9854_SPIWriteBurstReg(AD9854_Addr_RAMP_CLK,	//写扫频时钟寄存器
						AD9854_Reg_RAMP_CLK, AD9854_Length_RAMP_CLK);

	AD9854_Update();	 			  //更新AD9854输出
}
