#include "msp430f5529.h"

//190206
//按键亮灯

//中断亮灯

//定时器亮灯

//频率设置

unsigned int I;

void IO_SET();
void CLK_SET();
void TIMER_A_SET();

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;
	
	//初始设置
	IO_SET();
	CLK_SET();
	TIMER_A_SET();
	
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

void CLK_SET()
{
	//SMCLK频率设置：32768*2*352 ~23Mhz
	//MCLK频率设置：32768*2*352 ~23Mhz
	
	//清空DCOx和MODx
	UCSCTL0 = 0;
	//SMCLK和MCLK选择DCOCLK
	UCSCTL4_L = SELS_3 +SELM_3;
	//禁止FLL
	__bis_SR_register(SCG0);
	//DCO频率范围6~54Mhz
	UCSCTL1 = DCORSEL_5;
	//FLL预先2分频
	//UCSCTL2 = 0x2000;
	//倍频系数
	UCSCTL2 |= 352-1;
	//使能FLL
	__bic_SR_register(SCG0);
	//延时等待稳定
	__delay_cycles(250000);
	
}

void TIMER_A_SET()
{
	//SMCLK，增计数，清除TAR
	TA0CCTL0 = CCIE;
	TA0CCR0 = 0xffff;
	TA0CTL = TASSEL_2 + MC_1 + TACLR;
	
	
}


#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER_A(void)
{
	//TIMER_A中断
	I++;
	if(I == 176)
	{
		I = 0;
		P1OUT ^= BIT0;
	}
	
	
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

