This is a basic, lightweight library for i2c communication protocol.<br>
I2c protol is a serial data communication protocol in master/slave mode, using two wires for data and clocking.<br>
Each device is targeted via a 7bits address - the last bit being use to command R (1) or W(0) operation.<br>
The code is based on Microchip AN1488 "Bit Banging I2C™ on Mid-Range MCUs with the XC8 C Compiler", and the source is available on Microchip website.<br>
The i2c device used in this application note is an external EEPROM storage device.<br>
It is strongly advised to create/use a common.h header file with the following directives :<br>
<br>
#define _XTAL_FREQ  8000000     // All TU's ; needed for _delay_ms<br>
<br>
#define SDA_TRIS  TRISIObits.TRISIO0<br>
#define SCL_TRIS  TRISIObits.TRISIO1<br>
#define SDA       GPIObits.GP0<br>
#define SCL       GPIObits.GP1<br>
<br>
Include this file in main.c, i2c_bb.h and other header files of your program if necessary.<br>
Use whatever ports are prefered for clock line and data line.<br>
This code has been tested with the following i2c device :<br>
- DS1621 temperature sensor
- MCP9808 temperature sensor
- BME280 temperature, humidity and pressure sensor

SDA and SDC usually need pull-up resistors to default the lines to HIGH state. See typical digram below :<br>
<br>
![i2c-diagram](https://github.com/user-attachments/assets/eab75d01-6812-405b-a0db-0a06333a0f47)

When short cables are used between the controler and the device, 5K to 10K resistors can be used.<br>
The limit is the capacitance of the line, which combined with the resistors make up an RC circuit that can slow the rise and falling time of the clock/signal.<br>
<br> 
Important ! Always check that the operating voltage of the i2c device is compatible with your microcontroler ports HIGH/LOW specifications (typically 5V or 3.3V).
