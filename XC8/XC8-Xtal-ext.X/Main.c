/*
 * File:   main.c
 * Author: Christian de Balorre
 * External crystal 16Mhz crystal oscillator testing
 * Automatic external/ internal clock toggling in case of ext clock failure (FCMEN)
 * Software switchover enabled (IESO)
 * Created on October 10, 2024 21:44
 */

// OCTETS DE CONFIGURATION (FUSIBLES)
// HS gain for external oscillator     : FOSC = 010  
// Watchdog Timer Enable bit           : disabled (WDTE)
// Power-up Timer Enable bit           : enabled (PWRT)
// MCLR Reset Pin Function Select bit  : disabled (MCLRE)
// Code Protection bit                 : disabled (CP))
// Data Code Protection bit            : disabled (CPD)
// Brown Out Detect                    : disabled (BOREN)
// Internal External Switchover bit    : enabled (IESO)
// Fail-Safe Clock Monitor Enabled bit : enaabled (FCMEN)
#pragma config FOSC = HS, WDTE = OFF, PWRTE = ON, MCLRE = OFF 
#pragma config CP = OFF, CPD = OFF, BOREN = OFF, IESO = ON, FCMEN = ON

#include <xc.h>
#include <stdbool.h>

#define _XTAL_FREQ  16000000    // 16Mhz, external quartz
#define GPIO_MASK 0b11111000    // For GPIO0, 1, 2

__bit FAILFLAG;
unsigned char counter;
unsigned char retry;

// Declare functions
void __interrupt() isr(void);
void PicInit(void);

void __interrupt() isr() {
    if (PIE1bits.OSFIE && PIR1bits.OSFIF) {
        FAILFLAG = 1;
        PIE1bits.OSFIE = 0;     // Disable oscillator fail safe interrupt
        OSCCONbits.SCS = 1;     // Use internal clock as set in OSCCON.IRCF     
    }        
    return;
}

void PicInit (void) {
    OSCCONbits.IRCF = 0b111;    // Internal oscillator 8MHz clock
    TRISIO = 0b00111000;        // GPIO3 inputs (1), 2/1/0 outputs (0), GPPIO4-5 always read 1 in HS mode
    GPIO = 0;                   // Output driving low for xtal testing
    ANSEL = 0;                  // Force all GPIO pins to digital mode
    CMCON0 = 0b111;             // Comparator OFF
    WDTCONbits.WDTPS = 0b0110;  // Set watchdog timer to 66 ms (1/31000) * 2048
    // Set up peripheral interrupt
    PIE1bits.OSFIE = 1;
}

void main(void) {

 PicInit();
 INTCONbits.PEIE = 1;
 INTCONbits.GIE = 1;
 counter = 25;
 
 while(true){
    if (FAILFLAG) {             // FAILFLAG set by interrupt OSFIE, device on internal clock
        GPIO &= GPIO_MASK;      // Blink alarm red LED
        while (counter){
            GPIObits.GP2 ^= 1;  // Toggle pin status with XOR
            __delay_ms(50);
            counter--;
        }
    GPIO &= GPIO_MASK;          // Try to recover from crystal failure
    for (unsigned char i = 0 ; i < 10 ; i++) {
    GPIObits.GP1 ^= 1;
    __delay_ms(200);
    }
    retry ++;
    if (retry > 4 ) {               // After 5 soft retries, either sleep or reset device
     WDTCONbits.SWDTEN = 1;         // Sleep / reset timeout per watchdog timer
     retry = 0;
     SLEEP();                       // A sleep disconnects external/internal oscillator, and use internal 31kHz clock
     //RESET()
    }        
    OSCCONbits.SCS = 0;             // Try to soft revive external oscillator
    __delay_ms(10);
    counter = 25;
    }

    if (OSCCONbits.OSTS) {          // 1 = device running from external clock ; 0 = internal clock
        FAILFLAG = 0;               // Clear fail conditions
        PIR1bits.OSFIF = 0;
        PIE1bits.OSFIE = 1;
        GPIO &= GPIO_MASK;
        __delay_ms(800);
        GPIObits.GP0 = 1;
        __delay_ms(800);
    }
    if (!OSCCONbits.OSTS && !FAILFLAG){     // Device running from internal clock, no fail condition       
        GPIO &= GPIO_MASK;
        __delay_ms(200);
        GPIObits.GP1 = 1;
        __delay_ms(200);       
    }    
    NOP();
 }   
return;
}   //loop forever   