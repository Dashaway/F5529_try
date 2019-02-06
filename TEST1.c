#include "msp430f5529.h"

//190206
//按键亮灯


void IO_SET();

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;
	
	IO_SET();
	
	while(1)
	{
		P1OUT |= BIT0;
		if(P1IN & BIT1)
		{
			P4OUT |= BIT7;
		}
		else
		{
			P4OUT &= ~BIT7;
		}
		
	}
	
	
}

void IO_SET()
{
	P1DIR |= BIT0;
	
	P1DIR &= ~BIT1;
	P1REN |= BIT1;
	P1OUT |= BIT1;
	
	P4DIR |= BIT7;

}

