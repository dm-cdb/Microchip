This project shows an example of ADC result shown on standard LCD 2x16 chars display,like the LCM1602A.<br>
It has a rudimentary and light LCD library that can be reused for other projects.<br>
Note : 8bit controllers like the PIC 12F683 do NOT like floating point operation. So a kind of fixed point operation is used :
- The adc conversion full result is stored in a 24 bits unsigned int variable.
- The result is then translated from V to mV by * 1000, and then * 5 (if Vref = 5V).
- Final operation, the result is divided by 1024, shifting by 10 bits to the right.
The end product is then translated to BDC for LCD or serial reading.<br>

t
