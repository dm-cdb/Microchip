/*
 * File:   main.c
 * This project use the capture feature of CCP1
 * Important : TMR1 must be in free running mode !
 * It captures the period (in TMR1 ticks) between rising/falling edges of a signal.
 * GP0 will be twice the period, or 1/8 of original frequency when 
 * capture mode is set to every 4th rising edge - ie for 500Hz input on CCP1 pin, 
 * output will be 62,5Hz
 * 
 * Author: Christian de Balorre
 * Created on December 29th 2025, 21:44
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
#include <stdint.h>

#define _XTAL_FREQ  4000000     // Needed for built-in  __delay_us(x) & __delay_ms(x)
#define FOSC _XTAL_FREQ         // Fosc in Hz - for information, not used in this project
#define TOSC 1/FOSC             // Period = 0,250 탎
#define FCYC FOSC/4             // Instruction freq - one instruction consumes 4 cycles
#define TCYC 1/FCYC             // Instruction period (0,250탎 * 4 = 1탎 for default 4MHz FOSC)

//Global variables
volatile unsigned int capture_old = 0;// Always use volatile for variable outside main() ; ie interrupt()
volatile unsigned int capture_new = 0;// Note : 16 bits variable stored in little endian : LSB firts, MSB second
volatile __bit first;
volatile unsigned int period = 0;

//Declare functions
void __interrupt() isr(void);
void init_Pic(void);

void __interrupt() isr(void) {
    if (PIE1bits.CCP1IE && PIR1bits.CCP1IF) {
        if (first) { // The first capture is always wrong
            first = 0;
            PIR1bits.CCP1IF = 0;
            TMR1H = 0;
            TMR1L = 0;
            return;
        }
        // Read captured value
        capture_new = ((unsigned int) CCPR1H << 8) | CCPR1L;

        // Compute period
        period = capture_new - capture_old; // Unit = Timer1 tick = (TCYC (1탎) * prescaler (2)) = 2탎
        capture_old = capture_new;
        GPIObits.GP0 ^= 1;   // LED on/off will be twice the captured period * capture mode
        PIR1bits.CCP1IF = 0; // Clear flag
    }
}

void init_Pic() {

    OSCCON = 0b01100000; // FOSC @ 4 MHz
    // GPIO
    TRISIO = 0b00111110; // GP2 input (CCP1); GP0 output
    ANSEL = 0x00; // All pins digital
    CMCON0 = 0x07; // Comparator off

    // Timer1 config
    T1CONbits.TMR1CS = 0; // Use internal clock
    T1CONbits.T1CKPS = 0b01; // Prescaler 1:2 : TMR1 tick = (TCYC (1탎) * prescaler (2)) = 2탎

    CCP1CONbits.CCP1M = 0b0110; // Capture mode, every 4th rising edge
    // Interrupt
    PIR1bits.CCP1IF = 0;
    PIE1bits.CCP1IE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

}

void main(void) {
    init_Pic();
    first = true;
      T1CONbits.TMR1ON = 1; // Timer1 ON
    
    while (true) {
    }
}