/* 
 * File:   i2c_bb.h
 * Author: Christian de Balorre
 * 
 * IMPORTANT :
 * Default clock = 4MHz ; define _XTAL_FREQ before any include in main.c
 * Default SDA and SDC = GPIO0 & GPIO1
 * Created on 27 octobre 2024, 13:19
 */

#ifndef _XTAL_FREQ
#define _XTAL_FREQ  4000000     // Needed for built-in  __delay_us(x) & __delay_ms(x)
#endif

// Define SDA and SDC port before any include in main.c
#ifndef SDA_TRIS
#define SDA_TRIS  TRISIObits.TRISIO0
#endif
#ifndef SCL_TRIS
#define SCL_TRIS  TRISIObits.TRISIO1
#endif
#ifndef SDA
#define SDA       GPIObits.GP0
#endif
#ifndef SCL
#define SCL       GPIObits.GP1
#endif


#ifndef I2C_BB_H
#define	I2C_BB_H

#define ACK       0x00
#define NACK      0x01

// Standard I2C timings (minimum)
// Start condition is defined as when the SDA line goes LOW before SCL transitions LOW.
// Stop condition is defined as SDA transitions to a HIGH state after the SCL transitions HIGH.

// Setup time : minimum time data must remain stable before it is clocked.
// Hold time : max time interval after clocking has been initiated.
// Hold Time For Start Condition (tHD;STA): minimum time the SDA should be low before SCL goes low.
// Setup Time For a Start Condition (tSU;STA): minimum time the SDA line remains high before initiating a repeated start.

#define tLOW    5   // Low period of the SCL clock = 4.7탎
#define tLOWRD  2   // Read takes more processing cycles
#define tHIGH   4   // High Period of the SCL clock = 4탎
#define tHIGHRD 2   // Read takes more processing cycles
#define tSU_DAT 1   // Data setup time = 250ns
#define tHD_STA 4   // Hold time (low) [repeated] START condition = 4탎
#define tSU_STA 5   // Set-up time (high) for a repeated START condition = 4.7탎
#define tSU_STO 4   // Set-up time for STOP condition = 4탎
#define tBUF    5   // Bus free time between a STOP and START condition = 4.7탎

void i2c_start(void);                   				// I2C start condition
void i2c_stop(void);                    				// I2C stop condition
static void i2c_bit_out(unsigned char data);            // Writes a bit to I2C bus
static void i2c_bit_in(unsigned char *data);            // Reads a bit from the I2C bus
unsigned char i2c_wr(unsigned char i2c_data);           // Writes a byte to the I2C bus
unsigned char i2c_rd(unsigned char ack);                // Reads a byte from the I2C bus

#endif	/* I2C_BB_H */

