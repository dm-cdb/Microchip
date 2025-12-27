/*
 * File:   main.c
 * Here we will see five different methods to set and clear a GPIO pin.
 * 
 * Author: Christian de Balorre
 * Created on 10 janvier 2023, 21:44
 */

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
#pragma config FOSC = INTOSCCLK, WDTE = OFF, PWRTE = ON, MCLRE = OFF 
#pragma config CP = OFF, CPD = OFF, BOREN = OFF, IESO = OFF, FCMEN = OFF

#include <xc.h>
#include <stdbool.h>

#define _XTAL_FREQ  4000000

//Functions declaration

void PicInit(void);

// Functions

void PicInit(void) {
    OSCCONbits.IRCF = 0b110; // Select 4MHz clock
    TRISIO = 0b00111000; // GP 5/4/3 inputs (1), 2/1/0 outputs (0)
    ANSEL = 0; // Force all GPIO pins to digital mode
    CMCON0 = 0b111; // Comparator OFF
    return;
}

void main(void) {

    PicInit();
    while (true) {
        // Using defined bitfields (see pic12f683.h)
        GPIObits.GP2 = 1;
        GPIObits.GP1 = 0;
        GPIObits.GP0 = 0;
        __delay_ms(500);

        // Using Direct Bit Names - not really recommended : make sure they do not conflict with TRISIO register
        GP2 = 0;
        GP1 = 1;
        GP0 = 0;
        __delay_ms(500);

        // Using bitwise operation
        GPIO &= ~(0b0000100); // Clear GP2
        GPIO &= ~(0b0000010); // Clear GP1
        GPIO |= 0b00000001; // Set GP0
        __delay_ms(500);

        // Toggle GP0 (clear))
        GPIO ^= 0b0000001;
        __delay_ms(1000);

        // Bit shifting method
        GPIO &= ~(1 << 2); // Clear GP2
        GPIO &= ~(1 << 1); // Clear GP1
        GPIO |= 1; // Set GP0
        __delay_ms(1000);

        // Inline ASM
        asm("bcf GPIO,0"); // clear GP0
        __delay_ms(2000);
        asm("bsf GPIO,0"); // set GP0
        __delay_ms(2000);

    }

    return;

} 
