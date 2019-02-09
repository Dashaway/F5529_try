#include "msp430f5529.h"


//190206
//TA1定时器亮灯

//频率设置

//TA2定时器亮灯

unsigned int I,J,TA2_I,SW;

void IO_SET();
void SetVcoreUp(unsigned int level);
void CLK_SET();
void TIMER_SET();

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;
	
	J = 625;
	SW = 0;
	
	//初始设置
	IO_SET();
	CLK_SET();
	TIMER_SET();
	
	P1OUT |= BIT0;
	P4OUT &= ~BIT7;
	
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
	/*
	LPM0
	
	单TA0亮灯最高频率：
	SMCLK频率设置：32768*2*770 ~50Mhz
	MCLK频率设置：32768*2*770 ~50Mhz
	
	TA0、TA2亮灯最高频率：
	SMCLK频率设置：32768*2*750 ~45Mhz
	MCLK频率设置：32768*2*750 ~45Mhz
	
	
	*/
	
	SetVcoreUp(0x03);
	
	//清空DCOx和MODx
	UCSCTL0 = 0;
	
	//禁止FLL
	__bis_SR_register(SCG0);
	//DCO频率范围6~54Mhz
	UCSCTL1 = DCORSEL_6;
	//FLL预先2分频
	//倍频系数
	UCSCTL2 = FLLD_1 + 625-1;
	//FLL参考时钟XT1CLK,不预分频
	UCSCTL3 = SELREF_0 + FLLREFDIV_0;
	//ACLK选择XT1CLK,SMCLK选择DCOCLK,MCLK选择DCOCLKDIV
	UCSCTL4_L = SELA_0 + SELS_3 + SELM_4;
	//使能FLL
	__bic_SR_register(SCG0);
	//延时等待稳定
	__delay_cycles(400000);
	
	
	
}

void TIMER_SET()
{
	/*
	定时器总设置
	中断优先级：
	TB0>WDT>TA0>TA1>TA2>RTC
	
	*/
	
	//TB0
	
	
	//TA0
	
	//使能CCR0终端允许
	TA0CCTL0 = CCIE;
	//计数值
	//TA0CCR0 = 0xffff;
	//时钟源SMCLK，连续计数，不分频，清除TAR
	TA0CTL = TASSEL_2 + MC_2 + ID_0 + TACLR;
	
	//TA1
	
	
	//TA2
	
	//使能CCR0终端允许
	TA2CCTL0 = CCIE;
	//计数值
	TA2CCR0 = 32-1;
	//时钟源ACLK，增计数，不分频，清除TAR
	TA2CTL = TASSEL_1 + MC_1 + ID_0 + TACLR;
	
	
}


#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0(void)
{
	//TIMER0_A0中断
	I++;
	if(I >= J)
	{
		I = 0;
		//P1OUT ^= BIT0;
	}
	
	
}


#pragma vector = TIMER2_A0_VECTOR
__interrupt void TIMER2_A0(void)
{
	//TIMER2_A0中断
	
	
	
	//按键识别
	if(~P1IN & BIT1)
	{
		//触发时操作
		TA2_I++;
		SW = 7;
		
		//中按常亮指示，长按闪烁指示
		if((TA2_I >= 512) && (TA2_I < 1536))
		{
			P4OUT |= BIT7;
		}
		else if((TA2_I >= 1536) && (TA2_I < 3072))
		{
			if(TA2_I%64 == 64-1)
			{
				P4OUT ^= BIT7;
			}
		}
		else if((TA2_I >= 3072))
		{
			P4OUT &= ~BIT7;
		}
	
	}
	
	
	if((P1IN & BIT1) && (SW == 7))
	{
		//释放后操作
		if((TA2_I >= 20) && (TA2_I < 512))
		{
			//短按模式
			SW = 1; 
		}
		else if((TA2_I >= 512) && (TA2_I < 1536))
		{
			//中按模式
			SW = 2;
		}
		else if((TA2_I >= 1536) && (TA2_I < 3072))
		{
			//长按模式
			SW = 3;
		}
		else 
		{
			//无操作
			SW = 0;
		}
		
		P4OUT &= ~BIT7;
		TA2_I = 0;
	}
	
	
	if((SW != 0) && (SW < 6))
	{
		switch(SW)
		{
			case 1:
				{
					P1OUT ^= BIT0;
					SW = 0;
				}
			case 2:
				{
					P1OUT |= BIT0;
					SW = 0;
				}
			case 3:
				{
					P1OUT &= ~BIT0;
					SW = 0;
				}
			case 4:
				{
					SW = 0;
				}
			case 5:
				{
					SW = 0;
				}
			default:
				{
					SW = 0;
				}
	
		}
	
	}


	
}


