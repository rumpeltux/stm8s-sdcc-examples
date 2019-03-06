// Source code under CC0 1.0
// Src & walkthrough: http://www.colecovision.eu/stm8/HCDVBD0017%20Serial.shtml

#include <stdint.h>
#include <stdio.h>
#include "stm8.h"

void putchar(char c) {
  while (!(UART1_SR & UART_SR_TXE))
    ;
  UART1_DR = c;
}

void main(void) {
  unsigned long i = 0;

  CLK_CKDIVR = 0x00;   // Set the frequency to 16 MHz
  CLK_PCKENR1 = 0xFF;  // Enable peripherals

  UART1_CR2 = UART_CR2_TEN;                         // Allow TX and RX
  UART1_CR3 &= ~(UART_CR3_STOP1 | UART_CR3_STOP2);  // 1 stop bit
  UART1_BRR2 = 0x03;
  UART1_BRR1 = 0x68;  // 9600 baud

  for (;;) {
    printf("Hello World!\n");
    for (i = 0; i < 147456; i++)
      ;  // Sleep
  }
}
