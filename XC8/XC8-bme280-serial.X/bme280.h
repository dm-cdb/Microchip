/*
 * File:   bme280.h
 * Author: Christian de Balorre
 *
 * Created on 6 novembre 2024, 19:26
 */

#include "common.h"

// Default BME280 address (8bits)
#ifndef ADDR
#define ADDR 0xEC
#endif

#ifndef BME280_H
#define	BME280_H

// BME280 registers addresses
#define DEV_ID      0xD0                 // Device ID
#define DEV_RESET   0xE0                 // Reset device
#define CTRL_HUM    0xF2                 // Humidity aquisition options (oversampling))
#define DEV_STATUS  0xF3                 // Device status : measuring, updating
#define CTRL_MEAS   0xF4                 // T° & pressure acquisition options (oversampling) + mode (sleep, forced, normal)
#define DEV_CONFIG  0xF5                 // Standby time, filter time constant, enable spi
#define PRESS_REG   0xF7                 // Pressure data 0xF7, 0xF8, 0xF9
#define TEMP_REG    0xFA                 // Temperature data 0xFA, 0xFB, 0xFC
#define HUM_RE      0xFD                 // Humidity data 0xFD, 0xFE
#define DIG_T       0x88                // Temp calibration data

struct tempDig {
    unsigned char T1l;
    unsigned char T1h;
    char T2l;
    char T2h;
    char T3l;
    char T3h;
};

struct dataRead {
    unsigned char msb;
    unsigned char lsb;
    unsigned char xlsb;
};

long t_fine;

/*
 * arg1 = device i2c address
 * arg2 = register address
 * arg3 = data written in pointed register
 */


unsigned char bme280_rd_byte(unsigned char, unsigned char);
unsigned int bme280_rd_int(unsigned char, unsigned char);
void bme_wr_byte(unsigned char, unsigned char, unsigned char);
unsigned char bme280_hello(unsigned char);
void bme280_init(unsigned char);
unsigned char bme280_get_id(unsigned char);
void bme280_get_tdig(unsigned char, struct tempDig *);
void bme280_get_tADC(unsigned char, struct dataRead *);
void bme280_temp_fine(unsigned char addr);

// arg2 = pointer to compensated T° : signed int
void bme280_comp_temp(unsigned char, int *);

#endif	/* BME280 */

