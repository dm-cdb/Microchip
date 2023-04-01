/*
 * File:   main.c
 * Author: Christian de Balorre
 * Driving an LCD with an HC595 shift register
 * From https://www.electronics-lab.com/project/3-wire-serial-lcd-using-a-shift-register/
 * Also see http://romanblack.com/shift1.htm
 * Created on March 31th 2023, 17:00
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

#define _XTAL_FREQ  4000000     // Needed for built-in  __delay_us(x) & __delay_ms(x)
#define FOSC _XTAL_FREQ         // Fosc in Hz - for information, not used in this project
#define FCYC FOSC/4             // Instruction freq = 1MHz for 4MHz FOSC
#define TCYC 1/FCYC             // Instruction period = 1µs for default 4MHz FOSC

#define CLOCK GPIObits.GP0
#define ENLCD GPIObits.GP1  // Data transfered from high to low transition
#define DATA  GPIObits.GP5

#define CMD 0
#define DISP 1

// LCD commands
const unsigned char lcd4bits = 0x20; // 0010 0000
const unsigned char lcdClear = 0x01; // 0000 0001
const unsigned char lcdHome = 0x02; // 0000 0010
const unsigned char lcdCon = 0x0E; // 0000 1110 disp on, underline
const unsigned char lcdFunc = 0x28; // 0010 1000 4bits,2 lines, 5x7 size

const unsigned char myText1[] = "3 pins LCD display";
const unsigned char myText2[] = "With HC595";

// Declare functions
void __interrupt() isr(void);
void init_Pic(void);
// Project functions
void do_Lcden(void);
void do_Shiftr(unsigned char);
void init_Lcd(void);
void pos_Lcd(unsigned char, unsigned char);
void reset_Lcd(void);
void send_Nibbles(unsigned char, bool);
void send_Text(const unsigned char*, unsigned char);

// Interrupt function

void __interrupt() isr() {
    // only process Timer0-triggered interrupts
    if (INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        __nop();
        __nop();
        INTCONbits.TMR0IF = 0;
    }
}
// Initialize PIC

void init_Pic() {
    OSCCONbits.IRCF = 0b110; // Select 4MHz clock
    CMCON0 = 0b111; // Comparator OFF
    ANSEL = 0; // Force all GPIO pins to digital mode
    TRISIO = 0b00011100; // GP3-2-4 inputs (1), 1/2/5 outputs (0)
    GPIO = 0; // Output driving low for xtal testing
    return;
}

// Send 2 x nibbles to shift register

void send_Nibbles(unsigned char data, bool rs) {
    char lowB, hiB, mask = 0x10;
    hiB = data >> 4; // Mask lower bits
    lowB = data & 0x0F; // Mask higher bit
    if (rs) { //If display mode, set 5th bits (RS on Q4) to 1
        hiB = hiB | mask;
        lowB = lowB | mask;
    }
    do_Shiftr(hiB);
    do_Lcden();
    do_Shiftr(lowB);
    do_Lcden();

    return;
}

void do_Shiftr(unsigned char value) {
    for (char i = 0; i < 8; i++) {
        DATA = (value & 0x80) ? 1 : 0; // Compare value AND 10000000 - HIGH/LOW = true/false
        value <<= 1;
        __delay_us(10);
        CLOCK = 1;
        __delay_us(5);
        CLOCK = 0;
        __delay_us(5);
    }
    CLOCK = 1; // LCD spec allow ST and SH clock to be tied together
    __delay_us(5); // However it means ST is one clock behind SH. See datasheet p3
    CLOCK = 0;
    return;
}

void do_Lcden() {
    __delay_us(5);
    ENLCD = 1; //Enable high time > 450ns
    __delay_us(15);
    ENLCD = 0; //Enable low time > 500ns
    __delay_us(15);

    return;
}

// send whole text

void send_Text(const unsigned char* s, unsigned char n) {
    for (unsigned char i = 0; i < n - 1; i++) {
        send_Nibbles(s[i], DISP);
        __delay_ms(5);
    }
    return;
}

// Position cursor on screen

void pos_Lcd(unsigned char x, unsigned char y) {
    char pos = 127 + y; //Line 1 starts at 0x00, line 2 at 0x40 -cmd format 1xxx xxxx
    if (x == 2) pos += 64;
    send_Nibbles(pos, CMD);
    __delay_ms(2);

    return;
}

// run time lcd init

void init_Lcd() {
    __delay_ms(100); // Wait LCD to init
    do_Shiftr(0x0); // Purge shift register
    __delay_us(5);
    do_Shiftr(0x02); // Brute force LCD to 4-bits mode 
    do_Lcden();
    __delay_us(50);
    // send_Nibbles(0x20, CMD);      // 2nd method to init LCD to 4-bits mmode
    //__delay_us(50);
    send_Nibbles(lcdCon, CMD); // Display ON > 40µms
    __delay_us(50);
    send_Nibbles(lcdFunc, CMD); // 2 lines mode, 5x7 format > 40µms
    __delay_us(50);

    return;
}

// reinit LCD

void reset_Lcd() {
    send_Nibbles(lcdClear, CMD); // Clear delay 82µs<n<1,64ms
    __delay_ms(2);
    send_Nibbles(lcdHome, CMD); // Home delay 40µs<n<1,64ms
    __delay_ms(2);

    return;
}

void main(void) {
    init_Pic();
    init_Lcd();
    while (true) {
        pos_Lcd(1, 2);
        for (char i = 0; i < sizeof (myText1) - 1; i++) {
            send_Nibbles(myText1[i], DISP);
        }
        __delay_ms(1000);
        pos_Lcd(2, 1);
        send_Text(myText2, sizeof (myText2));
        __delay_ms(3000);
        reset_Lcd();
    }
}
