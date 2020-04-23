#include"AD9854_Parallel.h"
#include  <msp430.h>
#include "general.h"

//******************************************************************

//*********************************
//ȫ��Ƶ�ʿ�����
//*********************************
uchar Freq_Word[6];
//��ʱ������
//#define		CLK_Set	0x04	//4��Ƶ������30MHz,�õ�120MHz
//const ulong	Freq_Set_ulong = 2345625;
//const double	Freq_Set_double = 2345624.805922133;

//#define		CLK_Set	0x45	//5��Ƶ������30MHz,�õ�150MHz
//const ulong	Freq_Set_ulong = 1876499;
//const double	Freq_Set_double = 1876499.844737707;

//#define		CLK_Set	0x46	//6��Ƶ������30MHz,�õ�180MHz
//const ulong	Freq_Set_ulong = 1563749;
//const double	Freq_Set_double = 1563749.870614756;

#define			CLK_Set	0x47	//7��Ƶ������30MHz,�õ�210MHz
const ulong		Freq_Set_ulong = 1340357;
const double	Freq_Set_double = 1340357.031955505;

//#define			CLK_Set	0x48	//8��Ƶ������30MHz,�õ�240MHz
//const ulong		Freq_Set_ulong = 1172812;
//const double	Freq_Set_double = 1172812.402961067;

//#define		CLK_Set	0x4a	//10��Ƶ������30MHz,�õ�300MHz
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
//��������AD9854_Parallel_Setup(void)
//���룺��
//�������
//�����������������߶˿�����
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
//��������void AD9854_WR_Byte(uchar addr, uchar data);
//���룺��ַ��д�뻺������д�����
//�������
//������������������д���üĴ���
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
//��������AD9854_Parallel_WriteBurstReg(uchar addr, uchar *buffer, uchar count)
//���룺��ַ��д�뻺������д�����
//�������
//������������������д���üĴ���
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
//��������void AD9854_Parallel_ReadBurstReg(uchar addr, uchar *buffer, uchar count)
//���룺��ַ���������ݺ��ݴ�Ļ��������������ø���
//�������
//�����������������������üĴ���
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
//��������:��������
//�������:��
//******************************************************************
void AD9854_Update(void)
{
	AD9854_UPDATE_UP;      //ԭ���϶��ǵ͵�ƽ,�����Ҿ������øߺ�
	_NOP() ;
	_NOP() ;
	_NOP() ;
	AD9854_UPDATE_DOWN;
}

//******************************************************************
//��������:AD9854��ʼ�� д���ƼĴ���
//�������:��
//Ӳ��˵����
//******************************************************************
void AD9854_Init(void)
{
	AD9854_Parallel_Setup();		//��ʼ������

	AD9854_WR_UP;
	AD9854_RD_UP;
	AD9854_UPDATE_DOWN;		//���͸���ʱ�ӣ���������

	AD9854_RESET_DOWN;
	AD9854_RESET_UP;	
	DelayMs(10);
	AD9854_RESET_DOWN;		//����λ	

	AD9854_WR_Byte(0x1d,0x00);		//д���ƼĴ���
	AD9854_WR_Byte(0x1e,CLK_Set);	//����ϵͳʱ�ӱ�ƵΪ7��Ƶ��ϵͳʱ��Ϊ210MHz
	AD9854_WR_Byte(0x1f,0x00);		//����ϵͳΪģʽ0�����ⲿ����
	AD9854_WR_Byte(0x20,0x60);		//����Ϊ�ɵ��ڷ��ȣ�ȡ����ֵ����

	AD9854_UPDATE_UP;			//��߸���ʱ�ӣ���������
	DelayMs(10);
	AD9854_UPDATE_DOWN;		//���͸���ʱ�ӣ������޸�
}
//******************************************************************
//�������ܣ������ź�Ƶ������ת��
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
//�������ܣ������ź�Ƶ������ת��
//˵������Ҫת����Ƶ��ֵ
//	 ϵͳʱ��Ϊ300MHz
//	���㷨λ���ֽ�����㷨���й�ʽFTW = (Desired Output Frequency �� 2N)/SYSCLK
// �õ����㷨������N=48��Desired Output Frequency Ϊ����Ҫ��Ƶ�ʣ���Freq��SYSCLK
// Ϊ�ɱ�̵�ϵͳʱ�ӣ�FTWΪ48Bit��Ƶ�ʿ����֣���Freq_Word[6]
//******************************************************************
void AD9854_Freq_Convey1(ulong Freq)
{
	ulong FreqBuf;
	ulong Temp = Freq_Set_ulong;

	uchar Array_Freq[4];	//������Ƶ�����ӷ�Ϊ�ĸ��ֽ�
	Array_Freq[0] = (uchar)Freq;	//���λ��λ
	Array_Freq[1] = (uchar)(Freq>>8);
	Array_Freq[2] = (uchar)(Freq>>16);
	Array_Freq[3] = (uchar)(Freq>>24);	//�����λ

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
//��������:void Freq_doublt_convert(double Freq)
//��������:�����ź�Ƶ������ת��
//��ڲ���:Freq   ��Ҫת����Ƶ�ʣ�ȡֵ��0~SYSCLK/2
//���ڲ���:��   ����Ӱ��ȫ�ֱ���Freq_Word[6]��ֵ
//˵����   �й�ʽFTW = (Desired Output Frequency �� 2N)/SYSCLK�õ��ú�����
//         ����N=48��Desired Output Frequency Ϊ����Ҫ��Ƶ�ʣ���Freq��SYSCLK
//         Ϊ�ɱ�̵�ϵͳʱ�ӣ�FTWΪ48Bit��Ƶ�ʿ����֣���Freq_Word[6]
//ע�⣺   �ú��������溯��������Ϊ�ú�������ڲ���Ϊdouble����ʹ�źŵ�Ƶ�ʸ���ȷ
//         ���꽨����100HZ�����ñ��������ڸ���100HZ��������ú���void Freq_convert(long Freq)
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
//�������ܣ������źŲ�������
//******************************************************************
void AD9854_SetSine(double Freq,uint Shape)
{
	AD9854_Freq_Convey(Freq);

	AD9854_Parallel_WriteBurstReg(AD9854_Addr_FRE1,	//I·Ƶ������
			Freq_Word, AD9854_Length_FRE1);
	//AD9854_Parallel_WriteBurstReg(AD9854_Addr_FRE2,	//Q·Ƶ������
	//		Freq_Word, AD9854_Length_FRE2);

	AD9854_WR_Byte(AD9854_Addr_I_MUL, Shape>>8);
	AD9854_WR_Byte(AD9854_Addr_I_MUL + 1, (uchar)(Shape&0xff));

	//AD9854_WR_Byte(AD9854_Addr_Q_MUL, Shape>>8);
	//AD9854_WR_Byte(AD9854_Addr_Q_MUL + 1, (uchar)(Shape&0xff));

	AD9854_Update();
}
//******************************************************************
//�������ܣ������źŲ�������
//˵����Freq   Ƶ�����ã�ȡֵ��ΧΪ0~(1/2)*SYSCLK
//    Shape  ��������. Ϊ12 Bit,ȡֵ��ΧΪ(0~4095) ,ȡֵԽ��,����Խ��
//******************************************************************
void AD9854_SetSine1(ulong Freq,uint Shape)
{
	uchar count = 0;
	uchar Adress1 = AD9854_Addr_FRE1;
	//uchar Adress2 = AD9854_Addr_FRE2;

	AD9854_Freq_Convey1(Freq);        //Ƶ��ת��

	for(count = 6;count > 0;)	          //д��6�ֽڵ�Ƶ�ʿ�����
	{
		AD9854_WR_Byte(Adress1 ++, Freq_Word[--count]);
    }

	//for(count=6;count>0;)	          //д��6�ֽڵ�Ƶ�ʿ�����
	//{
	//	AD9854_WR_Byte(Adress2 ++,Freq_Word[--count]);
	//}

	AD9854_WR_Byte(0x21,Shape >> 8);	  //����Iͨ������
	AD9854_WR_Byte(0x22,(uchar)(Shape & 0xff));

	//AD9854_WR_Byte(0x23,Shape>>8);	  //����Qͨ������
	//AD9854_WR_Byte(0x24,(uchar)(Shape&0xff));

	AD9854_Update();	 			  //����AD9854���
}

void AD9854_SetSine2(ulong Freq)
{
	uchar count = 0;
	uchar Adress1 = AD9854_Addr_FRE1;
	//uchar Adress2 = AD9854_Addr_FRE2;

	AD9854_Freq_Convey1(Freq);        //Ƶ��ת��

	for(count = 6;count > 0;)	          //д��6�ֽڵ�Ƶ�ʿ�����
	{
		AD9854_WR_Byte(Adress1 ++, Freq_Word[--count]);
    }
}

//******************************************************************
//��������:void AD9854_SetSine_double(double Freq,uint Shape)
//��������:AD9854���Ҳ���������
//��ڲ���:Freq   Ƶ�����ã�ȡֵ��ΧΪ0~1/2*SYSCLK
//         Shape  ��������. Ϊ12 Bit,ȡֵ��ΧΪ(0~4095)
//���ڲ���:��
//******************************************************************
void AD9854_SetSine_double(double Freq,uint Shape)
{
	uchar count = 0;
	uchar Adress1 = AD9854_Addr_FRE1;
	//uchar Adress2 = AD9854_Addr_FRE2;

	AD9854_double_Freq_Convey(Freq);		           //Ƶ��ת��

	for(count=6;count>0;)	                    //д��6�ֽڵ�Ƶ�ʿ�����
    {
		AD9854_WR_Byte(Adress1 ++,Freq_Word[--count]);
    }

	//for(count=6;count>0;)	          //д��6�ֽڵ�Ƶ�ʿ�����
	//{
	//	AD9854_WR_Byte(Adress2 ++,Freq_Word[--count]);
	//}

	AD9854_WR_Byte(0x21,Shape >> 8);	  //����Iͨ������
	AD9854_WR_Byte(0x22,(uchar)(Shape & 0xff));

	//AD9854_WR_Byte(0x23,Shape>>8);	  //����Qͨ������
	//AD9854_WR_Byte(0x24,(uchar)(Shape&0xff));

	AD9854_Update();	 			  //����AD9854���
}

//******************************************************************
//�������ܣ�д Frequency Tuning Word #1
//�������: frq1(ָ��,ָ��unsigned char��������׵�ַ,���鳤��Ϊ48,6���ֽ�)
//Serial Register Address :2
//д���ַ�ֽ���:1 Byte
//д�������ֽ���:6 Bytes 
//******************************************************************
void AD9854_WR_Frq1(void)
{ 	
	AD9854_Parallel_WriteBurstReg(AD9854_Addr_FRE1,		//дƵ�ʿ��ƼĴ���1
						AD9854_Reg_FRE1, AD9854_Length_FRE1);
	AD9854_Update();
}

//******************************************************************
//�������ܣ���������ɨƵ
//�������:
//Serial Register Address :2��3��4��6
//д���ַ�ֽ���:1 Byte
//д�������ֽ���:6 Bytes 
//******************************************************************
void AD9854_WR_FrqSW(void)
{ 	
	AD9854_Parallel_WriteBurstReg(AD9854_Addr_FRE1,		//дƵ�ʿ��ƼĴ���1
						AD9854_Reg_FRE1, AD9854_Length_FRE1);
		
	AD9854_Parallel_WriteBurstReg(AD9854_Addr_FRE2,		//дƵ�ʿ��ƼĴ���2
						AD9854_Reg_FRE2, AD9854_Length_FRE2);
	
	AD9854_Parallel_WriteBurstReg(AD9854_Addr_DELTA,		//дƵ�������Ĵ���
						AD9854_Reg_DELTA, AD9854_Length_DELTA);
	
	AD9854_Parallel_WriteBurstReg(AD9854_Addr_RAMP_CLK,	//дɨƵʱ�ӼĴ���
						AD9854_Reg_RAMP_CLK, AD9854_Length_RAMP_CLK);
	AD9854_Update();
}
//====================================================================================
//��������:void AD9854_InitRFSK(void)
//��������:AD9854��RFSK��ʼ��
//��ڲ���:��
//���ڲ���:��
//====================================================================================
void AD9854_InitRFSK(void)
{
	AD9854_WR_UP;                      //������д���ƶ˿���Ϊ��Ч
    AD9854_RD_UP;
    AD9854_UPDATE_DOWN;
    AD9854_RESET_UP;                     //��λAD9854
    AD9854_RESET_DOWN;

	AD9854_WR_Byte(0x1d,0x00);	       //�رձȽ���
	AD9854_WR_Byte(0x1e,CLK_Set);	   //����ϵͳʱ�ӱ�Ƶ
	AD9854_WR_Byte(0x1f,0x24);	        //����ϵͳΪģʽ2�����ⲿ����,ʹ�����ǲ�ɨƵ����
	AD9854_WR_Byte(0x20,0x60);	        //����Ϊ�ɵ��ڷ��ȣ�ȡ����ֵ����

	AD9854_Update();
}

//====================================================================================
//��������:void AD9854_SetRFSK(void)
//��������:AD9854��RFSK����
//��ڲ���:Freq_Low          RFSK��Ƶ��	   48Bit
//         Freq_High         RFSK��Ƶ��	   48Bit
//         Freq_Up_Down		 ����Ƶ��	   48Bit
//		   FreRate           б��ʱ�ӿ���  20Bit
//���ڲ���:��
//ע��     ÿ��������֮���ʱ����������ʽ��ʾ��FreRate +1��*��System Clock ����һ������,
//         Ƶ�� ���������½� һ������Ƶ��
//====================================================================================
void AD9854_SetRFSK(ulong Freq_Low,ulong Freq_High,ulong Freq_Up_Down,ulong FreRate,uint Shape)
{
	uchar count=6;
	uchar Adress1,Adress2,Adress3;
    //const uint  Shape=4000;			   //��������. Ϊ12 Bit,ȡֵ��ΧΪ(0~4095)

	Adress1=0x04;		     //ѡ��Ƶ�ʿ����ֵ�ַ�ĳ�ֵ
	Adress2=0x0a;
	Adress3=0x10;

	AD9854_Freq_Convey1(Freq_Low);                   //Ƶ��1ת��
	for(count=6;count>0;)	                         //д��6�ֽڵ�Ƶ�ʿ�����
    {
		AD9854_WR_Byte(Adress1++,Freq_Word[--count]);
    }

	AD9854_Freq_Convey1(Freq_High);                         //Ƶ��2ת��
	for(count=6;count>0;)	                         //д��6�ֽڵ�Ƶ�ʿ�����
    {
		AD9854_WR_Byte(Adress2++,Freq_Word[--count]);
    }

	AD9854_double_Freq_Convey(Freq_Up_Down);               //����Ƶ��ת��
	for(count=6;count>0;)	                         //д��6�ֽڵ�Ƶ�ʿ�����
    {
		AD9854_WR_Byte(Adress3++,Freq_Word[--count]);
    }

	AD9854_WR_Byte(0x1a,(uchar)((FreRate>>16)&0x0f));	//����б������
	AD9854_WR_Byte(0x1b,(uchar)(FreRate>>8));
	AD9854_WR_Byte(0x1c,(uchar)FreRate);

	AD9854_WR_Byte(0x21,Shape>>8);	                  	//����Iͨ������
	AD9854_WR_Byte(0x22,(uchar)(Shape&0xff));
	AD9854_WR_Byte(0x23,Shape>>8);	                  //����Qͨ������
	AD9854_WR_Byte(0x24,(uchar)(Shape&0xff));

	AD9854_Update();
}
