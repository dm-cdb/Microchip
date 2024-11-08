This is a serial UART communication bit-bang C library, predefined at 9600 bauds.<br>
This firmware uses TMR2 counter to clock the signal as accurately as possible.<br>
It is recommended to create a common.h file with the following preprocessor directive :<br>
<br>
 _XTAL_FREQ  : almost always needed in any program, since this is the basis for built-in __delay functions.<br>
 TX          : GPIO port used for data transmission. Can be any but GP2, reserved for RX, with PIC12F683<br>
 RX          : GPIO port used for data reception ; it uses the external interrupt (GP2 with PIC12F683) on falling edge to detect start of transmission from peer.<br>
<br> 
Include the common.h in both your main.c and serial.h files.<br>