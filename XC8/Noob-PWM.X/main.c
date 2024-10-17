/*
 * File:   main.c
 * Author: Christian de Balorre
 * Created on 28 mars 2023, 15:21
 * Notes :
 * See adc project for Analog-to-digital paramaters and requirements
 * PWM output on GP2/CCP1, modulated from potentiometer on GP4
 * We want a 500hz frequency to modify led brightness
 * PR2 = [4MHz / (4 * 16 * 500Hz)] -1 = 124
 */

#include <xc.h>
#include <stdbool.h>

// OCTETS DE CONFIGURATION (FUSIBLES)
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

#define _XTAL_FREQ 4000000      // Used by built-in delay()
#define FOSC  _XTAL_FREQ
#define TOSC (1 / FOSC)   // 0,25us - 250 ns
#define FCY (FOSC / 4)    // Instruction cycle = 4 clocks
#define TCY (1 / FCY)           // 1 us / instruction/*
 
// Declare functions
void pic_Init(void);

void pic_Init() {
    OSCCONbits.IRCF = 0b110;    // 4MHz
    CMCON0bits.CM = 0b111;      // Disable comparator
    ANSEL  = 0b01011000;        // FOSC 1/16 (conversiont delay -> 100ns + 11 x 4µs), AN3 (GP4)
    TRISIO = 0b00111111;
    ADCON0 = 0b00001100;        // ADFM left, Vref = VDD, AN3 (GP4)
    CCP1CON = 0b00001100;       // PWM mode, DB bits to 0, active-high

    return;
}

void main (void){
    pic_Init();
    // PWM Period = [(PR2) + 1] * 4 * TOSC * (TMR2 Prescale Value)
    // Pulse Width = (CCPR1L:CCP1CON<5:4>) * TOSC * (TMR2 Prescale Value)
    // Duty Cycle Ratio = (CCPR1L:CCP1CON<5:4>) / 4(PR2 + 1)
    CCPR1L = 0b00111110;        // 250 | 62 => Period = 2ms ; Pulse = 1ms ; Duty cycle = 50%
    T2CONbits.T2CKPS = 0b11;    // Prescaler 1:16
    PR2 = 124;                  // PR2 specifies the period : [(PR2) + 1] * 4 * TOSC * (TMR2 Prescale Value) = 500Hz
    T2CONbits.TMR2ON = 1;
    while (!PIR1bits.TMR2IF);
    // Set PWM on GP2 - CCP1
    TRISIObits.TRISIO2 = 0;
    // ADC routine
    PIR1bits.ADIF = 0;
    while (true) {
        ADCON0bits.ADON = 1;
        __delay_us(4);          // Acquisition time after ADC init : amplifier settling + CHOLD times
        ADCON0bits.GO_DONE = 1;
        while(!PIR1bits.ADIF);
        CCPR1L = ADRESH;        // New pulse width modulation
        PIR1bits.ADIF = 0;
        __delay_ms(250);        
    }
    return;
}
