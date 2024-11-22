/* 
 * File:   common.h
 * Author: cdebalorre
 *
 * Created on 4 novembre 2024, 15:38
 */

#ifndef COMMON_H
#define	COMMON_H

#define _XTAL_FREQ  8000000     // All TU's ; needed for _delay IMPORTANT : set OSCONbits.IRCF accordingly
#define TX GPIObits.GP4         // Serial TU - we don't use RX line
#define ADDR 0x30               // mcp9808 TU - device address 7 bits + 0 (0x18 < 1)
// I2C TU
#define SDA_TRIS  TRISIObits.TRISIO0
#define SCL_TRIS  TRISIObits.TRISIO1
#define SDA       GPIObits.GP0
#define SCL       GPIObits.GP1

#endif
