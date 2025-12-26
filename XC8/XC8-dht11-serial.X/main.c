/*
 * File:   main.c
 * Author: Christian de Balorre
 *
 * !!! IMPORTANT : this code will only work with FOSC >= 8MHz !!!
 * DHT11 send 40 bits = 5 bytes
 * byte[0] (integer) = RH ; BYTE[1] (decimal) = 0 : byte[2] (integer) = T°C
 * byte[3] (decimal)
 * Most of DHT11 clones, not from ASAIR/AOSONG will set this byte to 0.
 * On guenine ASAIR parts : the first bit indicates the negative T° when set to 1.
 * This code presently does not check for negative T°. 
 * Created on 21 octobre 2024, 05:02
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

#define _XTAL_FREQ  8000000     // Needed for built-in  __delay_us(x) & __delay_ms(x)
#define FOSC _XTAL_FREQ         // Fosc in Hz - for information, not used in this project
#define TOSC 1/FOSC             // Period = 0,125 µs
#define FCYC FOSC/4             // Instruction freq - one instruction consumes 4 clocks
#define TCYC 1/FCYC             // Instruction period (0,5µs for 8MHz FOSC)

#define BAUDR 9600              // Baud rate = 9600 bps -> 1 / 9600 = 104µs/bit
#define BITDY  208              // Bit delay : 104µs * 2 @ 8MHz
#define DATABITS 8
#define TX GPIObits.GP1         // Serial TX pin

#define DHTPIN GPIObits.GP0
#define DHTDIRPIN TRISIObits.TRISIO0

unsigned const char rn[] = "\r\n"; // Char line feed
unsigned const char errMsg1[] = "Data LOW error";
unsigned const char errMsg2[] = "Data HIGH error";
unsigned const char errMsg3[] = "Sensor not responding";
unsigned const char errMsg4[] = "Checksum error";
static volatile unsigned char txBuffer __at(0x70); //Forced in shared ram - for debug
static volatile __bit TXFLAG;

unsigned char temperature[] = "Temp = 00.0 C  ";
unsigned char humidity[] = "RH   = 00.0 %  ";
unsigned char status;
// data = RH integer byte, RH decimal byte (always 0), T° integer byte, T° decimal byte, checksum
unsigned char data[5] __at(0xa0); //Forced in shared ram - for debug

//Declare functions
void picInit(void);
void tx_UART(void);
void send_Txt(const unsigned char *, unsigned char);
void helo(void); // Start com with sensor
__bit ehlo(void); // Ack start with sensor
char readData(unsigned char*);

void tx_UART() {
    TXFLAG = 1;
    PR2 = BITDY;
    PIR1bits.TMR2IF = 0;
    TX = 0; // Send Start Bit
    TMR2 = 0; // Reset timer2
    while (!PIR1bits.TMR2IF); // Run for BITDY delay
    PIR1bits.TMR2IF = 0;
    for (unsigned char i = 1; i; i = (unsigned char) (i << 1)) {
        (txBuffer & i) ? (TX = 1) : (TX = 0);
        while (!PIR1bits.TMR2IF);
        PIR1bits.TMR2IF = 0;
    }
    TX = 1; // Send Stop bit
    while (!PIR1bits.TMR2IF);
    PIR1bits.TMR2IF = 0;
    // House keeping
    txBuffer = 0x00;
    TMR2 = 0;
    PR2 = 0xFF;
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

void helo() {
    DHTDIRPIN = 0; // Set DHT com pin OUTPUT
    DHTPIN = 0; // Set DHT com line LOW 
    __delay_ms(20); // Helo signal LOW >= 18ms
    DHTDIRPIN = 1; // Set DHT com pin high Z, line HIGH
    __delay_us(20); // DHT ehlo response should come within 20-40µs

}

__bit ehlo() {
    TMR2 = 0;
    while (DHTPIN && TMR2 < 100) { // Check if DHTPIN still HIGH after helo
        if (TMR2 > 99) return 0; // Ehlo timeout - sensor not ready
    }

    TMR2 = 0;
    while (!DHTPIN && TMR2 < 200) { // Ehlo 80µs LOW response 
        if (TMR2 > 199) return 0;
    }

    TMR2 = 0;
    while (DHTPIN && TMR2 < 200) { // Ehlo 80µs HIGH response 
        if (TMR2 > 199) return 0;
    }
    return 1; // Ehlo OK ; data transmission follow
}

char readData(unsigned char data[]) {
    unsigned char Idx = 0;
    while (Idx < 5) {
        unsigned char value = 0;
        for (char i = 0; i < 8; i++) {
            TMR2 = 0;
            while (!DHTPIN) {
                if (TMR2 > 150) return 1; // LOW between bits = 50µs 
            }
            TMR2 = 0;
            while (DHTPIN) { // Assess HIGH duration
                if (TMR2 > 254) return 2;
            }
            if (TMR2 > 80) // bit 0 = 26-28µs - bit 1 = 70µs
                value |= (1 << (7 - i)); // Populate value byte - MSB first
        }
        data[Idx] = value;
        Idx++;
    }

    return 0; // Read byte OK
}

void picInit() {
    OSCCONbits.IRCF = 0b111; // Select 8MHz clock - TOSC 125ns, TCY 0,5µs
    CMCON0bits.CM = 0b111; // Comparator OFF
    VRCONbits.VREN = 0; // Disable comparator internal VREF
    ANSEL = 0x00; // Force all GPIO pins to digital mode
    TRISIO = 0b00111101; // GPIO 1 outputs (0)
    T2CON = 0b00000000; // Timer 2 pre & post scaler 1:1 => 1 tick = 0,5µs @ 8MHz
    INTCONbits.GIE = 0;
    PR2 = 0xFF;
    TX = 1;

    return;
}

void main(void) {
    picInit();
    txBuffer = 0x00;
    T2CONbits.TMR2ON = 1;

    while (true) {
        helo();
        if (ehlo()) { // Test if sensor is alive
            status = readData(data);
            switch (status) {
                case 1:
                    send_Txt(errMsg1, sizeof (errMsg1));
                    send_Txt(rn, sizeof (rn));
                    break;
                case 2:
                    send_Txt(errMsg2, sizeof (errMsg2));
                    send_Txt(rn, sizeof (rn));
                    break;
            }
            if (data[4] == (data[0] + data[1] + data[2] + data[3])) {
                temperature[7] = data[2] / 10 + '0';
                temperature[8] = data[2] % 10 + '0';
                temperature[10] = data[3] + '0';
                humidity[7] = data[0] / 10 + '0';
                humidity[8] = data[0] % 10 + '0';
                humidity[10] = data[1] / 10 + '0';
                send_Txt(temperature, sizeof (temperature));
                send_Txt(rn, sizeof (rn));
                send_Txt(humidity, sizeof (humidity));
                send_Txt(rn, sizeof (rn));
            } else {
                send_Txt(errMsg4, sizeof (errMsg4));
                send_Txt(rn, sizeof (rn));
            }
        } else {
            send_Txt(errMsg3, sizeof (errMsg3));
            send_Txt(rn, sizeof (rn));
        }
        //built-in reminder
        __nop();
        __delay_ms(3000);
        __delay_us(1000);
    }
}