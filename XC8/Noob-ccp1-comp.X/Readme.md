This program toggles a LED on the CCP1 pin (GP2) of the PIC 12F683 with the compare mode.

![12f683-ccp1-pins](https://github.com/user-attachments/assets/fc7647e3-b51b-47c6-a6b0-710af65a7ae5)

The compare mode of CCP1 use the timer TMR1 2 x 8 bits registers, and checks when a match occur with the CCP1R 2 x 8 bits registers.<br>
When a match occurs, the CCP1IF flag is set in the PIR1 register.<br>

We also use the CCP1CON control register :

![12f683-ccp1-register](https://github.com/user-attachments/assets/37399ead-d704-46e5-a7d5-64b8021d2e22)

CCP1M<3:0>: CCP Mode Select bits : <br>
1000 = Compare mode, set output on match (CCP1IF bit is set) => CCP1 pin output on<br>
1001 = Compare mode, clear output on match (CCP1IF bit is set) => CCP1 pin output off<br>

Target : blink a LED at around 5Hz

Steps :
- Fosc = 4MHz, default clock.
- Set CCP1/GP2 pin mode to output with TRISIO register
- Configure CCPR1H and CCP1L registers : we'll set them to 0b01011010 and 0b11101110, which together on 16 bits are 0x5AEE = 23278 ticks = 0,025ms w/o prescaler
- Configure TMR1 prescaler to 1:4, so one TMR1 tick = 4 µs ; this means TMR1 will tick during 0,025 * 4 = 0,1ms when matching CCPR1 registers.
  - Led will be on during 0,1ms and off during 0,1ms => period 0,2ms = 5Hz (more or less)

The tricky bit to understand when you configure the CCP1CON / CCP Mode Select bits : <br>
When set to : <br>
- 1000 = Compare mode, set output on match (CCP1IF bit is set) : the pin will be set (on) WHEN a match occurs (CCP1IF flag set).
- 1001 = Compare mode, clear output on match (CCP1IF bit is set) : the pin will be cleared (off) WHEN a match occurs (CCP1IF flag is set).
