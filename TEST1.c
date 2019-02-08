#include "msp430f5529.h"


//190206
//按键亮灯

//中断亮灯

//定时器亮灯

//频率设置

unsigned int I;

void IO_SET();
void SetVcoreUp(unsigned int level);
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


void SetVcoreUp(unsigned int level)
{
	//内核电压调整
	
	// Open PMM registers for write
	PMMCTL0_H = PMMPW_H;              
	// Set SVS/SVM high side new level
	SVSMHCTL = SVSHE + SVSHRVL0*level + SVMHE + SVSMHRRL0*level;
	// Set SVM low side to new level
	SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0*level;
	// Wait till SVM is settled
	while ((PMMIFG & SVSMLDLYIFG) == 0);
	// Clear already set flags
	PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
	// Set VCore to new level
	PMMCTL0_L = PMMCOREV0*level;
	// Wait till new level reached
	if((PMMIFG & SVMLIFG))
		while((PMMIFG & SVMLVLRIFG) == 0);
	// Set SVS/SVM low side to new level
	SVSMLCTL = SVSLE + SVSLRVL0*level + SVMLE + SVSMLRRL0*level;
	// Lock PMM registers for write access
	PMMCTL0_H = 0x00;
}


void CLK_SET()
{
	//SMCLK频率设置：32768*2*770 ~50Mhz
	//MCLK频率设置：32768*2*770 ~50Mhz
	
	SetVcoreUp(0x03);
	
	//清空DCOx和MODx
	UCSCTL0 = 0;
	//SMCLK和MCLK选择DCOCLK
	UCSCTL4_L = SELS_3 +SELM_3;
	//禁止FLL
	__bis_SR_register(SCG0);
	//DCO频率范围6~54Mhz
	UCSCTL1 = DCORSEL_6;
	//FLL预先2分频
	//倍频系数
	UCSCTL2 = FLLD_1 + 770-1;
	//使能FLL
	__bic_SR_register(SCG0);
	//延时等待稳定
	__delay_cycles(400000);
	
	
	
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
	if(I == 770)
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

