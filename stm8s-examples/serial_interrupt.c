// An example for using interrupts while sending.

// See serial.c before to get a general understand of how the serial
// transmission works.

#include <stdint.h>
#include <stdio.h>

#define CLK_DIVR (*(volatile uint8_t *)0x50c6)
#define CLK_PCKENR1 (*(volatile uint8_t *)0x50c7)

#define UART1_SR (*(volatile uint8_t *)0x5230)
#define UART1_DR (*(volatile uint8_t *)0x5231)
#define UART1_BRR1 (*(volatile uint8_t *)0x5232)
#define UART1_BRR2 (*(volatile uint8_t *)0x5233)
#define UART1_CR2 (*(volatile uint8_t *)0x5235)
#define UART1_CR3 (*(volatile uint8_t *)0x5236)

#define UART_CR2_TEN (1 << 3)
#define UART_CR2_TIEN (1 << 7)
#define UART_CR3_STOP1 (1 << 4)
#define UART_CR3_STOP2 (1 << 5)
#define UART_SR_TXE (1 << 7)

// This is the IRQ number (see the MCU's datasheet). Note that the number starts
// with IRQ 0, i.e. RESET
// and TRAP are left out. This is in contrast to iostm8s003f3.h in which
// UART1_T_TXE_vector would be defined as 19.
#define UART1_T_TXE_vector 17

#define wfi()                                                                  \
  { __asm__("wfi\n"); } /* Wait For Interrupt */
#define rmi()                                                                  \
  { __asm__("rim"); } /* Enable interrupts */

// These are optimized by sdcc to use the bset/bres assembly instructions.
#define CLRBIT(REG, VALUE) REG &= ~VALUE
#define SETBIT(REG, VALUE) REG |= ~VALUE

// The interrupt is called every time the TXE (transmission byte empty)
// flag in UART_SR is set. It doesn't have any context, so needs to
// know everything to transmit the next byte.
char *buffer;
uint8_t buffer_position = 0;
uint8_t buffer_size = 0;

void uart1_isr(void) __interrupt(UART1_T_TXE_vector) {
  // The vector is used for multiple purposes, check that TXE is indeed set.
  if (UART1_SR & UART_SR_TXE) {
    if (buffer_position < buffer_size)
      UART1_DR = buffer[buffer_position++];
    else {
      // Transfer complete. Disable interrupts.
      // The main loop detects this condition.
      CLRBIT(UART1_CR2, UART_CR2_TIEN)
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

  CLK_DIVR = 0x00;    // Set the frequency to 16 MHz
  CLK_PCKENR1 = 0xFF; // Enable peripherals

  UART1_CR2 = UART_CR2_TEN;                        // Allow TX and RX
  UART1_CR3 &= ~(UART_CR3_STOP1 | UART_CR3_STOP2); // 1 stop bit
  UART1_BRR2 = 0x03;
  UART1_BRR1 = 0x68; // 9600 baud

  rmi(); // enable interrupts

  for (;;) {
    putstring("Hello world!\n", 6 + 7);
    do {
      wfi(); // Puts the processor in wait mode until an interrupt is received.
    } while (UART1_CR2 & UART_CR2_TIEN); // When the transmission is complete,
                                         // the interrupt handler clears this
                                         // flag.

    for (i = 0; i < 147456; i++)
      ; // Sleep
  }
}
