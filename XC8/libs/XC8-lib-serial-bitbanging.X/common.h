/* 
 * File:   common.h
 * Author: Christian de Balorre
 *
 * Created on 4 novembre 2024, 15:38
 */

#ifndef COMMON_H
#define	COMMON_H

// All TU's ; needed for _delay ; set 
#define _XTAL_FREQ  8000000     

// Serial TU
#define TX      GPIObits.GP1        // Any GPIO ports
#define RX      GPIObits.GP2        // Port with external interrupt
#endif