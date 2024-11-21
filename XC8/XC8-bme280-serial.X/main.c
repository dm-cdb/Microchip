/*
 * File:   main.c
 * Author: Christian de Balorre
 * D:\Temp\MPLAB-Proj\XC8-TEMPLATE.X\main.c
 * Created on November 6th 2024, 21:44
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
#include "bme280.h"

// If necessary, change simulator istruction frequency Fcyc - default 1MHz
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000
#endif

#define FOSC _XTAL_FREQ         // Fosc in Hz - for information, not used in this project
#define TOSC 1/FOSC             // Period = 0,250 µs
#define FCYC FOSC/4             // Instruction freq - one instruction consumes 4 clocks
#define TCYC 1/FCYC             // Instruction period (1µs for default 4MHz FOSC)


unsigned char temperature[] = "T = +00:00C";
const  unsigned char device[] = "Device ";
const unsigned char id[] = "BME280 online";
const unsigned char unknown[] = "unknown";
const unsigned char notresp[] = "not respondng";
unsigned char rn[] = "\r\n";      // Char line feed

//Declare functions
void pic_init(void);

void pic_init() {
    OSCCONbits.IRCF = 0b111;    // Select 8MHz clock
    CMCON0bits.CM = 0b111;      // Comparator OFF
    ANSEL = 0x00;               // Force all GPIO pins to digital mode
    VRCONbits.VREN = 0;         // CVREF circuit down
    TRISIO = 0b00101111;        // GPIO 4 outputs (0) -> serial TX
    GPIO = 0x00;                // Output driving low
 
    return;
}

void main(void) {
    
    pic_init();
    init_UART();    
    int T;    
    unsigned char ack, prov, t_i, t_d;
    
    send_Txt(device, sizeof(device));
    do {
        if ((ack = bme280_hello(ADDR))) {       // Device should respond with ack LOW = 0
            
            send_Txt(notresp, sizeof(notresp));
            send_Txt(rn, sizeof(rn));
            __delay_ms(3000);
        }        
    } while (ack);
    
    prov = bme280_get_id(ADDR);
    if (prov == 0x60) {
        send_Txt(id, sizeof(id));
    }
    else {
        send_Txt(unknown, sizeof(unknown));
    }
    send_Txt(rn, sizeof(rn));
    
    bme280_init(ADDR);  // Normal mode, standby 125ms
    __delay_ms(1000);    
    
     
    while (true) {
        T = 0;
        // Get compensated temperature value in form int:int ; ie 5123 -> 51.23°C
        bme280_comp_temp(ADDR, &T);
        unsigned char sign = 0x2b;        
        if ((T & 0x8000)) { // negative value ?
            T = (~T) - 1;   // 2 complements
            sign = 0x2d;            
        }
        t_i = (unsigned char)(T/100);
        t_d = (unsigned char) (T%100);
        
                  // Ascii "+"
        temperature[4] = sign;
        temperature[5] = t_i / 10 + 0x30;
        temperature[6] = t_i % 10 + 0x30;
        temperature[8] = t_d / 10 + 0x30;
        temperature[9] = t_d % 10 + 0x30;
        
        send_Txt(temperature, sizeof(temperature));    
        send_Txt(rn, sizeof(rn));
        __delay_ms(3000);
        NOP();   
    }

    return;
}