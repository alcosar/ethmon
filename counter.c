#include "stm32f10x.h"
#include "counter.h"

volatile uint16_t ms_count;
volatile uint32_t tick_count;
volatile uint32_t second_count;

void TIM6_DAC_IRQHandler (void) //100mSec
{
	TIM6->SR &= ~TIM_SR_UIF; //Clear status register update
		
	++ms_count;
	++tick_count;
	
	if(ms_count == 1000)
	{
		++second_count;
		ms_count = 0;
	}

}
