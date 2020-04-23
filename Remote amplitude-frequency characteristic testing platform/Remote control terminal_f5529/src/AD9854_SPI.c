#include"AD9854_SPI.h"
#include  <msp430.h>
#include "general.h"

//******************************************************************************

//*********************************
//ȫ��Ƶ�ʿ�����
//*********************************
uchar Freq_Word[6];
uchar Shape_Word[2];

#define			CLK_Set	0x47	//7��Ƶ������30MHz,�õ�210MHz
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
unsigned char AD9854_Reg_CTR_REG[4]	={	0x00,       //д���ƼĴ���
										CLK_Set,   	//����ϵͳʱ�ӱ�ƵΪ7��Ƶ��ϵͳʱ��Ϊ210MHz
			   							0x00,       //����ϵͳΪģʽ0�����ⲿ����
										0x60};     	//����Ϊ�ɵ��ڷ��ȣ�ȡ����ֵ����
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
//��������SpisendByte(uchar dat)
//���룺���͵�����
//�������
//����������SPI����һ���ֽ�
//******************************************************************************
uchar SpiTxRxByte(uchar dat)
{
//----------------------������ģ��SPIʱ��ʽ-----------------------------------
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
	if(AD9854_SDO_IN)temp++;  //��ȡMISO״̬
		AD9854_SCLK_DOWN;
	}
	return temp;
}

//******************************************************************************
//��������AD9854_SPIWriteBurstReg(uchar addr, uchar *buffer, uchar count)
//���룺��ַ��д�뻺������д�����
//�������
//����������SPI����д���üĴ���
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
//��������void AD9854_SPIReadBurstReg(uchar addr, uchar *buffer, uchar count)
//���룺��ַ���������ݺ��ݴ�Ļ��������������ø���
//�������
//����������SPI���������üĴ���
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

//��������:��λ
//�������:��
//******************************************************************
void AD9854_Io_Reset(void)
{
	unsigned i=100 ;        //
	AD9854_IO_RESET_UP ;
	while(i--) ;
	AD9854_IO_RESET_DOWN ;
}
//******************************************************************

//��������:AD9854��ʼ�� д���ƼĴ���
//�������:��
//Ӳ��˵����
//******************************************************************
void AD9854_Init(void)
{
	P6DIR |= 0x3f;
	P6DIR &= ~BIT6;       //��ʼ���ܽ�����

	AD9854_CS_DOWN ;        //Ƭѡ	
	AD9854_UPDATE_DOWN;		//���͸���ʱ�ӣ���������

	AD9854_RESET_DOWN;
	AD9854_RESET_UP;	
	DelayMs(10);
	AD9854_RESET_DOWN;		//����λ
	AD9854_Io_Reset();		//IO��λ

	AD9854_SPIWriteBurstReg(AD9854_Addr_CTR_REG,		//д���ƼĴ���
						AD9854_Reg_CTR_REG, AD9854_Length_CTR_REG);			

	AD9854_UPDATE_UP;		//��߸���ʱ�ӣ���������
	DelayMs(10);
	AD9854_UPDATE_DOWN;		//���ø���ʱ��Ϊ���
}
//******************************************************************
//����ת������
//******************************************************************
void AD9854_Shape_Convey(uint Shape)
{
	Shape_Word[0] = (uchar)(Shape>>8);	//���λ8λ
	Shape_Word[1] = (uchar)(Shape&0xff);
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
//˵����Freq   Ƶ�����ã�ȡֵ��ΧΪ0~(1/2)*SYSCLK
//    Shape  ��������. Ϊ12 Bit,ȡֵ��ΧΪ(0~4095) ,ȡֵԽ��,����Խ��
//******************************************************************
void AD9854_SetSine1(ulong Freq,uint Shape)
{
	AD9854_Freq_Convey1(Freq);        //Ƶ��ת��
	AD9854_SPIWriteBurstReg(AD9854_Addr_FRE1,		//����Iͨ��Ƶ��
								Freq_Word, AD9854_Length_FRE1);

	AD9854_Shape_Convey(Shape);				   		   //����ת��
	AD9854_SPIWriteBurstReg(AD9854_Addr_I_MUL,		//����Iͨ������
								Shape_Word, AD9854_Length_I_MUL);

	AD9854_Update();	 			  //����AD9854���
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

	AD9854_double_Freq_Convey(Freq);		           //Ƶ��ת��
	AD9854_SPIWriteBurstReg(AD9854_Addr_FRE1,		//����Iͨ��Ƶ��
								Freq_Word, AD9854_Length_FRE1);

	AD9854_Shape_Convey(Shape);				   		   //����ת��
	AD9854_SPIWriteBurstReg(AD9854_Addr_I_MUL,		//����Iͨ������
								Shape_Word, AD9854_Length_I_MUL);

	AD9854_Update();	 			  //����AD9854���
}

//******************************************************************
//�������ܣ���������ɨƵ
//�������:
//Serial Register Address :2��3��4��6
//д���ַ�ֽ���:1 Byte
//д�������ֽ���:6 Bytes 
//******************************************************************
void Write_AD9854_FrqSW(void)
{ 	
	AD9854_SPIWriteBurstReg(AD9854_Addr_FRE1,		//дƵ�ʿ��ƼĴ���1
						AD9854_Reg_FRE1, AD9854_Length_FRE1);
		  AD9854_Io_Reset();
	AD9854_SPIWriteBurstReg(AD9854_Addr_FRE2,		//дƵ�ʿ��ƼĴ���2
						AD9854_Reg_FRE2, AD9854_Length_FRE2);
		  AD9854_Io_Reset();
	AD9854_SPIWriteBurstReg(AD9854_Addr_DELTA,		//дƵ�������Ĵ���
						AD9854_Reg_DELTA, AD9854_Length_DELTA);
		  AD9854_Io_Reset();
	AD9854_SPIWriteBurstReg(AD9854_Addr_RAMP_CLK,	//дɨƵʱ�ӼĴ���
						AD9854_Reg_RAMP_CLK, AD9854_Length_RAMP_CLK);

	AD9854_Update();	 			  //����AD9854���
}
