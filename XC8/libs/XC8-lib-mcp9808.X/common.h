/* 
 * File:   common.h
 * Author: Christian de Balorre
 *
 * Created on 4 novembre 2024, 15:38
 */

#ifndef COMMON_H
#define	COMMON_H

#define _XTAL_FREQ  8000000     // All TU's ; needed for _delay IMPORTANT : set OSCONbits.IRCF accordingly
// mcp9808 TU : Default address (0011000 = 0x18) < 1
#define ADDR 0x30
// I2C TU
#define SDA_TRIS  TRISIObits.TRISIO0
#define SCL_TRIS  TRISIObits.TRISIO1
#define SDA       GPIObits.GP0
#define SCL       GPIObits.GP1

#endif
