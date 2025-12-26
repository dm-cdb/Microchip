/*
 * File:   main.c
 * This simple project shows how to use the TMR2 counters with the PR2 register.
 * When TMR2 = PR2, TMR2 is reset to 0 and TMR2IF flag is set
 * Here the prescaler is set to 1:16, which means it will need to 16 ticks clock
 * to increment TMR2. You'll see the simple math below
 * Author: Christian de Balorre
 * Created on October 16th 2024, 19:21
 *
 * 
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
#define FCYC FOSC/4             // Instruction freq - one instruction consumes 4 clocks
#define TCYC 1/FCYC             // Instruction period (1µs for default 4MHz FOSC)


//Declare functions
void __interrupt() isr(void);
void pic_Init(void);

void __interrupt() isr() {
     // Only process Timer2 triggered interrupts
    if (PIE1bits.TMR2IE && PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;
        GPIObits.GP0 ^= 1;      // Toggle GPIO0
        return;
    }
}

void pic_Init(void) {
    OSCCONbits.IRCF = 0b110;    // Select 4MHz clock
    CMCON0 = 0b00000111;        // Comparator OFF
    ANSEL = 0x00;               // Force all GPIO pins to digital mode
    TRISIO = 0b00111110;        // GPIO 0 outputs (0)
    T2CONbits.T2CKPS = 0b0011;  // Set prescaler 1:16
    PR2 = 0x7d;                 // Set TMR2 register limit to 125 = 1µs * 16 * 125 = 2ms duty cycle => 250Hz
    INTCONbits.GIE = 1;		// Enable General Interrupt
    INTCONbits.PEIE = 1;	// Enable Interrupt in the PIE1 register ; this register control the interrupt of TMR2
    PIE1bits.TMR2IE = 1;        // Enable TMR2 interrupt
    return;
}

void main(void) {
    pic_Init();
    T2CONbits.TMR2ON = 1;       // Start timer
    
    while(true);
}
