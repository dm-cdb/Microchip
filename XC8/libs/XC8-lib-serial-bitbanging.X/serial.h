/* 
 * File:   serial.h
 * Author: Christian de Balorre
 * Serial bitbang program for PIC12f683
 * @4Mhz : 9600bps => 1/9600 = 104,167µs = 104 tcy
 * IMPORTANT : this library uses TMR2 for serial clocking
 * And the external interrupt port GP2 for RX line timing
 * Erase ANSEL, register, disable CMCON0 comparator
 * Set TRISIO TX OUT
 * Set TRISIO RX HIGH Z
 * Created on 2 novembre 2024, 14:19
 */
#include <xc.h>
#include "common.h"

#ifndef SERIAL_H
#define	SERIAL_H

#define BAUDR 9600              // Baud rate = 9600 bps -> 1 / 9600 = 104µs/bit
#define BITDY  (_XTAL_FREQ / 4 / BAUDR)             // Bit delay 104µs @ default 4Mhz
#define DATABITS 8

unsigned char rx_UART(void);
void tx_UART(void);
void init_UART(void);
void send_Txt(const unsigned char *, unsigned char);

#endif	/* SERIAL_H */

