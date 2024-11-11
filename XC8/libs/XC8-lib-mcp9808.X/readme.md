Basic library for Microchip MCP9808 temperature sensor.<br>
Note that this sensor uses an i2c bus to communicate digital temperature data (in °C) : you need to include the i2c bitbang library file in this project.<br>
On a high level, a simple call to mcp9808_get_temp(struct temp_res *) will retrieve the 16 bits sensor register 0x06 in the following structure :<br>
<code>
struct temp_res {
    unsigned char sign;    // Temperature sign char : positive '+' (0x2b) or negative '-' (0x2d)
    unsigned char t_ing;  // Integral part of temperature
    unsigned char t_dec;  // Decimal part of temperature
    unsigned char alert;  // Alert flag
};
</code>
Edit the common.h files with the necessay SDC/SDA pins options, as well as device address and PIC operationnal clock frequency.
<br>
About the Microchip MPC9808 sensor :<br>
The MCP9808 is an i2c temperature sensor that provide fine resolution and accuracy between -40°C and +125°C.<br>
Its Operating Voltage Range is conveniently from 2.7V -> 5.5V.<br>
Please note the temperature data are converted in °C.<br>
<br>

![mcp9808](https://github.com/user-attachments/assets/10d21a26-8c0a-46c2-a960-f89ce13a9531)

<br>
There is unfortunately no DIP version of this sensor ; but you can find breakout modules, ie DFRobot Fermion: MCP9808 High Accuracy I2C Temperature Sensor.<br>
Note that this module comme with 6 x 10K pull up resistors (the 103 components on the module) for the address pins, the i2c pins (so no need to add external pullups in the circuit), and also the Alert pin.<br>
<br>

![dfrobot-mcp9808](https://github.com/user-attachments/assets/c417b6a4-3a56-4c23-950e-5372ed879ee4)

<br>
Pin 6-7-8 A are used to configure the device i2c address : default is 7 bits 0011000 - with the last three bits user defined.
Pin3 Alert is an open collector pin with a default active low : when an alert threshold is set, the pin will be connected to ground.<br>
You can for exemple connect a led with a pullup resistor that will turn on in that case.<br>
A connection diagram example is shown below :<br>
<br>

![mcp9808-simulide](https://github.com/user-attachments/assets/24c0b5d8-a4a0-4332-ac63-6d5e32da6131)
<br>
A LED is connected to the Alert pin of the MCP9808 ; when a threshold is exceeded, the led will be connected to ground and turned on.<br>
Optionnally GPIO5 of PIC12F683 could be used in ADC mode to set some threshold on the MCP9808 device.
