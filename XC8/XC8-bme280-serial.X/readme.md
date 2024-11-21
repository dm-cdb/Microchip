This project interfaces a Bosch Sensortec BME280 sensor with a PIC12F683.<br>
The BME280 sensor provides temperature, pressure and relative humidity measurements with high accuracy ;<br>
Due to intensive computation formula based on 32bits and float variables, only temperature data is supported here.<br>
Just check the common.h header file and amend it with your own pins topology. Please note the I2C address of the device is in 8 bits form : ie 0x76 (01110110) -> 0xEC (11101100).<br>
<br>
The 32bits temperature formula used is taken from the <a href="https://github.com/boschsensortec/BME280_SensorAPI">Bosch Sensortec API</a> source files.
