/*
 * File:   i2c_bb.c
 * Author: Christian de Balorre
 * Adapted from Microchip AN1488 Bit Banging I2C on PIC Mid-Range MCU
 * See also : davbucci.chez-alice.fr/index.php?argument=elettronica/pic_i2c/pic_i2c.inc
 * Created on 27 octobre 2024, 13:22
 */


#include <xc.h>
#include "i2c_bb.h"


// Start : SDA HIGH to LOW while Clock HIGH
void i2c_start(void) {
    SCL_TRIS = 0;
    SDA_TRIS = 1;                   // SDA & Clock high
    SCL = 1;
    __delay_us(tSU_STA);            // tSU:STA = 4.7탎 (setup for repeated START), SDA and clock HIGH
    SDA_TRIS = 0;                   // SDA = OUTPUT
    SDA = 0;                        // SDA LOW
    __delay_us(tHD_STA);            // tHD:STA = 4.7탎 (hold for [repeated] START), while clock HIGH
    SCL = 0;                        // Leave SDA and clock low for next bit transmission
    __delay_us(tLOW);
    
    return;
}

// Stop : SDA LOW to HIGH while Clock HIGH
void i2c_stop(void) {
    SCL = 0;                        // Clock LOW
    SDA_TRIS = 0;                   // SDA = output
    SDA = 0;                        // SDA LOW
    __delay_us(tLOW);               // tLOW = 4.7탎, clock LOW period 
    SCL = 1;                        // Clock HIGH
    __delay_us(tSU_STO);            // tSU:STO = 4탎, before SDA HIGH
    SDA_TRIS = 1;                   // SDA HIGH
    __delay_us(tBUF);               // tBUF = 4.7탎, SDA and clock HIGH, I2C bus free
    SCL_TRIS = 1;                   // Release clock line
    
    return;
}

void i2c_bit_out(unsigned char data) {
    SCL = 0;                        // Clock low
    SDA_TRIS = 0;                   // SDA = output
    SDA = data;                     // Output the LSB
    __delay_us(tSU_DAT);            // tSU:DAT = 250ns, before clocking
    SCL = 1;                        // Clock bit out
    __delay_us(tHIGH);              // tHIGH = 4탎, clock HIGH period
    SCL = 0;                        // Clock LOW for next bit
    SDA = 0;
    
    return;
}

void i2c_bit_in(unsigned char *data) {
    SCL = 0;
    SDA_TRIS = 1;                   // SDA as high Z
    __delay_us(tSU_DAT);            // tSU:DAT = 250ns, before clock
    SCL = 1;                        // Clock bit in
    __delay_us(tHIGH);              // tHIGH = 4탎, clock HIGH period
    *data |= SDA;                   // Bit = SDA state
    SCL = 0;                        // Clock LOW for next bit
    
    return;
}

unsigned char i2c_wr(unsigned char data) {
    volatile unsigned char temp = data; // Dirty trick to force compiler to use data var
    unsigned char ack = 0;            // Ack bit status from slave
    SCL = 0;
    SDA_TRIS = 0;
    SDA = 0;
    __delay_us(1);
    for (unsigned char i = 0; i < 8; i++) {
        // Code optimization to speed up dirty compiler code : 17탎 -> 3-4탎
        // data = (unsigned char)(data << 1);  // MSB bit shifted into CARRY bit of STATUS                        
        asm("rlf i2c_wr@temp, f");      // Compiler uses dirty x2 algo...
        //if (STATUSbits.C)SDA = 1;
        asm("btfss 3, 0");              // Test CARRY bit
        asm("goto $ + 3");              // Skip BANKSEL + bsf SDA code if CARRY = 0
        SDA = 1;
        __delay_us(tSU_DAT);
        SCL = 1;
        __delay_us(tHIGH);
        SCL = 0;
        SDA = 0;
    }
    i2c_bit_in(&ack);                 // ACK bit from slave ; SDA back HIGH Z
    
    return ack;
}

unsigned char i2c_rd(unsigned char ack) {
    unsigned char ret = 0;
    SDA_TRIS = 1;                   // SDA HIGH Z
    SCL = 0;                        // Clock LOW, wait device signal   
    for (unsigned char i = 0; i < 8; i++) {
        ret = (unsigned char) (ret << 1);
        __delay_us(tSU_DAT);        // tSU:DAT = 250ns, before clocking
        SCL = 1;                    // Clock hHIGH for reception
        ret |= SDA;                 // Read bit from slave
        __delay_us(tHIGHRD);
        SCL = 0;
        __delay_us(tLOWRD);         // tLOW = 4.7탎
    }    
    i2c_bit_out(ack);               // Output ACK|NAK bit
    
    return ret;
}