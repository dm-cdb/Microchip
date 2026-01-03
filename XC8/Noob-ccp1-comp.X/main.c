/*
 * File:   main.c
 * In Compare mode, the 16-bit CCPR1 register value is constantly compared
 * against the TMR1 register pair value.
 * This program tests basic compare features : set, clear CCP1 pin (GP2)
 * CCP1 pin must be set as output
 * 
 * Author: Christian de Balorre
 * Created on April 8th 2023, 21:44
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
#define TOSC 1/FOSC             // Period = 0,250 탎
#define FCYC FOSC/4             // Instruction freq - one instruction consumes 4 clocks
#define TCYC 1/FCYC             // Instruction period (1탎 for default 4MHz FOSC)

#define BAUDR 9600              // Baud rate = 9600 bps -> 104 �/bit
#define BITDEL  103             // Bit delay 104 - isr TCYC
#define DATABITS 8
#define TX GPIObits.GP1         // UART TX pin


//Global variables
unsigned const char myText1[] = "OC pin toggling...";
unsigned const char myText2[] = "OC pin SET...";
unsigned const char myText3[] = "OC pin CLEAR, GP0 SET...";
unsigned const char rn[] = "\r\n"; // Char line feed
static volatile unsigned char txBuffer __at(0x70); //Force var in shared ram for debug
static volatile __bit TXFLAG;
unsigned char counter;


//Declare functions
void init_Pic(void);

void tx_UART(void);
void send_Txt(const unsigned char *, unsigned char);

void tx_UART() {
    TXFLAG = 1;
    TX = 0; // Send Start Bit       
    _delay(BITDEL - 11);
    for (unsigned char i = 1; i; i = (unsigned char) (i << 1)) {
        (txBuffer & i) ? (TX = 1) : (TX = 0);
        _delay(BITDEL - 19);
    }
    TX = 1; // Send Stop bit
    _delay(BITDEL);
    TXFLAG = 0;
    txBuffer = 0x00;
    return;
}

void send_Txt(const unsigned char * s, unsigned char k) {
    for (unsigned char i = 0; i < k; i++) {
        txBuffer = s[i];
        tx_UART();
    }
    return;
}

void init_Pic() {
    OSCCONbits.IRCF = 0b110; // Select 4MHz clock
    CMCON0bits.CM = 0b111; // Comparator OFF
    TRISIO = 0b00111011; // GPIO 2 outputs (0)
    GPIO = 0x00; // OUTPUT LOW
    CCPR1H = 0b01011010; // CCPR1 = 0x5AEE = 23278 ticks = 0,025ms w/o prescaler
    CCPR1L = 0b11101110; // TMR1 registers will be compared with these registers
    T1CONbits.TMR1CS = 0; // TMR1 with internal Clock source FOSC/4 = 1탎
    T1CONbits.T1CKPS = 0b10; // Prescale 1:4 => 1 TMR1 tick = 4 탎
    T1CONbits.TMR1ON = 0;
    TMR1L = 0x00;
    TMR1H = 0x00;
    return;
}

void main(void) {
    init_Pic();
    TX = 1;
   send_Txt(myText1, sizeof (myText1));
   send_Txt(rn, sizeof (rn));
   PIR1bits.CCP1IF = 0;
   T1CONbits.TMR1ON = 1;
   CCP1CONbits.CCP1M = 0b1001; // OC pin cleared (GP2)
   while (true) {
        while (!PIR1bits.CCP1IF);
        CCP1CONbits.CCP1M = 0b1000; // OC pin set (GP2)
        PIR1bits.CCP1IF = 0;
        TMR1L = 0x00;
        TMR1H = 0x00;
        while (!PIR1bits.CCP1IF);
        CCP1CONbits.CCP1M = 0b1001; // OC pin cleard (GP2)
        PIR1bits.CCP1IF = 0;
        TMR1L = 0x00;
        TMR1H = 0x00;
    }
}   
