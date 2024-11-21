/* 
 * File:   common.h
 * Author: Christian de Balorre
 *
 * Created on 4 novembre 2024, 15:38
 */

#ifndef COMMON_H
#define	COMMON_H

#define _XTAL_FREQ  8000000     // All TU's ; needed for _delay
// Bosch BME280E address = 1110110 (0x76) or 1110111 (0x77, like BMP280)
#define ADDR 0xEC
// I2C TU
#define SDA_TRIS  TRISIObits.TRISIO0
#define SCL_TRIS  TRISIObits.TRISIO1
#define SDA       GPIObits.GP0
#define SCL       GPIObits.GP1
// Serial TU
#define TX      GPIObits.GP4
#endif
