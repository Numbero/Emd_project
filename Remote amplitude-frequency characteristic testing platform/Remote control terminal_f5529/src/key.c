#include"key.h"

unsigned char key_pressed;//�����Ƿ��±�־λ
unsigned char key_val;//����ֵ�洢����
unsigned char key_flag;//�����¼���־


unsigned char key_change(unsigned char data)
{
	unsigned char temp;
	switch(data)
	{
	case 7:
		temp = 0;
		break;
	case 11:
		temp = 1;
		break;
	case 15:
		temp = 2;
		break;
	case 0:
		temp = 3;
		break;
	case 2:
		temp = 4;
		break;
	case 6:
		temp = 5;
		break;
	case 10:
		temp = 6;
		break;
	case 4:
		temp = 7;
		break;
	case 14:
		temp = 8;
		break;
	case 1:
		temp = 9;
		break;
	case 5:
		temp = 10;
		break;
	case 8:
		temp = 11;
		break;
	case 9:
		temp = 12;
		break;
	case 3:
		temp = 13;
		break;
	case 13:
		temp = 14;
		break;
	case 12:
		temp = 15;
		break;
	default:break;
	}
	return temp;
}
/*******************************************
��������: key_init()
��    ��: ������ض˿ڱ�����ʼ��
��    ��: ��
����ֵ  : ��
********************************************/
void key_init()
{
	 P1DIR &= ~BIT2;
	 P1DIR &= ~BIT3;
	 P1DIR &= ~BIT4;
	 P1DIR &= ~BIT5;

	 P1REN |= BIT2 + BIT3+ BIT4 + BIT5;
	 P1OUT |= BIT2 + BIT3+ BIT4 + BIT5;

         
	 P1DIR |= BIT6;
	 P1OUT &= ~BIT6;
	 P2DIR |= BIT0 + BIT2 + BIT3;
	 P2OUT &= ~(BIT0 + BIT2 + BIT3);
         
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
	unsigned char col,tmp2;
	int row;
	for(row=0;row<4;row++)
	{
		SCAN(row);//ɨ�谴��
		if ((SCAN_IN() & 0x0f) < 0x0f)
		{ 
			tmp2 = 0x01; // tmp2���ڼ�����һλΪ0
			for(col =0;col<4;col++)// �м��
			{ 
				if((SCAN_IN() & tmp2)==0x00)// �Ƿ��Ǹ���
				{ 
					key_val = key_change(row + col*4);  // ��ȡ��ֵ �˳�ѭ��
				}
				tmp2 *= 2; // tmp2����һλ
			}
                        break;
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
        SCAN(4);
	tmp = SCAN_IN(); 
	if ((key_pressed == 0x00)&&((tmp & 0x0f) < 0x0f))//�Ƿ��м�����
	{ 
		key_pressed = 1; // ����а������£�����key_pressed��ʶ
		_delay_ms(10); //��������
		check_key(); // ����check_key(),��ȡ��ֵ
	}
	else if ((key_pressed ==1)&&((tmp & 0x0f) == 0x0f))//�Ƿ񰴼��Ѿ��ͷ�
	{ 
		key_pressed = 0; // ���key_pressed��ʶ
		key_flag = 1;
	}
}

void SCAN(int row){
  switch(row){
  case 0:
	  P1OUT &= ~BIT6;
	  P2OUT |= BIT0;
	  P2OUT |= BIT2;
	  P2OUT |= BIT3;
  break;
  case 1:
	  P1OUT |= BIT6;
	  P2OUT &= ~BIT0;
	  P2OUT |= BIT2;
	  P2OUT |= BIT3;
  break;
  case 2:
	  P1OUT |= BIT6;
	  P2OUT |= BIT0;
	  P2OUT &= ~BIT2;
	  P2OUT |= BIT3;
  break;
  case 3:
	  P1OUT |= BIT6;
	  P2OUT |= BIT0;
	  P2OUT |= BIT2;
	  P2OUT &= ~BIT3;
  break;
  case 4:
	  P1OUT &= ~BIT6;
	  P2OUT &= ~BIT0;
	  P2OUT &= ~BIT2;
	  P2OUT &= ~BIT3;
  break;
  default:break;
}
}

char SCAN_IN(){
  char temp = P1IN>>2;
  return temp;
}
