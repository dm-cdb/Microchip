This project use an external 16MHz clock/ crystal oscillator on the 12F683 OSC1 & OSC2 pins.<br>
<br>
The 12F683 has two internal programmable clocks :
 - A low frequency internal oscillator (LFINTOSC), running at 31kHz. Its is used mainly for watchdog timer, fail safe clock mode operations.
 - A high frequency internal oscillator (HFINTOSC), running at 8MHz. The frequency can be adjusted in the OSCCON register (IRCF bits), from 125kHz to its nominal 8MHz (default to 4MHz, IRCF = 110).

An external clock to 20MHz can be driven by the PIC should the need occurs. We'll see in this project how to setup an external crystal oscillator of 16MHz, and how to switch between the internal and the external clock.<br>
Please note that using different external and internal clock will affect the built-in __delay() functions, as they rely on the DEFINE _XTAL_FREQ precompiler constant.<br>
Timers will also change, as they are clocked on the instruction cycle (Fcy), which is clock frequency / 4 (Fosc /4).<br>
  - At 8Mhz the instruction frequency Fcy would be 2MHz, and an instruction period or timer tick will last 500ns.
  - At 16Mhzn the  instruction Fcy would be 4MHz, and an instruction period or timer tick will last 250ns.

So the TIMER0 complete cycle around its 255 ticks, with prescaler 1:1, will last 63µs at 16MHz, and obviously twice  or 127µs at 8MHz. Serial communication will be pretty much rendered useless if the code is not adapated to such clocks switchover.<br>
Below is a schematic of a typical external quartz connections to a 12F683 :<br>
<p>
 <br>
![pic-external-crystal-operation](https://github.com/user-attachments/assets/7f3bb083-f277-457f-afee-b27d381fbccb)
 <br>
</p>
<br>
 - OSC1 pin is at pin2 (GP5) on 12F683 <br>
 - OSC2 pin is at pin3 (GP4) on 12F683<br>

 Capacitors C1 on OSC1, and C2 on OSC2, are used to maintain an oscillation, and a rough design guide is provided by the table below :<br>
  <p>
   <br>
  ![capacitors for xtal](https://github.com/user-attachments/assets/d344e37d-e252-4226-a082-f7fc42f0ff1f)
   <br>
 </p>
