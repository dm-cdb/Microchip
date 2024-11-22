/*
 * File:   main.c
 * Author: Christian de Balorre
 *
 * Created on 2 novembre 2024, 23:59
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
#include "common.h"
#include "i2c_bb.h"
#include "serial.h"
#include "mcp9808.h"

#ifndef _XTAL_FREQ
#define _XTAL_FREQ  4000000     // Needed for built-in  __delay_us(x) & __delay_ms(x)
#endif

#define FOSC _XTAL_FREQ         // Fosc in Hz - for information, not used in this project
#define TOSC 1/FOSC             // Period = 0,250 µs @ 4MHz
#define FCYC FOSC/4             // Instruction freq - one instruction consumes 4 clocks
#define TCYC 1/FCYC             // Instruction period = 1µs @ 4MHz FOSC

const unsigned char msg1[] = "Init...";
unsigned char temperature[] = "Temp = +000.00 C";
unsigned char sign;
const unsigned char alert[] = "Alert ";
const unsigned char notresp[] = "device not respondng";
const unsigned char upper[] = "UP !";
const unsigned char lower[] = "LOW !";
const unsigned char crit[] = "Crit !";
unsigned const char rn[] = "\r\n";      // Char line feed

void pic_init(void);
void send_alert(unsigned char);

void pic_init() {
    OSCCONbits.IRCF = 0b111;    // Select 8MHz clock
    CMCON0bits.CM = 0b111;      // Comparator OFF
    VRCONbits.VREN = 0;         // Disable comparator internal VREF
    ANSEL = 0x00;               // Force all GPIO pins to digital mode
    TRISIO = 0b00101111;        // Serial TX GPIO 2 outputs (0)
    GPIO = 0x00;                // Output driving low
    OPTION_REG = 0b11010001;    // Set prescaler 1:4 ; prescaler to TMR0
    INTCONbits.T0IF = 0;        // TOIF<2> set at 0
    return;
}

void send_alert(unsigned char code){
    send_Txt(alert, sizeof(alert));
    switch (code) {
        case 0x10:
            send_Txt(notresp, sizeof(notresp));
            break;
        case 0x20:
            send_Txt(lower, sizeof(lower));
            break;
        case 0x40:
            send_Txt(upper, sizeof(upper));
            break;
        case 0x80:
            send_Txt(crit, sizeof(crit));
            break;
    }
    send_Txt(rn, sizeof(rn));
    return;
}
void main(void) {
    pic_init();
    init_UART();
    unsigned char ack;
    struct temp_res myTemp;
    struct temp_data myData;
    
    do {
        if ((ack = mcp9808_hello(ADDR))) {
            send_alert(0x10);
            __delay_ms(3000);
        }        
    } while (ack);    
    
    send_Txt(msg1, sizeof(msg1));
    send_Txt(rn, sizeof(rn));
    __delay_ms(1000);
    
    // Config register : hyst = 1.5° 0b00000010 ; Alarm control on 0b00001000 = 0x0208
    myData.datah = 0x02;
    myData.datal = 0x08;
    mcp9808_wr_int(ADDR, CONFIG, &myData);
     __delay_us(4);
    
    // Set upper = 28° 0b00000001 0b11000000
    myData.datah = 0x01;
    myData.datal = 0xC0;
    mcp9808_wr_int(ADDR, ALERT_UPPER, &myData);
    __delay_us(4);
    
    // Set critical = 35° 0b00000010 0b00110000
    myData.datah = 0x02;
    myData.datal = 0x30;
    mcp9808_wr_int(ADDR, ALERT_CRIT, &myData);
    __delay_us(4);
    
    // Read upperT register : should display 1 a
    myData.datah = 0;
    myData.datal = 0;
    mcp9808_rd_int(ADDR, ALERT_UPPER, &myData);
        
    send_Txt(&myData.datah, sizeof(myData.datah));
    send_Txt(rn, sizeof(rn));
    send_Txt(&myData.datal, sizeof(myData.datal));
    send_Txt(rn, sizeof(rn));
        
    while (true){
        // Function populates sensor data at struct myTemp address
        mcp9808_get_temp(&myTemp);
        // Handle alert flag
        switch (myTemp.alert) {
            case 0x00:
                break;
            case 0x20:
                send_alert(0x20);
                break;
            case 0x40:
                send_alert(0x40);
                break;
            case 0x80:
                send_alert(0x80);
                break;
        }
        // Decimal = 1/16 = 0.0625 per step
        // Trim decimal part for simplicity : div per 6 ; add one for each 4 div to compensate
        unsigned char mod = myTemp.t_dec >> 2;
        myTemp.t_dec = (myTemp.t_dec * 6) + mod;
        // Display data in human reading
        temperature[7] = myTemp.sign;
        temperature[8] = (myTemp.t_ing / 100) + 0x30;
        myTemp.t_ing = myTemp.t_ing % 100;
        temperature[9] = (myTemp.t_ing / 10) + 0x30 ;
        myTemp.t_ing = myTemp.t_ing % 10;
        temperature[10] = myTemp.t_ing + 0x30;
        temperature[12] = (myTemp.t_dec / 10) + 0x30;
        myTemp.t_dec = myTemp.t_dec % 10;
        temperature[13] = myTemp.t_dec + 0x30;
            
        send_Txt(temperature, sizeof(temperature));
        send_Txt(rn, sizeof(rn));
        
        __delay_ms(5000);
    }
       
    return;
}
