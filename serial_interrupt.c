// An example for using interrupts while sending.

// See serial.c before to get a general understand of how the serial
// transmission works.

#include <stdint.h>
#include <stdio.h>
#include "stm8.h"

// These are (sometimes) optimized by sdcc to use the bset/bres assembly
// instructions.
#define CLRBIT(REG, VALUE) REG &= ~VALUE
#define SETBIT(REG, VALUE) REG |= ~VALUE

// The interrupt is called every time the TXE (transmission byte empty)
// flag in UART_SR is set. It doesn't have any context, so needs to
// know everything to transmit the next byte.
char *buffer;
uint8_t buffer_position = 0;
uint8_t buffer_size = 0;

ISR(uart1_isr, UART1_T_TXE_vector) {
  // The vector is used for multiple purposes, check that TXE is indeed set.
  if (UART1_SR & UART_SR_TXE) {
    if (buffer_position < buffer_size)
      UART1_DR = buffer[buffer_position++];
    else {
      // Transfer complete. Disable interrupts.
      // The main loop detects this condition.
      CLRBIT(UART1_CR2, UART_CR2_TIEN);
    }
  }
}

void putstring(char *s, uint8_t size) {
  buffer = s;
  buffer_position = 0;
  buffer_size = size;
  // Enables the interrupt, which is called immediately if TXE is already set.
  SETBIT(UART1_CR2, UART_CR2_TIEN);
}

void main(void) {
  unsigned long i = 0;

  CLK_CKDIVR = 0x00;   // Set the frequency to 16 MHz
  CLK_PCKENR1 = 0xFF;  // Enable peripherals

  UART1_CR2 = UART_CR2_TEN;                         // Allow TX and RX
  UART1_CR3 &= ~(UART_CR3_STOP1 | UART_CR3_STOP2);  // 1 stop bit
  UART1_BRR2 = 0x03;
  UART1_BRR1 = 0x68;  // 9600 baud

  rim();  // enable interrupts

  for (;;) {
    putstring("Hello world!\n", 6 + 7);
    do {
      wfi();  // Puts the processor in wait mode until an interrupt is received.
    } while (UART1_CR2 & UART_CR2_TIEN);  // When the transmission is complete,
                                          // the interrupt handler clears this
                                          // flag.

    for (i = 0; i < 147456; i++)
      ;  // Sleep
  }
}
