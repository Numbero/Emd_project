#include <stdint.h>
/* macro for input binary */ 
#define LongToBin(n) ( ((n >> 21) & 0x80) | ((n >> 18) & 0x40) | ((n >> 15) & 0x20) | ((n >> 12) & 0x10) | ((n >> 9) & 0x08) | ((n >> 6) & 0x04) | ((n >> 3) & 0x02) | ((n ) & 0x01) ) 
#define Bin(n) LongToBin(0x##n##l)

#define  ulong   unsigned long
#define  uchar   unsigned char
#define  uint    unsigned int
//******************************************************************
//端口操作
#define Parallel_Addr_SET  P1DIR |= 0x3F       //地址定义
#define Parallel_Addr_OUT  P1OUT       //

#define Parallel_Data_Setout  P3DIR |= 0xFF       //数据定义
#define Parallel_Data_Setin   P3DIR &= 0x00       //
#define Parallel_Data_OUT  P3OUT       //
#define Parallel_Data_IN   P3IN       //

#define AD9854_RD_UP       	P2OUT |= BIT5
#define AD9854_RD_DOWN     	P2OUT &=~BIT5   //模式选择

#define AD9854_WR_UP     	P2OUT |= BIT4
#define AD9854_WR_DOWN  	P2OUT &=~BIT4

#define AD9854_CRL_SET  	P2DIR |= 0x3c	//控制端口设置
#define AD9854_CRL_OUT 		P2OUT

#define AD9854_UPDATE_UP   	P2OUT |= BIT3
#define AD9854_UPDATE_DOWN 	P2OUT &=~BIT3	//更新时钟

//#define AD9854_SDIO_UP     	P2OUT |= BIT1
//#define AD9854_SDIO_DOWN   	P2OUT &=~BIT1

//#define AD9854_IO_RESET_UP    P2OUT |= BIT2
//#define AD9854_IO_RESET_DOWN  P2OUT &=~BIT2

#define AD9854_RESET_UP    	P2OUT |= BIT2
#define AD9854_RESET_DOWN  	P2OUT &=~BIT2   //主复位

//#define AD9854_IO_parallel P2OUT |= BIT7
//#define AD9854_IO_serial   P2OUT &=~BIT7   //串并选择

#define AD9854_OSC_ON    	P2OUT |= BIT5
#define AD9854_OSC_OFF  	P2OUT &=~BIT5

////////////////////////////////////////////////////////////////////
//                                                                //
//                             INSTRUCTION BYTE                   //
//                                                                //
////////////////////////////////////////////////////////////////////

//******************************************************************
//寄存器地址 注意是并行地址
#define	  AD9854_Addr_PHA1       0x00        	// 2 Bytes
#define	  AD9854_Addr_PHA2       0x02        	// 2 Bytes 
#define	  AD9854_Addr_FRE1       0x04        	// 6 Bytes	
#define	  AD9854_Addr_FRE2       0x0A        	// 6 Bytes
#define	  AD9854_Addr_DELTA      0x10        	// 6 Bytes
#define	  AD9854_Addr_UPDATA     0x16        	// 4 Bytes
#define	  AD9854_Addr_RAMP_CLK   0x1A        	// 3 Bytes
#define	  AD9854_Addr_CTR_REG    0x1D        	// 4 Bytes
#define	  AD9854_Addr_I_MUL      0x21        	// 2 Bytes
#define	  AD9854_Addr_Q_MUL      0x23        	// 2 Bytes
#define	  AD9854_Addr_ShapeD     0x25       	// 1 Bytes
#define	  AD9854_Addr_Q_DAC      0x26        	// 2 Bytes
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
#define	  AD9854_Length_ShapeD     1       		// 1 Bytes
#define	  AD9854_Length_Q_DAC      2        	// 2 Bytes
//******************************************************************

//******************************************************************
//主时钟配置
//#define			CLK_Set	4	//4倍频，外置30MHz,得到120MHz
//extern ulong	Freq_Set_ulong = 2345625;
//extern double	Freq_Set_double = 2345624.805922133;

//#define			CLK_Set	10	//10倍频，外置30MHz,得到300MHz
//const ulong		Freq_Set_ulong = 938250;
//const double		Freq_Set_double = 938249.922368853;


////////////////////////////////////////////////////////////////////
//                                                                //
//                          VARIABLES   DEFINITION                //
//                                                                //
////////////////////////////////////////////////////////////////////


extern void AD9854_Parallel_Setup(void);	//并行总线端口设置
extern void AD9854_WR_Byte(uchar addr, uchar data);	//写入内部寄存器
extern void AD9854_Parallel_WriteBurstReg(uchar addr, uchar *buffer, uchar count);	//写入寄存器
extern void AD9854_Parallel_ReadBurstReg(uchar addr, uchar *buffer, uchar count);	//读出寄存器
extern void AD9854_Update(void);	//更新当前配置
extern void AD9854_Init(void);		//初始化AD9854

extern void AD9854_WR_Frq1(void);	//输出测试频率
extern void AD9854_WR_FrqSW(void);	//进行扫频程序

extern void AD9854_Freq_Convey(double Freq);	//频率转换函数
extern void AD9854_Freq_Convey1(ulong Freq);
extern void AD9854_double_Freq_Convey(double Freq);

extern void AD9854_SetSine(double Freq,uint Shape);		//正弦波产生
extern void AD9854_SetSine1(ulong Freq,uint Shape);
extern void AD9854_SetSine2(ulong Freq);
extern void AD9854_SetSine_double(double Freq,uint Shape);

extern void AD9854_InitRFSK(void);		//三角扫频初始化
extern void AD9854_SetRFSK(ulong Freq_Low,ulong Freq_High,ulong Freq_Up_Down,ulong FreRate,uint Shape);		//三角扫频设置






