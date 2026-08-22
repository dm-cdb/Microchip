This is an assembly program to drive an HDC1080 temperature and humidity sensor from Texas Instrument with a simple Microchip PIC 12F675.<br>
It is one of the best value for money sensor on the market, with an excellent precision (14 bits ADC).<br>
No fancy pins : VCC, Ground + SCLA and SDA I2C pins for data communication.<br>

## Schematic:

<img width="683" height="398" alt="HDC-1080-diag" src="https://github.com/user-attachments/assets/49b2aa27-caab-47d9-95b9-8500156a39b5" />

You'll find this sensor with convenient breakout boards. I got one from Fasizi, but beware : the integrated 10K pull ups are not activated and you must solder the I2C path on it to activate them.<br>
For the sake of simplicity, I instead put two external 3.3K ohms resistors between I2C bus and 5V VCC.<br>
VCC is provided by the 5V from the USB to serial converter.

## Breadboard:

<img width="1600" height="1200" alt="HDC-1080-breakout" src="https://github.com/user-attachments/assets/91735b57-2ba9-4651-963a-85d827355a5e" />

## Transformation functions

This assembly code tries to optimize the 8bits architecture of this PIC : no software multiplication or division, just shifting around bits...<br>
The supported temperature for exemple range between +125°C and -400C (165°C amplitude). The transformation from the datasheet is therefore :<br>
((measured T°C)/2<sup>16</sup>) * 165 - 40

This means (considering that the 2 LSB of the 16bits data are always 0):
1 lsb = (4/2<sup>16</sup>) * 165 = 0,01007 <br>

I modified the transformation function first by shifting the measured result right by 2, then dividing the result by 2<sup>14</sup>.
1 lsb = (1/2<sup>14</sup>) * 165 = 0,01007 <br>
Hence this represents around one hundredth of degree. To have more precision we apply the following operation :

