#include <msp430.h>


#define  ulong   unsigned long
#define  uchar    unsigned char
#define  uint     unsigned int

#define LongToBin(n) (((n>>21)&0x80)|((n>>18)&0x40)|((n>>15)&0x20)|((n>>12)&0x10)|((n>>9)&0x08)|((n>>6)&0x04)|((n>>3)&0x02)|((n)&0x01)) 
#define Bin(n) LongToBin(0x##n##l)
//******************************************************************
//端口操作
#define AD9854_CS_UP       P6OUT |= BIT0
#define AD9854_CS_DOWN     P6OUT &=~BIT0   	//片选,RD

#define AD9854_SCLK_UP     P6OUT |= BIT1
#define AD9854_SCLK_DOWN   P6OUT &=~BIT1	//数据时钟,WR

#define AD9854_UPDATE_UP   P6OUT |= BIT2
#define AD9854_UPDATE_DOWN P6OUT &=~BIT2	//更新时钟,UP

#define AD9854_SDIO_UP     P6OUT |= BIT3
#define AD9854_SDIO_DOWN   P6OUT &=~BIT3	//数据输出,A0

#define AD9854_SDO_IN      (P6IN & 0x40) 	//数据输入,A1

#define AD9854_IO_RESET_UP    P6OUT |= BIT4
#define AD9854_IO_RESET_DOWN  P6OUT &=~BIT4	//SPI总线复位,IO/RST管脚，A2

#define AD9854_RESET_UP    P6OUT |= BIT5
#define AD9854_RESET_DOWN  P6OUT &=~BIT5   	//主复位,RST管脚

#define  HARDWARE_AD9854   P6DIR |= 0x3F;P6DIR & =~BIT6


////////////////////////////////////////////////////////////////////
//                                                                //
//                             INSTRUCTION BYTE                   //
//                                                                //
////////////////////////////////////////////////////////////////////

//******************************************************************
//寄存器地址 注意是串行地址
#define	  AD9854_Addr_PHA1       0        	// 2 Bytes
#define	  AD9854_Addr_PHA2       1        	// 2 Bytes 
#define	  AD9854_Addr_FRE1       2        	// 6 Bytes	
#define	  AD9854_Addr_FRE2       3        	// 6 Bytes
#define	  AD9854_Addr_DELTA      4        	// 6 Bytes
#define	  AD9854_Addr_UPDATA     5        	// 4 Bytes
#define	  AD9854_Addr_RAMP_CLK   6        	// 3 Bytes
#define	  AD9854_Addr_CTR_REG    7        	// 4 Bytes
#define	  AD9854_Addr_I_MUL      8        	// 2 Bytes
#define	  AD9854_Addr_Q_MUL      9        	// 2 Bytes
#define	  AD9854_Addr_SHAPED     10       	// 1 Bytes
#define	  AD9854_Addr_Q_DAC      11        	// 2 Bytes
//******************************************************************

//******************************************************************
//寄存器长度
#define	  AD9854_Length_PHA1       2        	// 2 Bytes
#define	  AD9854_Length_PHA2       2        	// 2 Bytes 
#define	  AD9854_Length_FRE1       6        	// 6 Bytes	
#define	  AD9854_Length_FRE2       6        	// 6 Bytes
#define	  AD9854_Length_DELTA      6        	// 6 Bytes
#define	  AD9854_Length_UPDATA     4        	// 4 Bytes
#define	  AD9854_Length_RAMP_CLK   3        	// 3 Bytes
#define	  AD9854_Length_CTR_REG    4        	// 4 Bytes
#define	  AD9854_Length_I_MUL      2        	// 2 Bytes
#define	  AD9854_Length_Q_MUL      2        	// 2 Bytes
#define	  AD9854_Length_SHAPED     1       		// 1 Bytes
#define	  AD9854_Length_Q_DAC      2        	// 2 Bytes

////////////////////////////////////////////////////////////////////
//                                                                //
//                          VARIABLES   DEFINITION                //
//                                                                //
////////////////////////////////////////////////////////////////////




extern void AD9854_SPIWriteBurstReg(uchar addr, uchar *buffer, uchar count);		//连续写寄存器
extern void AD9854_SPIReadBurstReg(uchar addr, uchar *buffer, uchar count);			//连续读寄存器

extern void AD9854_Update(void);							//更新当前配置
extern void AD9854_Io_Reset(void);							//IO复位
extern void AD9854_Init(void);								//初始化AD9854

extern void AD9854_Shape_Convey(uint Shape);				//幅度转换函数
extern void AD9854_Freq_Convey1(ulong Freq);				//频率转换函数
extern void AD9854_double_Freq_Convey(double Freq);

extern void AD9854_SetSine1(ulong Freq,uint Shape);			//正弦波产生
extern void AD9854_SetSine_double(double Freq,uint Shape);

extern void Write_AD9854_FrqSW(void);						//设定扫频





