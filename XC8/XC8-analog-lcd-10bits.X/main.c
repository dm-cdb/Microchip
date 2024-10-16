/*
 * File:   main.c
 * Author: Christian de Balorre
 * Adapted from https://circuitdigest.com/microcontroller-projects/pic-microcontroller-pic16f877a-adc-tutorial
 * Created on 22 mars 2023, 15:21
 * Notes :
 * Set GO/DONE bit -> TAD = 100 ns (0,1탎)
 * Analog-to-digital conversion tad cycles  = 0,1탎 + (11 x 4탎) = 44,1 탎
 * After the analog input channel is selected (or changed),
 * An A/D acquisition must be done before the conversion (charge holding capacitor) = 5탎 at 10K Ohms
 * The charge holding capacitor (CHOLD) is not discharged after each conversion.
 * Maximum recommended impedance for analog sources is 10k Ohms.
 */

#include <xc.h>
#include <string.h> // Used by memset())
#include <stdbool.h>
#include "lcd.h"

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
#define TOSC (1 / FOSC)         // 0,25us - 250 ns
#define FCY (FOSC / 4)          // Instruction cycle = 4 clocks
#define TCY (1 / FCY)           // 1 us / instruction

const unsigned char myText[] = "Volt :";
volatile unsigned int result __at(0x70); // Variables modified by isr routine ; absolute for easier debug
__uint24 convert;
unsigned int unit;
__bit ADCFLAG;

struct holder {
    char arr[10];
};

// Declare functions
void __interrupt() isr(void);
void pic_Init(void);
// Project functions
char *do_bdc(int, char);

void __interrupt() isr() {
    if (PIE1bits.ADIE && PIR1bits.ADIF) {
        PIE1bits.ADIE = 0;          // Disable ADC interrupt & clear flag, process result
        PIR1bits.ADIF = 0;
        ADCFLAG = 1;
        result = (unsigned int)((ADRESH << 8) + ADRESL);
        return;
    }
}

void pic_Init() {
    OSCCONbits.IRCF = 0b110;        // 4MHz
    CMCON0bits.CM = 0b111;          // Disable comparator
    ANSEL = 0b01011000;             // Prescaler 1/16 (101, TAD = 4탎 => 44탎 conversion delay), AN3 (GP4))
    TRISIO = 0b00011000;
    GPIO = 0;
    ADCON0 = 0b10001100;            //ADFM right, cvref = VDD, AN3 (GP4)
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    return;
}

char *do_BDC (unsigned int decode, char pos) {
    static char array[4];
    memset (array, 0, sizeof(array));
    char tp = 0;
    char ascii = 0;
    char i = 0;
    
    for (unsigned int k = 1000; k > 0 ; k /= 10, i++) {
        tp = (char)(decode / k);         // Compute BDC - first digit
        ascii = tp + 48;                 // Convert number to ASCII
        array[i] = ascii;       
        decode %= k;
    }
    return array;
}


void main(void) {
    
    struct holder buf;    
    pic_Init();
    init_Lcd();
    ADCON0bits.ADON = 1;        // TACQ = initial acquisition time = 
    __delay_us(5);              // Amplifier Settling Time + Hold Capacitor Charging Time + T� coeff
    pos_Lcd(1, 1);
    for (unsigned char i = 0; i < sizeof (myText) - 1; i++) {
        write_Nibbles(myText[i], DISP);
    }
    
    while (true) {
        result = 0;
        convert = 0;
        unit = 0;
        memset (buf.arr, 0, sizeof(buf.arr));                   // Wipe clean buf 
        ADCFLAG = 0;
        PIE1bits.ADIE = 1;                                      // Enable ADC interrupt and run a conversion
        ADCON0bits.GO_DONE = 1;
        while(!ADCFLAG);
        convert = (__uint24)result;                             // 8bits 킸ontroller do not like floating point
        convert = ((convert << 2) + convert) * 1000 ;           // result * 5 then by 1000 to get result in mV
        unit = (unsigned int)(convert >> 10);                   // 10 bits resolution ; divide convert by 1024
        char *bdc = do_BDC(unit, 3);
        // buf = (struct holder){ .arr = {*bdc++, 46, *bdc++, *bdc++, *bdc++, 0}};
        buf = (struct holder){ .arr = {bdc[0], 46, bdc[1], bdc[2], bdc[3], 0}};
        _nop();
        pos_Lcd(1, 8);
        for(char i = 0 ; i < 5 ; i++) {
            write_Nibbles(buf.arr[i], DISP);
        }     
        __delay_ms(200);
    }

    return;
}
