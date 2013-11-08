/* STM32 includes */
#include "stm32f10x.h"
#ifdef USE_STDPERIPH_DRIVER
#include <stm32f10x_conf.h>
#endif

#include "main.h"
#include "enc28j60.h"

#include "lan.h"

unsigned char revision = 0;

void sysInit(void)
{
	RCC->APB2ENR |=
		RCC_APB2ENR_IOPAEN |
		RCC_APB2ENR_IOPBEN |
		RCC_APB2ENR_IOPCEN |
		RCC_APB2ENR_IOPDEN |
		RCC_APB2ENR_USART1EN |
		RCC_APB2ENR_AFIOEN;

	//ENC
	//RESET
	GPIOA->CRL &= ~GPIO_CRL_MODE1;
	GPIOA->CRL |= GPIO_CRL_MODE1_1;
	GPIOA->CRL &= ~GPIO_CRL_CNF1;
	GPIOA->BSRR |= GPIO_BSRR_BS1;

	//////////SPI////////////////////////

	AFIO->MAPR   &= ~AFIO_MAPR_SPI1_REMAP; //Selecrt SPI on ports PA4, PA5, PA6, PA7
	//CS
	GPIOA->CRL &= ~GPIO_CRL_MODE4;
	GPIOA->CRL |= GPIO_CRL_MODE4_0;
	GPIOA->CRL &= ~GPIO_CRL_CNF4;
	//SCK
	GPIOA->CRL &= ~GPIO_CRL_MODE5;
	GPIOA->CRL |= GPIO_CRL_MODE5_0;
	GPIOA->CRL &= ~GPIO_CRL_CNF5;
	GPIOA->CRL |= GPIO_CRL_CNF5_1;
	//MISO
	GPIOA->CRL &= ~GPIO_CRL_MODE6;
	GPIOA->CRL &= ~GPIO_CRL_CNF6;
	GPIOA->CRL |= GPIO_CRL_CNF6_0;
	//MOSI
	GPIOA->CRL &= ~GPIO_CRL_MODE7;
	GPIOA->CRL |= GPIO_CRL_MODE7_0;
	GPIOA->CRL &= ~GPIO_CRL_CNF7;
	GPIOA->CRL |= GPIO_CRL_CNF7_1;

	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	SPI1->CR1 |= SPI_CR1_MSTR;		//MSTR = 1
	SPI1->CR1 |= SPI_CR1_SSM;
	SPI1->CR1 |= SPI_CR1_SSI;			//ENABLE SOFTWARE CONTROL CS
	SPI1->CR1 &= ~SPI_CR1_BR;	 // 24 / 2 = 12 MHz
	//SPI1->CR1 |= SPI_CR1_BR_0;	// 24 / 4 = 6 MHz
	//SPI1->CR1 |= SPI_CR1_BR_0;	// 24 / 8 = 3 MHz
	//SPI1->CR1 |= SPI_CR1_BR_0;
	//SPI1->CR1 &= ~SPI_CR1_CPHA;
	//SPI1->CR1 &= ~SPI_CR1_CPOL;
	SPI1->CR1 |= SPI_CR1_SPE; //ENABLE SPI



	//LEDS
	//Blue
	GPIOC->CRH &= ~GPIO_CRH_MODE8;
	GPIOC->CRH |= GPIO_CRH_MODE8_1;
	GPIOC->CRH &= ~GPIO_CRH_CNF8;
	//Green
	GPIOC->CRH &= ~GPIO_CRH_MODE9;
	GPIOC->CRH |= GPIO_CRH_MODE9_1;
	GPIOC->CRH &= ~GPIO_CRH_CNF9;

	//Stepper Motor Outputs
	GPIOC->CRL &= ~GPIO_CRL_MODE0;
	GPIOC->CRL |= GPIO_CRL_MODE0_1;
	GPIOC->CRL &= ~GPIO_CRL_CNF0;

	GPIOC->CRL &= ~GPIO_CRL_MODE1;
	GPIOC->CRL |= GPIO_CRL_MODE1_1;
	GPIOC->CRL &= ~GPIO_CRL_CNF1;

	GPIOC->CRL &= ~GPIO_CRL_MODE2;
	GPIOC->CRL |= GPIO_CRL_MODE2_1;
	GPIOC->CRL &= ~GPIO_CRL_CNF2;

	GPIOC->CRL &= ~GPIO_CRL_MODE3;
	GPIOC->CRL |= GPIO_CRL_MODE3_1;
	GPIOC->CRL &= ~GPIO_CRL_CNF3;



	//Timer6
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6->PSC = 23999; // 24 000 000 Mhz / (23999 + 1) = 1000, 1 / 1000 = 1 mSec
	TIM6->ARR = 1; //1 mSec (counter reload)
	TIM6->CR1 |= TIM_CR1_CEN; //enable timer
	TIM6->DIER |= TIM_DIER_UIE; //enable interrupt

	//Timer6
	//RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
	//TIM7->PSC = 239; // 24 000 000 Mhz / (239 + 1) = 100 000, 1 / 100 000 = 10 uSec
	//TIM7->ARR = 1; //20 uSec (counter reload)
	//TIM7->CR1 |= TIM_CR1_CEN; //enable timer
	//TIM7->DIER |= TIM_DIER_UIE; //enable interrupt

	__enable_irq();


	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	NVIC_SetPriority(TIM6_DAC_IRQn, 2); 

	//NVIC_EnableIRQ(TIM7_IRQn);
	//NVIC_SetPriority(TIM7_IRQn, 3); 

}


void TIM7_IRQHandler (void) {//20uSec

	TIM7->SR &= ~TIM_SR_UIF; //Clear status register update


}



// void udp_packet(eth_frame_t *frame, uint16_t len)
// {
// 	ip_packet_t *ip = (void*)(frame->data);
// 	udp_packet_t *udp = (void*)(ip->data);
// 	uint8_t *data = udp->data;

// 	LED_GREEN_ON;
// 	
// 	data[0] = 0x30;
// 	data[1] = 0x31;
// 	data[2] = 0x32;
// 	data[3] = 0x33;
// 	data[4] = 0x34;
// 	data[5] = 0x35;
// 	udp_reply(frame, 6);
// }

void udp_packet(eth_frame_t *frame, uint16_t len)
{
	ip_packet_t *ip = (void*)(frame->data);
	udp_packet_t *udp = (void*)(ip->data);
	uint8_t * data = udp->data;

	(void) len;
	if(udp->to_port == htons(1990)) 
	{		
		if(!memcmp(data, "preved", 6))
		{
			memcpy(data, "medved", 6);
			udp_reply(frame, 6);
		} 
		else if(!memcmp(data, "value", 6))
		{
			memcpy(data, "123456", 6);
			udp_reply(frame, 6);
		}
	}
}

/**
 * Main function
 */
int main(void)
{
	sysInit();
	lan_init();

	while(1)
	{
		lan_poll();
		if(BUTTON) {
			while(BUTTON);

			LED_GREEN_ON;
			revision = enc28j60_rcr(EREVID);
			revision = 0;

		}
	}
}

