// Source code under CC0 1.0
//
// This example uses Timer2 to PWM modulate 8-bit audio on a port A3.
// The CPU runs at 8 MHz, with an 8-bit resolution PWM signal, this yields
// 8MHz / 255 = 31372.5kHz sampling rate.

#include <stdint.h>
#include "stm8.h"

#define PWM_MODE_1 (6 << 4)

// 8-bit sine-wave for a 392Hz tone (31372.5kHz / 80 samples)
uint8_t tone[] = {128, 141, 154, 166, 178, 189, 199, 209, 217, 223, 229, 234,
                  238, 241, 244, 245, 246, 247, 247, 246, 244, 243, 240, 237,
                  233, 229, 225, 220, 214, 209, 202, 196, 189, 182, 175, 167,
                  160, 152, 144, 136, 128, 119, 111, 103, 95,  88,  80,  73,
                  66,  59,  53,  46,  41,  35,  30,  26,  22,  18,  15,  12,
                  11,  9,   8,   8,   9,   10,  11,  14,  17,  21,  26,  32,
                  38,  46,  56,  66,  77,  89,  101, 114};
uint8_t audio_idx = 0;

// On timer 2 overflow, load the next item for PWM.
ISR(timer2_isr, TIM2_OVR_UIF_vector) {
  // Sets the new CC (Counter Compare) on UEV
  TIM2_CCR3L = tone[audio_idx];
  TIM2_SR1 &= ~TIM_SR1_UIF;  // clear interrupt flag

  if (++audio_idx >= sizeof(tone)) audio_idx = 0;
}

void setup_sound() {
  // We set up Timer 2.
  // * It generates an overflow event after counting through 255 different
  //   values.
  // * Channel 3 is our PWM output. It's configured so that it's active/high
  //   while the timer is < CC3.

  // No prescaling. Defaults:
  // TIM2_PSCR = 0;

  // 255 values, gives us 8-bit resolution.
  TIM2_ARRH = 0;
  TIM2_ARRL = 254;  // counts from 0 to 254 incl -> 255 values

  // Port: PA3, Channel 3: Output Enable, Output-Compare-Preload-Enable
  // (we need to preload to be able to update the next pwm cycle without
  // affecting the current one)
  TIM2_CCMR3 = PWM_MODE_1 | 0x8;  // preload-enable
  TIM2_CCER2 = 1;                 // CC3 enable.

  // TIM2_IER (Interrupt Enable Register), Update interrupt (UIE) (bit 0)
  TIM2_IER |= TIM_IER_UIE;
  // TIM2_CR1 – Timer 2 Control Register 1, Counter ENable bit (CEN) (bit 0)
  TIM2_CR1 |= TIM_CR1_CEN;
}

void main(void) {
  CLK_CKDIVR = 0x8;  // CPU @8MHz , f_cpu = f_master, f_hsi /= 2

  setup_sound();

  rim();  // enable interrupts
  for (;;) {
    wfi();
  }
}
