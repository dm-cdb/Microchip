/*
 * File:   mcp9808.c
 * Author: Christian de Balorre
 *
 * Created on 1 novembre 2024, 19:40
 */


#include <xc.h>
#include "i2c_bb.h"
#include "mcp9808.h"

unsigned char mcp9808_hello(unsigned char addr) {
    unsigned char ret;
    i2c_start();
    ret = i2c_wr(addr & 0xFE);
    i2c_stop();
    
    return ret;
}


unsigned char mcp9808_rd_byte(unsigned char addr, unsigned char code){
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

void mcp9808_rd_int(unsigned char addr, unsigned char code,struct temp_data *data){
    i2c_start();
    i2c_wr(addr & 0xFE);
    i2c_wr(code);
    i2c_start();
    i2c_wr(addr | 0x01);
    data->datah = i2c_rd(ACK);
    data->datal = i2c_rd(NACK);
    i2c_stop();

    return;
}

void mcp9808_wr_int(unsigned char addr, unsigned char code, struct temp_data *reg) {
    i2c_start();
    i2c_wr(addr & 0xFE);
    i2c_wr(code);
    i2c_wr(reg->datah);
    i2c_wr(reg->datal);
    i2c_stop();
    
    return;
}

void mpc9808_wr_byte(unsigned char addr, unsigned char code, unsigned char reg){
    i2c_start();
    i2c_wr(addr & 0xFE);
    i2c_wr(code);
    i2c_wr(reg);
    i2c_stop();
    
    return;
}

void mcp9808_get_temp(struct temp_res *col) {
    struct temp_data data;
    // Default temp struct values : no alerts, temp positive
    col->alert = 0x00;
    col->sign = 0x2b;
    
    // Debug : alert upper, 35,75°
    // data.datah = 0x42;
    // data.datal = 0x3C;
    mcp9808_rd_int(ADDR, TEMP, &data);
            
    // Parse flag bits :    
    if ((data.datah & 0x20) == 0x20){ //T° < TLOWER
        col->alert = 0x20;
    }
    if ((data.datah & 0x40) == 0x40){ //T° > TUPPER
        col->alert = 0x40;
    }
    if ((data.datah & 0x80) == 0x80){ //T° ³ TCRIT
        col->alert = 0x80;
    }
    // Check if TA < 0 and perform 2 complement in that case
    if ((data.datah & 0x10) == 0x10){
        col->sign = 0x2d;
        data.datah = ~data.datah;
        data.datal = ~data.datal + 1;
    }
    // Temperature parsing
    data.datah = data.datah & 0x0F;           // Mask alert and sign
    data.datah = (unsigned char)(data.datah << 4);
    col->t_ing = data.datah | (data.datal >> 4);
    col->t_dec = data.datal & 0x0F;

    return;    
}