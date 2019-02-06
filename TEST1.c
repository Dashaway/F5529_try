#include "msp430f5529.h"

//190206
//按键亮灯

//中断亮灯


void IO_SET();

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;
	//端口设置
	IO_SET();
	
	P1OUT |= BIT0;
	
	//使能中断
	_EINT();
	//CPU休眠
	LPM0;	
}

void IO_SET()
{
	//输出
	P1DIR |= BIT0;
	
	//输入，使能上拉电阻
	P1DIR &= ~BIT1;
	P1REN |= BIT1;
	P1OUT |= BIT1;
	//使能下降沿中断
	P1IES |= BIT1;
	P1IFG &= ~BIT1;
	P1IE |= BIT1;
	
	//输出
	P4DIR |= BIT7;

}


#pragma vector = PORT1_VECTOR
__interrupt void P1(void)
{
	//端口1中断
	if(P1IFG & BIT1)
	{
		//翻转端口
		P4OUT ^= BIT7;	
	}
	
	//清除中断
	P1IFG = 0;
}

