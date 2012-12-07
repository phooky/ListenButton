
#include "core_pins.h"

// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 hes the LED on Teensy 2.0
// Pin 6 hes the LED on Teensy++ 2.0
// give it a name:
int led = 13;

// We're looking for a 44.1 or 22.05KHz. With a ADCK at 1.5MHz,
// that's about 34 clock cycles.
// For 16-bit single ended:
// Base conversion in ADCK cycles: 25
// LST can add 0, 2, 6, 12, or 20 cycles
// HSC can add 0 or 2
// Slow sample mode (ADLSMP set)

// LST: 6 cycles
// HSC off

void audioSetup() {
  // wait for calibration to complete
  while ((ADC0_SC3 & ADC_SC3_CAL) != 0) ;

  // We are sampling on "A0", or ADC0_SE5b.
  // Low power mode limits us to ~2.4MHz; far more than we need.
  // We operate at 1.5MHz (48MHz / 2*16)
  ADC0_CFG1 = ADC_CFG1_ADLPC | ADC_CFG1_ADIV(3) | ADC_CFG1_ADLSMP |
    ADC_CFG1_MODE(3) | ADC_CFG1_ADICLK(1);
  // We are using the b bank of analog inputs; we are using a moderately
  // long sample time.
  ADC0_CFG2 = ADC_CFG2_MUXSEL | ADC_CFG2_ADLSTS(2);

  // Teensy sets up the external reference
  ADC0_SC2 = ADC_SC2_REFSEL(0); // vcc/ext ref
  // Set up software conversion mode, continuous conversion
  ADC0_SC3 = ADC_SC3_ADCO;
  __disable_irq();
  // Enable the IRQ for the ADC
  NVIC_ENABLE_IRQ(IRQ_ADC0);
  __enable_irq();
  // Set interrupt on conversion; "A0" is ADC0_SE5b
  ADC0_SC1A = ADC_SC1_AIEN | ADC_SC1_ADCH(5);
  //result in ADC0_RA

}


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
  Serial.begin(115200);
  audioSetup();
  digitalWrite(led, HIGH);
}

bool state = false;

extern "C" {
// ADC0 interrupt vector is 0x0098, vector 38, irq 22, ipr reg # 5
void adc0_isr(void)
{
  volatile uint32_t result = ADC0_RA;
  digitalWrite(led, state?LOW:HIGH);
  state = !state;
}
}


// the loop routine runs over and over again forever:
void loop() {
}
