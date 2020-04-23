#include <msp430.h>
#include <stdio.h>
#include <math.h>

unsigned char* Float_to_Char(float d,unsigned int eps)
{
	unsigned char str[20];
    unsigned char ch[20];

    int high;		//float_��������
    double low;		//float_С������
    int n,i;

    high = (int)d;
    low = d - high;

    n = 0;
    if(high == 0)
    {
    	ch[n] = '0';
    	n++;
    }
    while(high > 0)		//����������ת��Ϊ�ַ���
    {
    	ch[n] = '0' + high % 10;
        high = high / 10;
        n++;
    }
    n = n-1;	//n����
    for(i = n;i >= 0;i--)	//���������ִ����ַ�����
    {
        str[n-i]=ch[i];
    }
    n = n - i;
    str[n] = '.';		//��С�����������֮��

    for(i=0;i < eps;i++)	//��С��λ�����ַ�����
    {
    	low *= 10;
    	n++;
    	str[n] = '0' + (int)low;
    	low -= (int)low;
    }
    if((int)(low*10) >= 5)
    	str[n] += 1;

    n++;	//��ĩβ�����ַ���������
    str[n] = '\0';

    return str;
}

unsigned char* Double_to_Char(double d,unsigned char eps)
{
	unsigned char str[20];
	unsigned char ch[20];

    long high;		//double_��������
    double low;		//double_С������
    int n,i;

    high = (long)d;
    low = d - high;

    n = 0;
    if(high == 0)
    {
    	ch[n] = '0';
    	n++;
    }
    while(high > 0)		//����������ת��Ϊ�ַ���
    {
    	ch[n] = '0' + high % 10;
        high = high / 10;
        n++;
    }
    n = n-1;	//n����
    for(i = n;i >= 0;i--)	//���������ִ����ַ�����
    {
        str[n-i]=ch[i];
    }
    n = n - i;
    str[n] = '.';		//��С�����������֮��

    for(i=0;i < eps;i++)	//��С��λ�����ַ�����
    {
    	low *= 10;
    	n++;
    	str[n] = '0' + (long)low;
    	low -= (long)low;
    }
    if((long)(low*10) >= 5)
    	str[n] += 1;

    n++;	//��ĩβ�����ַ���������
    str[n] = '\0';

    return str;
}

unsigned char* Freq_kHz_to_Char(unsigned long d)
{
	unsigned char str[20];
    unsigned char ch[20];
    int i,n;
    unsigned long high,low;

    high = d/1000;
    low = d - high*1000;

    n = 0;
    if(high == 0)
    {
    	ch[n] = '0';
    	n++;
    }
    while(high > 0)		//����������ת��Ϊ�ַ���
    {
    	ch[n] = '0' + high % 10;
        high = high / 10;
        n++;
    }
    n = n-1;	//n����
    for(i = n;i >= 0;i--)	//���������ִ����ַ�����
    {
        str[n-i]=ch[i];
    }
    n = n - i;
    str[n] = '.';		//��С�����������֮��

    for(i=0;i<3;i++)
    {
    	str[n+3-i] = low%10 + '0';
    	low /= 10;
    }
    str[n+4] = '\0';

    return str;
}

unsigned char* Freq_MHz_to_Char(unsigned long d)
{
	unsigned char str[20];
    unsigned char ch[20];
    int i,n;
    unsigned long high,low;

    high = d/1000000;
    low = (d - high*1000000)/1000;

    n = 0;
    if(high == 0)
    {
    	ch[n] = '0';
    	n++;
    }
    while(high > 0)		//����������ת��Ϊ�ַ���
    {
    	ch[n] = '0' + high % 10;
        high = high / 10;
        n++;
    }
    n = n-1;	//n����
    for(i = n;i >= 0;i--)	//���������ִ����ַ�����
    {
        str[n-i]=ch[i];
    }
    n = n - i;
    str[n] = '.';		//��С�����������֮��

    for(i=0;i<3;i++)
    {
    	str[n+3-i] = low%10 + '0';
    	low /= 10;
    }
    str[n+4] = '\0';

    return str;
}

int shape_trans(unsigned long freq,int shape)
{
	unsigned int shape_temp;
	double temp;

	temp = 460.4*sin(freq/1000000.0*0.08865 - 0.118);
	shape_temp = shape + temp;

	return shape_temp;
}

int shape_ctrl(unsigned long freq)
{
	unsigned int shape_temp;
	double temp;
	//temp = freq/1000000.0*6.464 + 722.2;
	//shape_temp = temp;

	temp = 1007*sin(freq/1000000.0*0.01848 + 0.8385);
	shape_temp = temp;

	return shape_temp;
}

int pack_trans(unsigned long pack)
{
	unsigned int pack_temp;

	//if(pack>90000)
		//pack_temp = 5 * (double)pack/1000.0 + 845;
	//else
		//pack_temp = 479.4 * pow((double)pack/1000.0,0.219);

	pack_temp = 455.4 * pow((double)pack/1000.0,0.1964);

	return pack_temp;
}
