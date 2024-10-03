/*
 * File:   main.c
 * Author: Christian de Balorre
 * Software UART
 * See : https://ww1.microchip.com/downloads/en/Appnotes/AVR274.pdf
 * @4Mhz : 9600bps => 1/9600 = 104,167µs = 104 tcy
 * Created on 15 mars 2023, 22:29
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
#define FCYC FOSC/4             // Instruction freq - one instruction consumes 4 clocks
#define TCYC 1/FCYC             // Instruction period (1µs for default 4MHz FOSC)

#define BAUDR 9600              // Baud rate = 9600 bps -> 1 / 9600 = 104µs/bit
#define BITDY  103              // Bit delay 104
#define DATABITS 8
#define TX GPIObits.GP1         // UART TX pin
#define RX GPIObits.GP2         // UART RX pin - on external interrupt pin

// Boiler platee functions
void __interrupt() isr(void);
void PicInit(void);
// Project functions
void tx_UART(void);
void send_Txt(const unsigned char *, unsigned char);
void set_Led(unsigned char);
void play_led(void);


//Global variables
unsigned const char myText[] = "Enter LED number : ";
unsigned const char rn[] = "\r\n";      // Char line feed
unsigned const char test = 0x55;        // Oscilloscope test char 0b01010101
static volatile unsigned char txBuffer __at(0x70); //Force in shared ram - easier to debug
static volatile unsigned char rxBuffer __at(0x71);
static volatile __bit TXFLAG = 0;
static volatile __bit RXFLAG = 0;
static volatile __bit RXREADY = 0;

// Interrupt function

void __interrupt() isr() {
    if (INTCONbits.INTE && INTCONbits.INTF) {
        T2CONbits.TMR2ON = 1;           // Fire timer
        RXREADY = 1;
        INTCONbits.INTF = 0;
        INTCONbits.INTE = 0;
        
        return;
    }
}

void PicInit(void) {
    OSCCONbits.IRCF = 0b110;        // Select 4MHz clock
    CMCON0 = 0b00000111;            // Comparator OFF
    VRCONbits.VREN = 0;             // Disable comparator internal VREF
    ANSEL = 0x00;                   // Force all GPIO pins to digital mode
    TRISIO = 0b00001100;            // GPIO 0/1/4/5 outputs (0)
    GPIO &= 0x0C;                   // All outputs driving low
    TMR2 = 0;
    PR2 = BITDY;
    INTCONbits.GIE = 1;             // Enable global interrupt
    INTCONbits.INTE = 0;            // External interrupt on GP2
    OPTION_REGbits.INTEDG = 0;      // Interrupt on falling edge of GP2 - Start bit goes low
    
    return;
}
void rx_UART() {
    if (TXFLAG){return;}                 // We could add a special code informing line not ready
    while (!PIR1bits.TMR2IF);            // This while() uses 5us per loop @4Mhz
    _delay(BITDY >> 1);                  // Add half a delay and start capturing bits
    TMR2 = 0;                            // Then reset timer, begin transmit
    PIR1bits.TMR2IF = 0;
    for (unsigned char i = 1; i; i = (unsigned char) (i << 1)) {
            if(RX == 1) { (rxBuffer |= i) ;}    // Serial data are transmitted LSB first
            while (!PIR1bits.TMR2IF);
            PIR1bits.TMR2IF = 0;            
    }
    // Check status of stop bit ; we suppose all went well
    _delay(BITDY >> 1); 
    if (RX == 1) {
        // House keeping
        RXFLAG = 0;
        T2CONbits.TMR2ON = 0;
        TMR2 = 0;            
    }
        return;
}

void tx_UART() {
    if (RXFLAG){return;}
    TXFLAG = 1;
    TX = 0;                             // Send Start Bit
    T2CONbits.TMR2ON = 1;               // Fire timer
    while (!PIR1bits.TMR2IF);           // This while() uses 5us per loop @4Mhz
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
    txBuffer = 0x00;
    
    return;
}

void play_led() {
    INTCONbits.INTF = 0;            // Clear flag, activate ext interrupt for RX data
    INTCONbits.INTE = 1;
    RXFLAG = 1;
    while (!RXREADY);               // Wait until char is received RXREADY set in ISR
    rx_UART();
    txBuffer = rxBuffer;            // Echo char to user terminal
    tx_UART();
    send_Txt(rn, sizeof (rn));
    set_Led(rxBuffer);
    // House keeping
    rxBuffer = 0x00;
    txBuffer = 0x00;
    RXREADY = 0;
    __delay_ms(500);
    
    return;
}

void set_Led(unsigned char value) {
    switch (value) {                    // Read serial input char and turn appropriate LED on
        case 0x30:                      // 0 is ASCII 0x30
            GPIObits.GP0 ^= 1;
            break;
        case 0x34:
            GPIObits.GP4 ^= 1;
            break;
        case 0x35:
            GPIObits.GP5 ^= 1;
            break;
        default:                        // Switch off all leds
            GPIO &= 0b00001110;
    }
    return;
}


void main(void) {
    PicInit();
    txBuffer = 0x00;
    rxBuffer = 0x00;
    TX = 1;
    __delay_ms(5);
    
    while (true) {
        // send_Txt(&test, sizeof (test));     // Send test char 0x55
        send_Txt(myText, sizeof(myText));        
        play_led();
        __delay_ms(2);
        
     }
}