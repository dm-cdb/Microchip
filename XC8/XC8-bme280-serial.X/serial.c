/*
 * File:   serial.c
 * Author: Christian de Balorre
 * Serial bitbang program for PIC12f683
 * RX must be GP2 / External Interrupt
 * Created on 2 novembre 2024, 14:24
 */

#include <xc.h>
#include "serial.h"

static volatile unsigned char txBuffer __at(0x70); //Force in shared ram - easier to debug
static volatile unsigned char rxBuffer __at(0x71);
static volatile __bit TXFLAG = 0;
static volatile __bit RXFLAG = 0;
static volatile __bit RXREADY = 0;

void init_UART(void) {
    CMCON0 = 0b00000111;                // Comparator OFF
    OPTION_REGbits.INTEDG = 0;          // Interrupt on falling edge of GP2 - UART start bit goes low
    TMR2 = 0;
    PR2 = BITDY;
    TX = 1;
}

unsigned char rx_UART() {
    if (TXFLAG){return 24;}                 // Line not ready - for bidir rx/tx line - 24 = CAN char
    INTCONbits.INTF =0;
    T2CONbits.TMR2ON = 0;    
    while(!INTCONbits.INTF);
    RXFLAG = 1;
    T2CONbits.TMR2ON = 1;
    while (!PIR1bits.TMR2IF);            // This while() uses 5us per loop @4Mhz
    _delay(BITDY >> 1);                  // Add half a delay and start capturing bits
    TMR2 = 0;                            // Then reset timer, begin rx
    PIR1bits.TMR2IF = 0;
    // Process incoming byte
    for (unsigned char i = 1; i; i = (unsigned char) (i << 1)) {
            if(RX == 1) { (rxBuffer |= i) ;}    // Serial data are transmitted LSB first
            while (!PIR1bits.TMR2IF);           // Usual 104µs delay
            PIR1bits.TMR2IF = 0;            
    }
    // Check status of stop bit ; we suppose all went well
    _delay(BITDY >> 1); 
    if (RX == 1) {
        // House keeping
        RXFLAG = 0;
        T2CONbits.TMR2ON = 0;
        TMR2 = 0;
        return rxBuffer;
    }
    
    return 24;        
}

void tx_UART() {
    if (RXFLAG){return;}
    TXFLAG = 1;
    TMR2 = 0;
    TX = 0;                             // Send Start Bit
    T2CONbits.TMR2ON = 1;               // Fire timer
    while (!PIR1bits.TMR2IF);           // Put line LOW for bit delay before transmitting
    PIR1bits.TMR2IF = 0;
    for (unsigned char i = 1; i; i = (unsigned char) (i << 1)) {
        (txBuffer & i) ? (TX = 1) : (TX = 0);
        while (!PIR1bits.TMR2IF);
        PIR1bits.TMR2IF = 0;
    }
    TX = 1;                         // Send Stop bit
    while (!PIR1bits.TMR2IF);
    PIR1bits.TMR2IF = 0;
    // House keeping
    txBuffer = 0x00;
    T2CONbits.TMR2ON = 0;           // Stop &  reset timer
    TMR2 = 0;
    TXFLAG = 0;
    
    return;
}

void send_Txt(const unsigned char * s, unsigned char k) {
    for (unsigned char i = 0; i < k; i++) {
        txBuffer = s[i];
        tx_UART();
    }
     
    return;
}