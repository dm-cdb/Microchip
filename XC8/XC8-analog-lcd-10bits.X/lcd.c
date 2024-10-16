#include <xc.h>
#include <stdbool.h>
#include "lcd.h"

#define _XTAL_FREQ 4000000      // Used by built-in delay()

const unsigned char lcdClear = 0x01;
const unsigned char lcdHome  = 0x02;
const unsigned char lcdDisp  = 0x0e; //0b00001110 - on, underline cursor
const unsigned char lcdFunc  = 0x28; //0b00101000 - 4 bits, 2 line, 5x7 
const unsigned char lcdShift = 0x1C; //0b00011100


void init_Lcd() {
    __delay_ms(100);
    do_Shiftr(0x00);     // Clear shift register
     __delay_ms(10);
    do_Shiftr(0x02);     // Brute force LCD 4 bits
    write_Lcden();
     __delay_us(50);
    write_Nibbles(lcdDisp, CMD);
    __delay_us(50);
    write_Nibbles(lcdFunc, CMD);
    __delay_us(50);

    return;
}

void pos_Lcd(unsigned char x, unsigned char y) {
    unsigned char pos;
    (x == 1) ? (pos = 127 + y) : (pos = 191 + y);
    write_Nibbles(pos, CMD);
    __delay_us(50);
    return;
}

void reset_Lcd() {
    write_Nibbles(lcdClear, CMD);
    __delay_ms(3);
    write_Nibbles(lcdHome, CMD);
    __delay_ms(3);
    return;
}

void write_Nibbles(unsigned char value, bool rs) {
    unsigned char hiB, loB, temp;
    hiB = value >> 4;
    loB = value & 0x0f;
    if (rs) {
        hiB = hiB | 0x10;
        loB = loB | 0x10;
    }
    do_Shiftr(hiB);
    write_Lcden();
    do_Shiftr(loB);
    write_Lcden();
    return;
}

void do_Shiftr(unsigned char value) {
    for (unsigned char i = 0; i < 8; i++) {
        (value & 0x80) ? (DATA = 1) : (DATA = 0);
        value <<= 1;
        __delay_us(10);
        CLOCK = 1;
        __delay_us(5);
        CLOCK = 0;
        __delay_us(5);
    }
    __delay_us(10);
    return;
}

void write_Lcden() {
    __delay_us(10);
    ENLCD = 1;
    __delay_us(5);
    ENLCD = 0;
    __delay_us(5);
    return;
}