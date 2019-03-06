// Source code under CC0 1.0
#include <stdint.h>
#include "stm8.h"

// Read the timer registers.
unsigned short clock(void) {
  unsigned char h = TIM1_CNTRH;
  unsigned char l = TIM1_CNTRL;
  return ((unsigned short)(h) << 8 | l);
}

void main(void) {
  CLK_CKDIVR = 0x00;  // Set the frequency to 16 MHz

  // Configure timer
  // At 16MHz, we set the prescaler to 16000 (0x3e80).
  // This gives us 16MHz / 16k = 1kHz, i.e. 1000 ticks per second.
  TIM1_PSCRH = 0x3e;
  TIM1_PSCRL = 0x80;
  // Enable timer
  TIM1_CR1 = 0x01;

  // The onboard LED is on Port B5
  PB_DDR = 0x20;  // 0x20 = 1 << 5
  PB_CR1 = 0x20;

  // Busy-wait and change LED state once per second.
  for (;;) PB_ODR = (clock() % 1000 < 500) << 5;
}
