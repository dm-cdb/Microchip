/*
 * File:   bme280.c
 * Author: Christian de Balorre
 *
 * Created on 6 novembre 2024, 19:40
 */


#include <xc.h>
#include "i2c_bb.h"
#include "bme280.h"

unsigned char bme280_hello(unsigned char addr) {
    unsigned char ret;
    i2c_start();
    ret = i2c_wr(addr & 0xFE);
    i2c_stop();
    
    return ret;
}


unsigned char bme280_rd_byte(unsigned char addr, unsigned char code){
    unsigned char ret;
    i2c_start();
    i2c_wr(addr & 0xFE);
    i2c_wr(code);
    i2c_start();
    i2c_wr(addr | 0x01);
    ret = i2c_rd(NACK);
    i2c_stop();
    
    return ret;
}

void bme280_wr_byte(unsigned char addr, unsigned char code, unsigned char reg){
    i2c_start();
    i2c_wr(addr & 0xFE);
    i2c_wr(code);
    i2c_wr(reg);
    i2c_stop();
    
    return;
}

void bme280_init(unsigned char addr) {
    unsigned char t_stby = 0x3C;  // 000 -> 5ms ; 001 -> 62.5ms ; 010 -> 125ms
    unsigned char mode = 0x23;    // 0b00100011 normal mode, skip pressure, T° oversamplig x 1
    bme280_wr_byte(addr, DEV_CONFIG, t_stby);
    bme280_wr_byte(addr, CTRL_MEAS, mode);
    
    return;    
}

unsigned char bme280_get_id(unsigned char addr) {
    unsigned char ret;
    ret = bme280_rd_byte(addr, DEV_ID);
    return ret;    
}

void bme280_get_tdig(unsigned char addr, struct tempDig * dig_t) {
    // Burst read
    i2c_start();
    i2c_wr(addr & 0xFE);
    i2c_wr(DIG_T);
    i2c_start();
    i2c_wr(addr | 0x01);
    dig_t->T1l = i2c_rd(ACK);
    dig_t->T1h = i2c_rd(ACK);
    dig_t->T2l = i2c_rd(ACK);
    dig_t->T2h = i2c_rd(ACK);
    dig_t->T3l = i2c_rd(ACK);
    dig_t->T3h = i2c_rd(NACK);
    i2c_stop();
    
    return;
}

void bme280_get_tADC(unsigned char addr, struct dataRead *col) {
    // Burst read
    i2c_start();
    i2c_wr(addr & 0xFE);
    i2c_wr(TEMP_REG);
    i2c_start();
    i2c_wr(addr | 0x01);
    col->msb = i2c_rd(ACK);
    col->lsb = i2c_rd(ACK);
    col->xlsb = i2c_rd(NACK);
    i2c_stop();
    
     return;    
}

void bme280_temp_fine(unsigned char addr) {
    struct tempDig dig_t = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    struct dataRead get_t = {0x0,0x0,0x0};
    unsigned int dig_T1;
    int dig_T2, dig_T3;
    long adc_T, var1, var2;
    // Debug
    // My PIC : dig_t = {0x90; 0x6E, 0xED, 0x68, 0x32, 0x00};
    bme280_get_tdig(ADDR, &dig_t);        
    dig_T1 = ((unsigned int)(dig_t.T1h << 8)) + dig_t.T1l;  // 0x6E90 = 28304
    dig_T2 = (dig_t.T2h << 8) + dig_t.T2l;                  // 0x68ED = 26861
    dig_T3 = (dig_t.T3h << 8) + dig_t.T3l;                  // 0x0032 = 50
    
    // Debug
    // get_t = {0x81, 0x2F, 0x60);
    bme280_get_tADC(ADDR, &get_t);
    
    // See Bosch datasheet 4.2.3 for compensation formulas
    // Also github.com/boschsensortec/BME280_SensorAPI/blob/master/bme280.c
    adc_T = 0;
    adc_T = get_t.msb;          // Trick to force a << 12 in a long (?) see XC8 user guide 5.3.7.1
    adc_T = adc_T << 12;
    adc_T = adc_T | (get_t.lsb << 4) | (get_t.xlsb >> 4);
    
    var1 =((adc_T / 8) - (dig_T1 * 2));
    var1 = (var1 * dig_T2) / 2048;
    var2 = ((adc_T / 16) - (dig_T1));
    var2 = (((var2 * var2) / 4096) * dig_T3) / 16384;
        
    t_fine = var1 + var2;
    
    return;    
}

// Store value in nn.dd° format : 5123 -> 51.23°C
void bme280_comp_temp(unsigned char addr, int * T) {
    bme280_temp_fine(addr);
    *T = (int)((t_fine * 5 + 128) >> 8);
    
    return;
}