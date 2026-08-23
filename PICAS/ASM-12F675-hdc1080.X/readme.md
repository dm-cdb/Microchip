This is an assembly program to drive an HDC1080 temperature and humidity sensor from Texas Instrument with a simple Microchip PIC 12F675.<br>
This sensor is one of the best value for money on the market, with an excellent precision (14 bits ADC).<br>
No fancy pins : VCC, Ground + SCLA and SDA I2C pins for data communication.<br>
The code is around 600 word long ; include a minimal serial driver, I2C driver and HDC1080 driver.

## Schematic:

From the datasheet:

<img width="1028" height="453" alt="HDC1080-application" src="https://github.com/user-attachments/assets/a552a83e-f3bd-4bce-9bc2-8428b862d8d6" />

From SimulIDE @ 5V VCC:

<img width="683" height="398" alt="HDC-1080-diag" src="https://github.com/user-attachments/assets/49b2aa27-caab-47d9-95b9-8500156a39b5" />

You'll find this sensor with convenient breakout boards. I got one from Fasizi, but beware: the integrated 10K pull ups are not activated and you must solder the I2C pads together to activate them.<br>
For the sake of simplicity, I instead put two external 3.3K ohms resistors between the I2C bus and 5V VCC.<br>
VCC is provided by the 5V pin from the USB to serial converter.

## Breadboard:

<img width="1600" height="1200" alt="HDC-1080-breakout" src="https://github.com/user-attachments/assets/91735b57-2ba9-4651-963a-85d827355a5e" />

## Transformation functions

This assembly code tries to optimize the 8bits architecture of this PIC : no software multiplication or division, just shifting around bits...<br>

### Temperature
The supported temperature for exemple range between +125°C and -400C (165°C amplitude). The transformation from the datasheet is therefore :<br>
((measured T°C)/2<sup>16</sup>) * 165 - 40

This means (considering that the 2 LSB of the 16bits data are always 0):
1 lsb = (4/2<sup>16</sup>) * 165 = 0,01007 <br>

I modified the transformation function first by shifting the measured result right by 2, then dividing the result by 2<sup>14</sup>.
1 lsb = (1/2<sup>14</sup>) * 165 = 0,01007 <br>
We could then decide that one lsb = 1 hundredth of °C. But the drift of 0.007 over the range of 16384 = 114, that is 1.14 degree.<br>
To achieve a better precision, we will perform the following operations:

1. Divide by 127 (2<sup>7</sup> and add to result
1/128 = 0,0078125<br>
This equal to shift right result by 7.<br>
Alomost there, but not quite ; we are 0.0081 higher from exact value.<br>

2. Divide by 1024 (2<sup>10</sup> and substract to result
1/1024 = 0,00097<br>
This equal to shift right result by 10.<br>
The correction is now equal to 0.0068. Getting closer.<br>

3. Last correction : Divide by 4096  (2<sup>12</sup> and add to result
1/4096 = 0,00024<br>
This equal to shift right result by 12.<br>
0.0068 + 0.00024 = 0.00704<br>
This is enough precision for a final result with two decimal places.<br>

4. We then substract 4000 to the final result
If the CARRY bit is not set, we then perform a 2's complement to get an absolute value, and set the sign to minus (-).<br>

### Humidity
We perform the same logic with humidity measurement, except we'll shift right by 13 the gross value from the sensor.<br>
1 lsb = (1/2<sup>13</sup>) * 100 = 0.0122<br>
We decide that one lsb = 1 hundredth of %RH.
We then only have to make it for the extra 0.22/<br>

1. Divide result by 4 and add to result (1 + 0.25)<br>
1/4 = 0.25<br>
This equal to shift right result by 2.<br>
This is 0.03 higher than expected.<br>

2. Divide result by 32 and substract to result
1/32 = 0,031<br>
This is more than enough for a RH with two decimal places.

## Notes
The default code displays on the serial terminal the raw value in hexadecimal from the sensor, so you can always compare the PIC transformed function with the real one from Texas Instrument datasheet.<br>
Code tested in real conditions from +30°C -> -10°C.<br>

Hope this will help ! ;-)
