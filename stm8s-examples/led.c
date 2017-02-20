#include <stdint.h>

#define CLK_DIVR	(*(volatile uint8_t *)0x50c6)
#define CLK_PCKENR1	(*(volatile uint8_t *)0x50c7)

#define TIM1_CR1	(*(volatile uint8_t *)0x5250)
#define TIM1_CNTRH	(*(volatile uint8_t *)0x525e)
#define TIM1_CNTRL	(*(volatile uint8_t *)0x525f)
#define TIM1_PSCRH	(*(volatile uint8_t *)0x5260)
#define TIM1_PSCRL	(*(volatile uint8_t *)0x5261)

#define PB_ODR	(*(volatile uint8_t *)0x5005)
#define PB_DDR	(*(volatile uint8_t *)0x5007)
#define PB_CR1	(*(volatile uint8_t *)0x5008)


unsigned int clock(void)
{
	unsigned char h = TIM1_CNTRH;
	unsigned char l = TIM1_CNTRL;
	return((unsigned int)(h) << 8 | l);
}

void main(void)
{
	CLK_DIVR = 0x00; // Set the frequency to 16 MHz

	// Configure timer
	// 1000 ticks per second
	TIM1_PSCRH = 0x3e;
	TIM1_PSCRL = 0x80;
	// Enable timer
	TIM1_CR1 = 0x01;

	PB_DDR = 0x20;
	PB_CR1 = 0x20;

	for(;;)
		PB_ODR = (clock() % 1000 < 500) << 5;
}
