Basic library for Microchip MCP9808 temperature sensor.<br>
Note that this sensor uses an i2c bus to communicate digital temperature data (in °C).<br>
On a high level, a simple call to mcp9808_get_temp(struct temp_res *) will retrieve 16 bits sensor register ox06 in the following structure :<br>
<code>
struct temp_res {
    unsigned char sign;    // Temperature sign char : positive '+' (0x2b) or negative '-' (0x2d)
    unsigned char t_ing;  // Integral part of temperature
    unsigned char t_dec;  // Decimal part of temperature
    unsigned char alert;  // Alert flag
};
</code>
<br>
The MCP9808 is an i2c temperature sensor that provide fine resolution and accuracy between -40°C and +125°C.<br>
Its Operating Voltage Range is conveniently from 2.7V -> 5.5V.<br>
Please note the temperature data are converted in °C. 
