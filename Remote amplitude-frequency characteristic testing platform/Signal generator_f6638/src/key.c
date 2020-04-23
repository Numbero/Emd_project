#include"key.h"

unsigned char key_pressed;//�����Ƿ��±�־λ
unsigned char key_val;//����ֵ�洢����
unsigned char key_flag;//�����¼���־
/*******************************************
��������: key_init()
��    ��: ������ض˿ڱ�����ʼ��
��    ��: ��
����ֵ  : ��
********************************************/
void key_init()
{
	 P4DIR = 0x0F;    //����ɨ���ʼ��
 	 P4REN = 0xF0;      //��������������ʹ�ܣ������������ʹ��
 	 P4OUT = 0xF0;     //��������
	 key_pressed = 0;
	 key_val = 255;
	 key_flag = 0;
}
/*******************************************
��������: check_key
��    ��: ��ü�ֵ
��    ��: ��
����ֵ  : ����м����£�������Ӧ��ֵ�����û�У�����0xFF��
********************************************/
void check_key(void)
{
	unsigned char row ,col,tmp1,tmp2;
	// tmp1��������P4OUT��ֵ��ʹP4.0~P4.3����һ��Ϊ0
	tmp1 = 0x01;
	for(row=0;row<4;row++)
	{
		P4OUT |= 0x0F; // p4.0~P4.3=1
		P4OUT -= tmp1; // P4.0~p4.3����һ��Ϊ0
		tmp1 *= 2; // tmp1 ����һλ
		if ((P4IN & 0xF0) < 0xF0)// �Ƿ�P4IN��P4.4~P4.7����һλΪ0
		{ 
			tmp2 = 0x10; // tmp2���ڼ�����һλΪ0
			for(col =0;col<4;col++)// �м��
			{ 
				if((P4IN & tmp2)==0x00)// �Ƿ��Ǹ���
				{ 
					key_val = row + col*4;  // ��ȡ��ֵ �˳�ѭ��
				}
				tmp2 *= 2; // tmp2����һλ
			}
		}
	}
}
/*******************************************
��������: key_event
��    ��: ����Ƿ��м�����
��    ��: ��
����ֵ  : ��
********************************************/
void key_event(void)
{
	unsigned char tmp;
	P4OUT &= 0xF0; // ����P4OUT���ֵ ʹ������м����� P4IN����Ϊȫ��
	tmp = P4IN; // ��ȡP4IN
	if ((key_pressed == 0x00)&&((tmp & 0xF0) < 0xF0))//�Ƿ��м�����
	{ 
		key_pressed = 1; // ����а������£�����key_pressed��ʶ
		_delay_ms(10); //��������
		check_key(); // ����check_key(),��ȡ��ֵ
	}
	else if ((key_pressed ==1)&&((tmp & 0xF0) == 0xF0))//�Ƿ񰴼��Ѿ��ͷ�
	{ 
		key_pressed = 0; // ���key_pressed��ʶ
		key_flag = 1;
	}
}

