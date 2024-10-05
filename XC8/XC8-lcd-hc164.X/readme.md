This project drives an 2x16 LCD screen with a PIC 12F683 using three wires.<br>
It makes use of the 74HC164 serial to parallel (un-latched) shift register.<br>
Because its outputs are not latched, the layout is bit more simple and straitforward than that of 74HC595.<br>

![74HC164](https://github.com/user-attachments/assets/5815cf4a-2fc9-4c51-a560-50093b98a675)

The 74HC164 shift register IC has two and-gated entries, A & B.<br>
If any one of the input is LOW, a logical "0" will be stored in QA/0 at the next clock input - the previous content/status of QA is then shifted to QB/1 etc.<br>
If both input are HIGH, A logical "1" will be stored in QA at the next clock input + the usual bit shift.<br> 
In our use case we'll just tie A&B together (see diagram below) ; alternatively A or B can be tied to VCC and the other port to the PIC pin. <br>
Unlike the 74HC595, the parallel outputs QA/0...QH/7 are not latched, so they immediatly present their state to the connected device.<br>
For this reason, the display will be disabled when shifiting the bits, and enabled when shifting is done.<br>
The clear pin is active LOW, hence must be tied to VCC.<br>
The clock pin as usual record the AB gates status HIGH or LOW to QA register and shift all bits to the neighbour Q.<br>

Below is a possible wiring diagram :


![pic12f683-74hc164](https://github.com/user-attachments/assets/e051cf02-b6f8-483c-864b-48beeee77b41)
