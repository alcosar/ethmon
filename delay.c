#include "delay.h"

void delay_ms(int val) {
	volatile int i;
	volatile int i2;

	for(i = 0; i < val; i++) {
		i2 = 0;
		while(i2 != 12000) i2++;
	}
}
