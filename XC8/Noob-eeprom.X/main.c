/*
 * File:   main.c
 * Author: cdebalorre
 * EEprom operations
 * Created on 4 mars 2023, 12:08
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
#pragma config FOSC = INTOSCIO, WDTE = OFF, PWRTE = ON, MCLRE = OFF 
#pragma config CP = OFF, CPD = OFF, BOREN = OFF, IESO = OFF, FCMEN = OFF

#include <xc.h>
#include <stdbool.h>
#include "lcd.h"

#define _XTAL_FREQ  4000000     // Needed for built-in  __delay_us(x) & __delay_ms(x)
#define FOSC _XTAL_FREQ         // Fosc in Hz - for information, not used in this project
#define FCYC FOSC/4             // Instruction freq - one instruction consumes 4 clocks
#define TCYC 1/FCYC             // Instruction period (1µs for default 4MHz FOSC)

//Declare global variables
unsigned char B0, B1;
const unsigned char myText1[] = "Writing eeprom...";
const unsigned char myText2[] = "Value read :";
const unsigned char myText3[] = "Erasing @ :";
const unsigned char myText4[] = "That's all";
const unsigned char myText5[] = "Folks !";
__bit EEFLAG;

//Declare functions
void __interrupt() isr(void);
void picInit(void);
void writeEEprom(unsigned char, unsigned char);
unsigned char readEEprom(unsigned char);

// Interrupt function - not used here
void __interrupt() isr() {
    // Process EEPROM-triggered interrupts if enabled
    if (PIE1bits.EEIE && PIR1bits.EEIF) {
        PIR1bits.EEIF = 0;
        EEFLAG = 1;
        return;
    }
}

void writeEEprom(unsigned char addr, unsigned char data) {    
    EEADR = addr;
    EEDAT = data;
    // Write sequence for each byte :
    // Set 0x55, then 0xAA in EECON2, then do  WR.
    EECON2 = 0x55;
    EECON2 = 0xAA;
    // WR is cleared in hardware. The WR bit can only be set, not cleared, in software.
    // An eeprom write takes around 6ms - cf datasheet D122 - TDEW - Erase/Write Cycle Time
    EECON1bits.WR = 1;
    while (!PIR1bits.EEIF);
    //while(EECON1bits.WR); // Alternatively check WR bit = 0
    _nop();
    PIR1bits.EEIF = 0;
}

unsigned char readEEprom(unsigned char addr){
    EEADR = addr;
    //RD is cleared in hardware. The RD bit can only be set, not cleared, in software.
    EECON1bits.RD = 1;
    // The data is available in the next instruction
    return EEDAT;
}

void picInit() {
    OSCCONbits.IRCF = 0b110;    // Select 4MHz clock
    CMCON0 = 0b00000111;        // Comparator OFF
    ANSEL = 0x00;               // Force all GPIO pins to digital mode
    TRISIO = 0b00001100;        // GPIO 5/4/1/0 outputs (0)
    GPIO = 0x00;                // Output driving low

    return;
}

void main(void) {
    picInit();
    init_Lcd();
    send_Text(myText1,sizeof (myText1));
        __delay_ms(50);
    // Write flash : disable interrupt & enable eeprom write
    INTCONbits.GIE = 0;
    EECON1bits.WREN = 1;    
    for (unsigned char i = 0, B0 = 33 ; B0<43 ; B0++){ // Special ASCII characters ; see ASCII table
        writeEEprom(i, B0);     
        pos_Lcd(2,6);
        send_Nibbles(B0, DISP);
        GPIObits.GP4 ^= 1;
        __delay_ms(1000);
        i++;
    }
    GPIObits.GP4 ^= 1;
    EECON1bits.WREN = 0;
    INTCONbits.GIE = 1;
    
    reset_Lcd();
    send_Text(myText2,sizeof (myText2));    
    // Read flash memory
    for (char i = 0 ; i < 10; i++){
        B1 = readEEprom(i); 
        pos_Lcd(2,6);
        send_Nibbles(B1, DISP);
        __delay_ms(1000);
    }
     
    reset_Lcd();
    send_Text(myText3,sizeof (myText3));
    // Erase flash
    INTCONbits.GIE = 0;
    EECON1bits.WREN = 1;    
    for (char i = 0 ; i < 10 ; i++){
        writeEEprom(i, 0xFF);    //Empty flash address = 0xFF
        pos_Lcd(2,6);
        send_Nibbles(i + 48, DISP);
        GPIObits.GP4 ^= 1;
        __delay_ms(1000);
    }
    GPIObits.GP4 ^= 1;
    EECON1bits.WREN = 0;
    INTCONbits.GIE = 1;
    
    while(true){
        reset_Lcd();
        pos_Lcd(1,2);
        send_Text(myText4,sizeof (myText4));
        pos_Lcd(2,5);
        send_Text(myText5,sizeof (myText5));
        __delay_ms(10000);
    }

}
