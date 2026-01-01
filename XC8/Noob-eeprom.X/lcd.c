#include <xc.h>
#include <stdbool.h>
#include "lcd.h"

#define _XTAL_FREQ 4000000      // Used by built-in delay()

//LCD commands
const unsigned char lcd4bits = 0x20;        // 0010 0000
const unsigned char lcdClear = 0x01;        // 0000 0001
const unsigned char lcdHome  = 0x02;        // 0000 0010
const unsigned char lcdCon   = 0x0e;        // 0000 1110 disp on, underline
const unsigned char lcdFunc  = 0x28;        // 0010 1000 4bits,2 lines, 5x7 size


// Send 2 x nibbles to shift register
void send_Nibbles(unsigned char data, bool rs) {
    char lowB, hiB, mask = 0x10;
    hiB = data >> 4;
    lowB = data & 0x0F;
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
void send_Text(const unsigned char* s, unsigned char n){
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
    __delay_ms(100);                // Wait LCD to init
    do_Shiftr(0x0);                // Purge shift register
    __delay_us(5);
    do_Shiftr(0x02);                // Brute force LCD to 4-bits mode 
    do_Lcden();
    __delay_us(50);
    // send_Nibbles(0x20, CMD);      // 2nd method to init LCD to 4-bits mmode
    //__delay_us(50);
     send_Nibbles(lcdCon, CMD);      // Display ON > 40µms
    __delay_us(50);
    send_Nibbles(lcdFunc, CMD);     // 2 lines mode, 5x7 format > 40µms
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
