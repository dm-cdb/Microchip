/* 
 * File:   common.h
 * Author: Christian
 * Define SDA and SCL I2C ports
 * Created on 4 novembre 2024, 15:38
 */

#ifndef COMMON_H
#define	COMMON_H

#define _XTAL_FREQ  8000000     // All TU's ; needed for _delay

// I2C TU
#define SDA_TRIS  TRISIObits.TRISIO0
#define SCL_TRIS  TRISIObits.TRISIO1
#define SDA       GPIObits.GP0
#define SCL       GPIObits.GP1

#endif
