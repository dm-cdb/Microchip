This is a crude bit bang code for serial transmission, 8-n-1 : one start bit (low), 8 data bit, one stop bit (high), no parity, 

It uses timer2 to manage the data signal. At 9600 bauds, a bit or symbol shall be transmitted/recieved at 1/9600 = 104us ;
At 4MHZ, each tick interval is one instruction cycle, that is 1us, so PIR2 register will simply equal 104, with no pre|post scaler.
No timer2 interrupt is used ; the routine simply check for the timer2 overflow flag.

![data-byte-transmission](https://github.com/user-attachments/assets/b1236586-8dc2-4569-8bbf-3a32ee17b685)


![data-bits-transmission](https://github.com/user-attachments/assets/d9164697-6cd7-4134-818e-faffdf5e4e14)

As can be seen, the least significant bit of the byte is transmitted/recieved first.
When in RX mode, ideally the reciever will "capture" the signal level at the middle of bit transmission, that is 104/2 us delay.
So the delay from the very start of the transmission to the first bit capture will be 104 (start) + 52 (middle of first bit) ; the following bit capture interval will then be the regular 104us.

This program propose a RX and TX routine, and some tests where a user can turn on/off leds according to serial input.
Of course this is mainly for PIC w/o hardware serial/UART support, like the PIC12F683.
If the need is only to display information or debug message to a terminal, just connect a single tx pin, and discard the RX code.
