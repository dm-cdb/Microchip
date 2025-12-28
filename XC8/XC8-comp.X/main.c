/*
 * File:   main.c
 * This program uses the comparator feature of the PIC 12F683.
 * The comparator feature offers 8 different comparator modes.
 * Here we chose the mode where Cin- (GP1) is used and a Vref is used as Cin+
 * Vref is set at 3V ; if CIN- < Vref, Cout (GP2) is on and GP5 is off
 * If Cin- > Vref, COUT is off and GP5 is on. 
 * 
 * Author: Christian de Balorre
 * Created on April 6th 2023, 21:44
 */

// CONFIG register (FUSES)
// INTOSCIO oscillator                 : I/O function on GPIO4 & GPIO5
// INTOSCCLK oscillator                : FOSC/4 output on GPIO4 CLKOUT, I/O on GP5
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

#define _XTAL_FREQ  4000000     // Needed for built-in  __delay_us(x) & __delay_ms(x)
#define FOSC _XTAL_FREQ         // Fosc in Hz - for information, not used in this project
#define TOSC 1/FOSC             // Period = 0,250 �s
#define FCYC FOSC/4             // Instruction freq - one instruction consumes 4 clocks
#define TCYC 1/FCYC             // Instruction period (1�s for default 4MHz FOSC)

//Declare functions
void __interrupt() isr(void);
void init_Pic(void);

// Interrupt function

void __interrupt() isr() {
    if (PIE1bits.CMIE && PIR1bits.CMIF) {
        PIR1bits.CMIF = 0;
        GPIObits.GP5 = ~CMCON0bits.COUT; // Switch LED status according to COUT value
    }
}

void init_Pic(void) {
    OSCCONbits.IRCF = 0b110; // Select 4MHz clock
    ANSEL = 0x00; // Force all GPIO pins to digital mode
    CMCON0bits.CM = 0b011; // CIN- analog, CIN+ I/O, COUT pin active, CVref on none-inverting
    VRCON = 0b10001011; // CVREF = VDD/4 + (VR<3:0>/32) * VDD => 5/4 + (11/32) * 5 = 3V
    TRISIO = 0b00011011; // GPIO 5/2 outputs (0)
    GPIO = 0x00; // Output driving low
    return;
}

void main(void) {
    init_Pic();
    PIE1bits.CMIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    while (true) {
        //built-in
        __nop();
        __delay_ms(1000);
        __delay_us(1000);
    }
}