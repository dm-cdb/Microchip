This project shows an example of ADC result displayed on standard LCD 2x16 chars screen,like the LCM1602A.<br>
The 74HC164 shift register enable the use of only 3 GPIO's on the PIC.<br>
It has a rudimentary and light LCD library that can be reused for other projects.<br>
Note : 8bit controllers like the PIC 12F683 do NOT like floating point operation. The ADC result is thus transformed from V to mV:
- The adc conversion full result is stored in a 24 bits unsigned int variable.
- The result is then translated from V to mV by * 1000, and then * 5 (if Vref = 5V).
- Final operation, the result is divided by 1024, shifting 10 bits to the right.
The end result is then translated to BDC for LCD or serial reading.<br>

![analog-lcd](https://github.com/user-attachments/assets/559606ad-142d-4977-b5c3-ac738cdbc8e8)

