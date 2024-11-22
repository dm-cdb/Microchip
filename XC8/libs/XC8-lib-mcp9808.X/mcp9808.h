/*
 * File:   mcp9808.h
 * Author: Christian de Balorre
 *
 * Created on 1 novembre 2024, 19:26
 */

#include "common.h"

#ifndef ADDR
#define ADDR 0x30
#endif

#ifndef MCP9808_H
#define	MCP9808_H

// MCP9808 pointers & Commands
#define CONFIG      0x01                         // Configuration register
#define ALERT_UPPER 0x02                         // Alert temperature upper
#define ALERT_LOWER 0x03                         // Alert temperature lower
#define ALERT_CRIT  0x04                       // Critical temperature trip
#define TEMP        0x05                         // Temperature
#define DEVID	    0x06                         // Manufacturer ID
#define DEVREV	    0x07                         // Revision ID
#define RES	    0X08                         // Resolution

struct temp_data {
    unsigned char datah;
    unsigned char datal;
};

struct temp_res {
    unsigned char sign;
    unsigned char t_ing;
    unsigned char t_dec;
    unsigned char alert;
};

/*
 * arg1 = device i2c address
 * arg2 = register pointer address
 * arg3 = data written in pointed register
 */
unsigned char mcp9808_hello(unsigned char);
unsigned char mcp9808_rd_byte(unsigned char, unsigned char);
void mcp9808_rd_int(unsigned char, unsigned char, struct temp_data *);
void mcp9808_wr_byte(unsigned char, unsigned char, unsigned char);
void mcp9808_wr_int(unsigned char, unsigned char, struct temp_data *);
void mcp9808_get_temp(struct temp_res *);

#endif	/* MCP9808*/

