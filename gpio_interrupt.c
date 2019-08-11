// Source code under CC0 1.0
#include <stdint.h>
#include "stm8.h"

#define LED_PIN PIN5
#define INTERRUPT_PIN PIN4

ISR(gpio_isr, EXTI_PORTB_vector) {
  // Turn on the LED.
  PB_ODR = 0;
}

void main(void) {
  // Configure PORT B.
  // LED_PIN is output HIGH (which is off).
  // INTERRUPT_PIN is floating input with interrupts enabled.
  PB_ODR = LED_PIN;
  PB_DDR = LED_PIN;
  PB_CR1 = LED_PIN;
  PB_CR2 = INTERRUPT_PIN;
  
  // Disable interrupts to make sure we can set the EXTI registers.
  sim();
  // Interrupt on rising or falling edge for PortB
  EXTI_CR1 = 3 << 2;

  rim();  // enable interrupts

  for(;;) {
    // turn off the cpu, wait for external interrupt
    halt();
    // wait a few cycles
    for (uint32_t i=100000;i!=0;i--) ;
    // turn off the LED again.
    PB_ODR = LED_PIN;
  }
}
