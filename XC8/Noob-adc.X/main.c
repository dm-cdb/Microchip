/*
 * File:   main.c
 * Description :
 *   An analog example, using a potentiometer to vary voltage on GP4/AN3
 *   Set TRISIO GP4 as input
 *   Set ANSEL to FOSC/16, ANS3 (on GP4) analog intput
 *   Set ADCON0 with VDD (5v) as voltage ref, ADFM left justified, using AN3 channel
 * 
 * Author: Christian de Balorre
 * Created on 02 Feb 2023, 21:44
 */

// FUSES bits see 12.1 of PIC12F683 datasheet, Configuration Word Register
// INTOSCIO oscillator                 : Internal OSC, I/O function on GPIO4 & GPIO5
// INTOSCCLK oscillator                : Internal OSC, FOSC/4 output on GPIO4 CLKOUT, I/O on GP5
// Watchdog Timer Enable bit           : disabled (WDTE)
// Power-up Timer Enable bit           : enabled (PWRT)
// MCLR Reset Pin Function Select bit  : disabled (MCLRE)
// Code Protection bit                 : disabled (CP))
// Data Code Protection bit            : disabled (CPD)
// Brown Out Detect                    : disabled (BOREN)
// Internal External Switchover bit    : disabled (IESO)
// Fail-Safe Clock Monitor Enabled bit : disabled (FCMEN)
#pragma config FOSC = INTOSCIO, WDTE = OFF, PWRTE = ON, MCLRE = OFF 
#pragma config CP = OFF, CPD = OFF, BOREN = OFF, IESO = OFF, FCMEN = OFF

#include <xc.h>
#include <stdbool.h>

#define _XTAL_FREQ  4000000
#define LED1_ON 0b00000001
#define LED2_ON 0b00000010
#define LED3_ON 0b00000100
#define LED4_ON 0b00100000
#define ALL_ON  0b00100111

unsigned int adValue __at(0xa0);                // For debug

void PicInit (void) {
    OSCCONbits.IRCF = 0b110;    // Select 4MHz clock (default)
    CMCON0bits.CM = 0b111;       // Disable comparator   
    TRISIO = 0b00011000;        // GPIO<4-3> inputs (1), GPIO5 & GPIO<2-0> outputs (0)
    GPIO = 0;                   // Output driving low
    // FOSC/16 => 1 bit AD conversion takes 4µs @ 4MHz (tad) ; it takes 11 tads to complete ADC conversion
    ANSEL  = 0b01011000;        // FOSC/16, AN3 analog input
    ADCON0 = 0b00001100;        // Left justified ADRESH ADRESL,AN3 channel selected
    CMCON0 = 0x07;              // Comparator OFF
}

void main(void) {

 PicInit();
 ADCON0bits.ADON = 1;
 __delay_us(6);                  //Minimal delay for ADC to settle, see 9.3 of PIC 12F683 datasheet
 while (true){   
    adValue = 0;
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO == 1) {}   //Wait for A/D conversion (11 * tad + 100ns)
    adValue = (unsigned int)(ADRESH << 2);
    ADRESL = (ADRESL >> 6);
    adValue = (adValue | ADRESL); // v = (ADC * Vref) / 1024.0; @Vref = 5v => resolution (1 LSB) = 5/1024 = 0,00488 = 5mV
    
    // 200 = 0,97V ; 400 = 1,95V ; 600 = 2,92V ; 800 = 3,90v ; 1000 = 4,88V
    
    if (adValue < 200) {
        GPIO = LED1_ON;
        }
    if ((200 < adValue) && (adValue < 400)) {
        GPIO = LED2_ON;
        }
    if ((400 < adValue) && (adValue < 600)) {
        GPIO = LED3_ON;
        }
    if ((600 < adValue) && (adValue < 800)) {
        GPIO = LED4_ON;
        }
    if ((800 < adValue) && (adValue < 900)) {
        GPIO = ALL_ON;
        }
    if (adValue > 1000) {
        GPIO = 0;
        }
    } 
}