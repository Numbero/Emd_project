#include"key.h"

unsigned char key_pressed;//按键是否按下标志位
unsigned char key_val;//按键值存储变量
unsigned char key_flag;//按键事件标志


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
函数名称: key_init()
功    能: 键盘相关端口变量初始化
参    数: 无
返回值  : 无
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
函数名称: check_key
功    能: 获得键值
参    数: 无
返回值  : 如果有键按下，返回相应键值，如果没有，返回0xFF；
********************************************/
void check_key(void)
{
	unsigned char col,tmp2;
	int row;
	for(row=0;row<4;row++)
	{
		SCAN(row);//扫描按键
		if ((SCAN_IN() & 0x0f) < 0x0f)
		{ 
			tmp2 = 0x01; // tmp2用于检测出那一位为0
			for(col =0;col<4;col++)// 列检测
			{ 
				if((SCAN_IN() & tmp2)==0x00)// 是否是该列
				{ 
					key_val = key_change(row + col*4);  // 获取键值 退出循环
				}
				tmp2 *= 2; // tmp2左移一位
			}
                        break;
		}
	}
}
/*******************************************
函数名称: key_event
功    能: 检测是否有键按下
参    数: 无
返回值  : 无
********************************************/
void key_event(void)
{
	unsigned char tmp;
        SCAN(4);
	tmp = SCAN_IN(); 
	if ((key_pressed == 0x00)&&((tmp & 0x0f) < 0x0f))//是否有键按下
	{ 
		key_pressed = 1; // 如果有按键按下，设置key_pressed标识
		_delay_ms(10); //消除抖动
		check_key(); // 调用check_key(),获取键值
	}
	else if ((key_pressed ==1)&&((tmp & 0x0f) == 0x0f))//是否按键已经释放
	{ 
		key_pressed = 0; // 清除key_pressed标识
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
