This code is an example of how to use the TMR1 timer.<br>
Unlike TMR0 and TMR2 which hold tick counter in a byte register, the TMR1 uses two registers : TMR1H:TMR1 ; counter can then increment up to 0xFFFF, that is 65535.<br>
Here we use the standard 4MHz internal clock (1µs per tick), with TMR1 prescaler set to 1:8 ; ie, it will need 8 ticks to increment the counter.
We also enable the TMR1 interrupt : GIE = 1, PEIE = 1 in INTCON register, and TMR1IE = 1 in PIE1 register.
Below the different registers that can be used by TMR1 :

![TMR1-registers](https://github.com/user-attachments/assets/70af20cf-44f0-4456-8371-cc2c4b750921)


However we will preset  TMR1H:TMR1 registers with TMR1H = 0x0B and TMR1L = 0xDB   => 0x0BDB = 3035.<br>
The interrupt will then trigger in (65535 - 3035) * 8 * 1µs = 0,5 sec (8 is the prescaler).<br>
The interrupt routine increment a user defined variable, and when this variable == 2 (0,5sec * 2), it toggle a led (HIGH = 1 sec ; LOW = 1 sec).<br>
This gives a period of 2 sec, a frequency of 0,5Hz (1/2 sec) and a duty cycle of 50% (ratio :  HIGH/period * 100)<br>

![TMR1-interr](https://github.com/user-attachments/assets/9592a9e3-ca5e-488f-be64-e2c060549135)
