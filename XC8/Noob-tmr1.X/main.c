/*
 * File:   main.c
 * The Timer1 register pair (TMR1H:TMR1L) increments to FFFFh 
 * and rolls over to 0000h. 
 * When Timer1 rolls over, the Timer1 interrupt flag bit of the PIR1 register is set.
 * The program uses an interrupt routine to increment another counter
 * 
 * Author: Christian de Balorre
 * Created on March 5th 2023, 21:44
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

unsigned char toc;

//Declare functions
void __interrupt() isr(void);
void pic_Init(void);

// Interrupt function

void __interrupt() isr() {
    // Only process Timer1-triggered interrupts
    if (PIE1bits.TMR1IE && PIR1bits.TMR1IF) {
        TMR1L = 0xDB; // Preset Timer1 to 3035 - 0x0BDB
        TMR1H = 0x0B;
        toc++;
        PIR1bits.TMR1IF = 0;
    }
}

void pic_Init(void) {
    OSCCONbits.IRCF = 0b110;    // Select 4MHz clock
    CMCON0 = 0b00000111;        // Comparator OFF
    ANSEL = 0x00;               // Force all GPIO pins to digital mode
    TRISIO = 0b00111110;        // GPIO 
    INTCON = 0b11000000;        // Enable GIE & PEIE
    PIE1bits.TMR1IE = 1;        // Enable Timer1 interrupt
    T1CONbits.T1CKPS = 0b0011;  // Set prescaler 1:8
    return;
}

void main(void) {
    pic_Init();
    TMR1L = 0xDB;               // Preset Timer1 to 3035 - 0x0BDB
    TMR1H = 0x0B;               // Since prescaler i 1:8 : flag & interrupt set every (65535 - 3035) * 2 * 1µs = 5sec
    T1CONbits.TMR1ON = 1;       // Start Timer1
    
    toc = 0;
    
    while (true) {
        if (toc == 2) {
            GPIObits.GP0 = GPIObits.GP0 ^ 1;        // Toggle GP0 bit with xor
            toc = 0;
        }    
    }
}