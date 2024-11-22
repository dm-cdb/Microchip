The MCP9808 is a digital temperature sensor from Microchip, using i2c data bus.<br>
It is reliable and accurate within -20°C up to +100°C, with 0.0625° precision using the maximum resolution (default).<br>
It can be powered from 2.8V up to 5.5V.
<br>
![mc9808-package](https://github.com/user-attachments/assets/b5b654fa-b373-4e9d-a359-6e45b16ecc39)

<br>
Pin A0, A1, A2 are used to set the i2c address of the device. The default (with A0-A2 grounded) is 0011000 (7 bits i2c address), or 0x18. On 8bits, i2c addr << 1 = 0x30.<br>
If the board connects these pins with pullup resistors, make sure to connect the A pins to ground if the default address is wanted.<br>
Pin 3 is the Alert open drain pin. When some temperature thresholds are crossed, this pin connect to ground via a transistor ; see schematic below :<br>

![alert-pin](https://github.com/user-attachments/assets/0c5989b1-fedd-4d72-90dc-bb281ce021a2)

<br>
Open collector and open drain are similars, the difference being the type of transistors used (bipolar or FET).
Please note that SDA and SCL pins of the i2c bus are also open drain pins, and requires the use of pull up resistors. The resistor values depends on the bus capacitance, which cannot exceed 400pF.<br>
The pullup resistors and the i2c bus capacitance form together an RC circuit, which can lower the rise time of the i2c signal below the specification (max 300ns). For this reason pull-up resistors must be "strong", that is < 10k Ohms.<br>
A reasonable value is 4100/5100 Ohms. Breakout boards from DFRobot for example includes pull up resistors for the i2c bus and Alert pin @ 10k Ohms.<br>
It is NOT advisable to use the µcontroller internal weak pullups resistors to drive the i2c bus, the formers are typically > 20k Ohms on a PIC12F683 (on ATMega 328P - ie on Arduino Nano etc., 20k Ohms < pullup < 50k Ohms).<br>
- The i2c signal rise time is the time for a signal to rise from 30% up to 70% of VDD ; ie if VDD = 5V => time to reach 3.5V from 1.5V.
- The RC time constant is R*C : it is the time to charge a capacitance up to 63% given a certain circuit resistance/impedance. Suppose 5V on a circuit => time for a capacitor to reach 5 * 0.63 = 3.15V<br>
- If R = 20k, the max capacitance allowed on the i2c bus is : T = RC => 3x10^-7 / 20 000 = 15pF.<br>
- The SCL/SDA capacitance pins of the MCP9808 is around 5pF ; the 12F683 pin capacitance is around 7pF. On the Arduino Nano the i2c ports capacitance is 10pF. If we add the cabling and breadboard capacitance, we are edging to the danger zone rapidly.<br>
<br>

![mcp98078-dfrobot](https://github.com/user-attachments/assets/c98b53a5-2a34-46bc-bbde-aeed52c89299)

<br>
Please refer to the vendor specification for the exact board design. Below is the DFRobot board schematic :<br>

![dfrobot](https://github.com/user-attachments/assets/d0e1b51d-b24c-4b7c-be8a-395f87e69260)

<br>
Note on Alert pin : by default this pin is HIGH when the ambiant temperature does not cross any threshold (because of the pullup resistor). If it does, the pin voltage becomes lLOW (0V, active LOW). That is, the transistor connect the circuit to ground.<br>
If one want to connect a LED to this pin with default active LOW, put a resistor and LED in serie, with the LED's cathode connected to pin 3 Alert of the sensor and resistor + anode to power line. The led will remain off when the ambiant temperature is within thersholds, and on when it is not, with the cathode connect to ground.<br>
<br>
Note on temparature data : the ambiant temperature is stored in two 8 bits registers, as 12bits ADC result + 1 sign bit. The 4 LSB are the decimal part of temperature, providing a 1 / 2^4 resolution (0,0625). The 8 MSB form the integral part of the temperature.<br>
13th bit is sign bit, with the measurement data formatted in two's complement.<br>
To avoid float variable handling on the PIC12F683, the following process is implemented :<br>
- The 16bits temperature registers are stored in a byte structure. The sign bit member is then evaluated ; if set, then integral temperature member is simply complemented, and the decimal member is two's complemented.<br>
- We divide the decimal result by 4.<br>
- We multiply the decimal result by 6, and add the result of the division.<br>
=> Suppose the raw decimal data is 15 : true decimal result would be 15 * 0.0625 = 0,9375. We decide two decimals is more than enough so 15 * 6 = 90 ; we then add the result of 15/4 as integer = 3 (,75) => 90 + 3 <br>
=> Decimal part then = (0,)93<br>
With this trick we only manipulate byte data, which for an 8bits µcontroller is optimal.


