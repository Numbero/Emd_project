#include"AD9854_Parallel.h"
#include  <msp430.h>
#include "general.h"

//******************************************************************

//*********************************
//全局频率控制字
//*********************************
uchar Freq_Word[6];
//主时钟配置
//#define		CLK_Set	0x04	//4倍频，外置30MHz,得到120MHz
//const ulong	Freq_Set_ulong = 2345625;
//const double	Freq_Set_double = 2345624.805922133;

//#define		CLK_Set	0x45	//5倍频，外置30MHz,得到150MHz
//const ulong	Freq_Set_ulong = 1876499;
//const double	Freq_Set_double = 1876499.844737707;

//#define		CLK_Set	0x46	//6倍频，外置30MHz,得到180MHz
//const ulong	Freq_Set_ulong = 1563749;
//const double	Freq_Set_double = 1563749.870614756;

#define			CLK_Set	0x47	//7倍频，外置30MHz,得到210MHz
const ulong		Freq_Set_ulong = 1340357;
const double	Freq_Set_double = 1340357.031955505;

//#define			CLK_Set	0x48	//8倍频，外置30MHz,得到240MHz
//const ulong		Freq_Set_ulong = 1172812;
//const double	Freq_Set_double = 1172812.402961067;

//#define		CLK_Set	0x4a	//10倍频，外置30MHz,得到300MHz
//const ulong	Freq_Set_ulong = 938250;
//const double	Freq_Set_double = 938249.922368853;
//******************************************************************

unsigned char AD9854_Reg_PHA1[2]	={0x00,0x00};
unsigned char AD9854_Reg_PHA2[2]	={0x00,0x00};

unsigned char AD9854_Reg_FRE1[6]	={0x00,0x10,0x00,0x10,0x00,0x00};
unsigned char AD9854_Reg_FRE2[6]	={0x80,0x00,0x00,0x00,0x00,0x00};

unsigned char AD9854_Reg_DELTA[6]	={0x00,0x00,0x00,0x10,0x00,0x00};
unsigned char AD9854_Reg_UPDATA[4]	={0x00,0x00,0x00,0x00};
unsigned char AD9854_Reg_RAMP_CLK[3]={0x00,0x00,0x01};
unsigned char AD9854_Reg_CTR_REG[4]	={	Bin(00010000),//0x10       //comp pd dowm  ctr[28]=0 //qac pd down ctr[26]=0
                           				Bin(01000110),//0x46       //no pll pass ctr[21]=0  //multi times 6 ctr[19-16]=00110
			   							Bin(00100100),//0x24       //ext update ctr[8]=0    //Ramped FSK
                           				Bin(01000000) };//0x40     //pass inv CR[6]=1//osk EN ctr[5]=0 //msb first ctr[1]=0 //no sdo active ctr[0]=0
//*********************************
unsigned char AD9854_Reg_I_MUL[2]	={0x00,0x00};
unsigned char AD9854_Reg_Q_MUL[2]	={0x00,0x00};

unsigned char AD9854_Reg_ShapeD[1]	={0x00};
unsigned char AD9854_Reg_Q_DAC[2]	={0x00,0x00};

////////////////////////////////////////////////////////////////////
//                                                                //
//                             BASIC FUNCTION                     //
//                                                                //
////////////////////////////////////////////////////////////////////

//******************************************************************************
//函数名：AD9854_Parallel_Setup(void)
//输入：无
//输出：无
//功能描述：并行总线端口设置
//******************************************************************************
void AD9854_Parallel_Setup(void)
{
	Parallel_Addr_SET;
	Parallel_Addr_OUT = 0x00;

	Parallel_Data_Setout;
	Parallel_Data_OUT = 0x00;

	AD9854_CRL_SET;
	AD9854_CRL_OUT = 0x00;
}
//******************************************************************************
//函数名：void AD9854_WR_Byte(uchar addr, uchar data);
//输入：地址，写入缓冲区，写入个数
//输出：无
//功能描述：并行连续写配置寄存器
//******************************************************************************
void AD9854_WR_Byte(uchar addr, uchar data)
{
 	AD9854_RD_UP;
	AD9854_WR_UP;
	Parallel_Addr_OUT = addr;
	Parallel_Data_OUT = data;
	AD9854_WR_DOWN;
	_NOP() ;
	_NOP() ;
	AD9854_WR_UP;
}

//******************************************************************************
//函数名：AD9854_Parallel_WriteBurstReg(uchar addr, uchar *buffer, uchar count)
//输入：地址，写入缓冲区，写入个数
//输出：无
//功能描述：并行连续写配置寄存器
//******************************************************************************
void AD9854_Parallel_WriteBurstReg(uchar addr, uchar *buffer, uchar count)
{
    uchar i;
 	AD9854_RD_UP;
	AD9854_WR_UP;
    for (i = 0; i < count; i++)
    {
	  Parallel_Addr_OUT = addr + i;		// Send address
      Parallel_Data_OUT = buffer[i];   	// Send data
	  AD9854_WR_DOWN;
	  _NOP() ;
	  _NOP() ;
      AD9854_WR_UP;
    }
}

//***************************************************************************
//函数名：void AD9854_Parallel_ReadBurstReg(uchar addr, uchar *buffer, uchar count)
//输入：地址，读出数据后暂存的缓冲区，读出配置个数
//输出：无
//功能描述：并行连续读配置寄存器
//******************************************************************************
void AD9854_Parallel_ReadBurstReg(uchar addr, uchar *buffer, uchar count)
{
  unsigned int i;
  Parallel_Data_Setin;
  AD9854_RD_UP;
  for (i = 0; i < count; i++)
  {	
	Parallel_Addr_OUT = addr + i;		// Send address
	AD9854_RD_DOWN;
	_NOP() ;
	_NOP() ;
    buffer[i] = Parallel_Data_IN;     	// Store data
	AD9854_RD_UP;
  }
  Parallel_Data_Setout;           
}
//******************************************************************
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
//函数功能:AD9854初始化 写控制寄存器
//输入参数:无
//硬件说明：
//******************************************************************
void AD9854_Init(void)
{
	AD9854_Parallel_Setup();		//初始化总线

	AD9854_WR_UP;
	AD9854_RD_UP;
	AD9854_UPDATE_DOWN;		//拉低更新时钟，进行设置

	AD9854_RESET_DOWN;
	AD9854_RESET_UP;	
	DelayMs(10);
	AD9854_RESET_DOWN;		//主复位	

	AD9854_WR_Byte(0x1d,0x00);		//写控制寄存器
	AD9854_WR_Byte(0x1e,CLK_Set);	//设置系统时钟倍频为7倍频，系统时钟为210MHz
	AD9854_WR_Byte(0x1f,0x00);		//设置系统为模式0，由外部更新
	AD9854_WR_Byte(0x20,0x60);		//设置为可调节幅度，取消插值补偿

	AD9854_UPDATE_UP;			//提高更新时钟，更新配置
	DelayMs(10);
	AD9854_UPDATE_DOWN;		//拉低更新时钟，结束修改
}
//******************************************************************
//函数功能：正弦信号频率数据转换
//******************************************************************
void AD9854_Freq_Convey(double Freq)
{
	Freq_Word[0]=((uint32_t)((Freq*256)/200000000))&(0x0000000000ff);
	Freq_Word[1]=((uint32_t)((Freq*65536)/200000000))&(0x0000000000ff);
	Freq_Word[2]=((uint32_t)((Freq*16777216)/200000000))&(0x0000000000ff);
	Freq_Word[3]=((uint32_t)((Freq*4294967296)/200000000))&(0x0000000000ff);
	Freq_Word[4]=((uint32_t)((Freq*1099511627776)/200000000))&(0x0000000000ff);
	Freq_Word[5]=((uint32_t)((Freq*281474976710656)/200000000))&(0x0000000000ff);
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
//******************************************************************
void AD9854_SetSine(double Freq,uint Shape)
{
	AD9854_Freq_Convey(Freq);

	AD9854_Parallel_WriteBurstReg(AD9854_Addr_FRE1,	//I路频率设置
			Freq_Word, AD9854_Length_FRE1);
	//AD9854_Parallel_WriteBurstReg(AD9854_Addr_FRE2,	//Q路频率设置
	//		Freq_Word, AD9854_Length_FRE2);

	AD9854_WR_Byte(AD9854_Addr_I_MUL, Shape>>8);
	AD9854_WR_Byte(AD9854_Addr_I_MUL + 1, (uchar)(Shape&0xff));

	//AD9854_WR_Byte(AD9854_Addr_Q_MUL, Shape>>8);
	//AD9854_WR_Byte(AD9854_Addr_Q_MUL + 1, (uchar)(Shape&0xff));

	AD9854_Update();
}
//******************************************************************
//函数功能：正弦信号产生函数
//说明：Freq   频率设置，取值范围为0~(1/2)*SYSCLK
//    Shape  幅度设置. 为12 Bit,取值范围为(0~4095) ,取值越大,幅度越大
//******************************************************************
void AD9854_SetSine1(ulong Freq,uint Shape)
{
	uchar count = 0;
	uchar Adress1 = AD9854_Addr_FRE1;
	//uchar Adress2 = AD9854_Addr_FRE2;

	AD9854_Freq_Convey1(Freq);        //频率转换

	for(count = 6;count > 0;)	          //写入6字节的频率控制字
	{
		AD9854_WR_Byte(Adress1 ++, Freq_Word[--count]);
    }

	//for(count=6;count>0;)	          //写入6字节的频率控制字
	//{
	//	AD9854_WR_Byte(Adress2 ++,Freq_Word[--count]);
	//}

	AD9854_WR_Byte(0x21,Shape >> 8);	  //设置I通道幅度
	AD9854_WR_Byte(0x22,(uchar)(Shape & 0xff));

	//AD9854_WR_Byte(0x23,Shape>>8);	  //设置Q通道幅度
	//AD9854_WR_Byte(0x24,(uchar)(Shape&0xff));

	AD9854_Update();	 			  //更新AD9854输出
}

void AD9854_SetSine2(ulong Freq)
{
	uchar count = 0;
	uchar Adress1 = AD9854_Addr_FRE1;
	//uchar Adress2 = AD9854_Addr_FRE2;

	AD9854_Freq_Convey1(Freq);        //频率转换

	for(count = 6;count > 0;)	          //写入6字节的频率控制字
	{
		AD9854_WR_Byte(Adress1 ++, Freq_Word[--count]);
    }
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
	uchar count = 0;
	uchar Adress1 = AD9854_Addr_FRE1;
	//uchar Adress2 = AD9854_Addr_FRE2;

	AD9854_double_Freq_Convey(Freq);		           //频率转换

	for(count=6;count>0;)	                    //写入6字节的频率控制字
    {
		AD9854_WR_Byte(Adress1 ++,Freq_Word[--count]);
    }

	//for(count=6;count>0;)	          //写入6字节的频率控制字
	//{
	//	AD9854_WR_Byte(Adress2 ++,Freq_Word[--count]);
	//}

	AD9854_WR_Byte(0x21,Shape >> 8);	  //设置I通道幅度
	AD9854_WR_Byte(0x22,(uchar)(Shape & 0xff));

	//AD9854_WR_Byte(0x23,Shape>>8);	  //设置Q通道幅度
	//AD9854_WR_Byte(0x24,(uchar)(Shape&0xff));

	AD9854_Update();	 			  //更新AD9854输出
}

//******************************************************************
//函数功能：写 Frequency Tuning Word #1
//输入参数: frq1(指针,指向unsigned char型数组的首地址,数组长度为48,6个字节)
//Serial Register Address :2
//写入地址字节数:1 Byte
//写入数据字节数:6 Bytes 
//******************************************************************
void AD9854_WR_Frq1(void)
{ 	
	AD9854_Parallel_WriteBurstReg(AD9854_Addr_FRE1,		//写频率控制寄存器1
						AD9854_Reg_FRE1, AD9854_Length_FRE1);
	AD9854_Update();
}

//******************************************************************
//函数功能：设置三角扫频
//输入参数:
//Serial Register Address :2，3，4，6
//写入地址字节数:1 Byte
//写入数据字节数:6 Bytes 
//******************************************************************
void AD9854_WR_FrqSW(void)
{ 	
	AD9854_Parallel_WriteBurstReg(AD9854_Addr_FRE1,		//写频率控制寄存器1
						AD9854_Reg_FRE1, AD9854_Length_FRE1);
		
	AD9854_Parallel_WriteBurstReg(AD9854_Addr_FRE2,		//写频率控制寄存器2
						AD9854_Reg_FRE2, AD9854_Length_FRE2);
	
	AD9854_Parallel_WriteBurstReg(AD9854_Addr_DELTA,		//写频率增量寄存器
						AD9854_Reg_DELTA, AD9854_Length_DELTA);
	
	AD9854_Parallel_WriteBurstReg(AD9854_Addr_RAMP_CLK,	//写扫频时钟寄存器
						AD9854_Reg_RAMP_CLK, AD9854_Length_RAMP_CLK);
	AD9854_Update();
}
//====================================================================================
//函数名称:void AD9854_InitRFSK(void)
//函数功能:AD9854的RFSK初始化
//入口参数:无
//出口参数:无
//====================================================================================
void AD9854_InitRFSK(void)
{
	AD9854_WR_UP;                      //将读、写控制端口设为无效
    AD9854_RD_UP;
    AD9854_UPDATE_DOWN;
    AD9854_RESET_UP;                     //复位AD9854
    AD9854_RESET_DOWN;

	AD9854_WR_Byte(0x1d,0x00);	       //关闭比较器
	AD9854_WR_Byte(0x1e,CLK_Set);	   //设置系统时钟倍频
	AD9854_WR_Byte(0x1f,0x24);	        //设置系统为模式2，由外部更新,使能三角波扫频功能
	AD9854_WR_Byte(0x20,0x60);	        //设置为可调节幅度，取消插值补偿

	AD9854_Update();
}

//====================================================================================
//函数名称:void AD9854_SetRFSK(void)
//函数功能:AD9854的RFSK设置
//入口参数:Freq_Low          RFSK低频率	   48Bit
//         Freq_High         RFSK高频率	   48Bit
//         Freq_Up_Down		 步进频率	   48Bit
//		   FreRate           斜率时钟控制  20Bit
//出口参数:无
//注：     每两个脉冲之间的时间周期用下式表示（FreRate +1）*（System Clock ），一个脉冲,
//         频率 上升或者下降 一个步进频率
//====================================================================================
void AD9854_SetRFSK(ulong Freq_Low,ulong Freq_High,ulong Freq_Up_Down,ulong FreRate,uint Shape)
{
	uchar count=6;
	uchar Adress1,Adress2,Adress3;
    //const uint  Shape=4000;			   //幅度设置. 为12 Bit,取值范围为(0~4095)

	Adress1=0x04;		     //选择频率控制字地址的初值
	Adress2=0x0a;
	Adress3=0x10;

	AD9854_Freq_Convey1(Freq_Low);                   //频率1转换
	for(count=6;count>0;)	                         //写入6字节的频率控制字
    {
		AD9854_WR_Byte(Adress1++,Freq_Word[--count]);
    }

	AD9854_Freq_Convey1(Freq_High);                         //频率2转换
	for(count=6;count>0;)	                         //写入6字节的频率控制字
    {
		AD9854_WR_Byte(Adress2++,Freq_Word[--count]);
    }

	AD9854_double_Freq_Convey(Freq_Up_Down);               //步进频率转换
	for(count=6;count>0;)	                         //写入6字节的频率控制字
    {
		AD9854_WR_Byte(Adress3++,Freq_Word[--count]);
    }

	AD9854_WR_Byte(0x1a,(uchar)((FreRate>>16)&0x0f));	//设置斜升速率
	AD9854_WR_Byte(0x1b,(uchar)(FreRate>>8));
	AD9854_WR_Byte(0x1c,(uchar)FreRate);

	AD9854_WR_Byte(0x21,Shape>>8);	                  	//设置I通道幅度
	AD9854_WR_Byte(0x22,(uchar)(Shape&0xff));
	AD9854_WR_Byte(0x23,Shape>>8);	                  //设置Q通道幅度
	AD9854_WR_Byte(0x24,(uchar)(Shape&0xff));

	AD9854_Update();
}
