#include"key.h"

unsigned char key_pressed;//按键是否按下标志位
unsigned char key_val;//按键值存储变量
unsigned char key_flag;//按键事件标志
/*******************************************
函数名称: key_init()
功    能: 键盘相关端口变量初始化
参    数: 无
返回值  : 无
********************************************/
void key_init()
{
	 P4DIR = 0x0F;    //键盘扫描初始化
 	 P4REN = 0xF0;      //输入上下拉电阻使能，输出上下拉不使能
 	 P4OUT = 0xF0;     //输入上拉
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
	unsigned char row ,col,tmp1,tmp2;
	// tmp1用来设置P4OUT的值，使P4.0~P4.3中有一个为0
	tmp1 = 0x01;
	for(row=0;row<4;row++)
	{
		P4OUT |= 0x0F; // p4.0~P4.3=1
		P4OUT -= tmp1; // P4.0~p4.3中有一个为0
		tmp1 *= 2; // tmp1 左移一位
		if ((P4IN & 0xF0) < 0xF0)// 是否P4IN的P4.4~P4.7中有一位为0
		{ 
			tmp2 = 0x10; // tmp2用于检测出那一位为0
			for(col =0;col<4;col++)// 列检测
			{ 
				if((P4IN & tmp2)==0x00)// 是否是该列
				{ 
					key_val = row + col*4;  // 获取键值 退出循环
				}
				tmp2 *= 2; // tmp2左移一位
			}
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
	P4OUT &= 0xF0; // 设置P4OUT输出值 使得如果有键按下 P4IN将不为全高
	tmp = P4IN; // 获取P4IN
	if ((key_pressed == 0x00)&&((tmp & 0xF0) < 0xF0))//是否有键按下
	{ 
		key_pressed = 1; // 如果有按键按下，设置key_pressed标识
		_delay_ms(10); //消除抖动
		check_key(); // 调用check_key(),获取键值
	}
	else if ((key_pressed ==1)&&((tmp & 0xF0) == 0xF0))//是否按键已经释放
	{ 
		key_pressed = 0; // 清除key_pressed标识
		key_flag = 1;
	}
}

