This project use an external 16MHz clock/ crystal oscillator on the 12F683 OSC1 & OSC2 pins.<br>
<br>
The 12F683 has two internal programmable clocks :
 - A low frequency internal oscillator (LFINTOSC), running at 31kHz. Its is used mainly for watchdog timer, fail safe clock mode operations.
 - A high frequency internal oscillator (HFINTOSC), running at 8MHz. The frequency can be adjusted in the OSCCON register (IRCF bits), from 125kHz to its nominal 8MHz (default to 4MHz, IRCF = 110).

An external clock up to 20MHz can be easily driven by the PIC should the need occurs. We'll see in this project how to setup an external crystal oscillator of 16MHz, and how to switch between the internal and the external clock.<br>
Please note that using different external and internal clock will affect the built-in XC8 __delay() functions, as they rely on the DEFINE _XTAL_FREQ precompiler constant.<br>
Timer behaviours will also change, as they are synchronized on the instruction cycle (Fcy), which is clock frequency / 4 (Fosc /4).<br>
  - At 8Mhz the instruction frequency Fcy would be 2MHz, and an instruction period or timer tick will last 500ns.
  - At 16Mhz the  instruction Fcy would be 4MHz, and an instruction period or timer tick will last 250ns.

So for the TIMER0 to complete a whole cycle around its 255 ticks with prescaler set at 1:1, it will take 63µs at 16MHz, but obviously twice  or 127µs at 8MHz.<br>
Serial communication for instance will be pretty much rendered useless if the code is not adapated to such clock switchovers.<br>
Below is a schematic of a typical external quartz connections to a 12F683 taken from its datasheet :<br>
 <br>
![pic-external-crystal-operation](https://github.com/user-attachments/assets/7f3bb083-f277-457f-afee-b27d381fbccb)
 <br>
<br>
- The crystal is connected across :<br>
 - OSC1 pin, located at pin2 (GP5) <br>
 - OSC2 pin, located at pin3 (GP4) <br>

 Capacitors C1 on OSC1 and ground, and C2 on OSC2 and ground, are used to maintain a stable oscillation, and some typicals values are provided by the table below, from the datasheet :<br>
 A crystal and two capacitors are then pretty much all is needed. The rest is software configuration.<br>
 <br>
  ![capacitors for xtal](https://github.com/user-attachments/assets/d344e37d-e252-4226-a082-f7fc42f0ff1f)
  <br>
 
  The topology used by the Pic to drive the crystal is a basic Pierce oscillation cicrcuit : it relies on a simple inverting amplifier, a feedback resistor RF, and the oscillator circuit with the crystal and the two capacitors C1 and C2.<br>
  An optionnal RS resistor can be added if the "drive" (or current, or gain) provided by the Pic amplifier is to powerful for the crystal, and needs to be lowered.<br>
  <br>
 <a href="https://www.ti.com/lit/an/szza043/szza043.pdf?ts=1728842789584"> From Texas Inbstrument application note</a> :<br>
 
 RF : RF is the feedback resistor of the CMOS inverter and it biases the inverter in its linear region :<br>
 This means that instead of a binary high (ie 5V) or low (0V) at its input and output, both will be around VDD / 2, with VDD = supplied voltage, usually 5V for a PIC)<br>
Usually, the value chosen is between 1 MΩ and 10Ω, to create some initial noise and "bootstrap" the oscillations.<br>
When the oscillation is close to the resonant (nominal) frequency of the crystal, its circuit impedance is much lower than the RF circuit which is therefore kind of discarded (for its AC part).<br>
The PIC12F683 datasheet thus states : "The value of RF varies with the Oscillator mode selected (typically between 2MΩ to 10MΩ)." <br>
<br>
RS : RS isolates the output of the inverter from the crystal and prevents spurious high-frequency oscillation, so that a clean waveform can be obtained. Typical value 3kΩ - 10kΩ. Also used to lower the amplifier drive for low frequency crystal.<br>
Normaly not necessary if the PIC amplifier drive mode (or gain) has been correctly selected (XT vs HS). For crystals around 4MHz, since they could be driven on XT or HS mode, one could use a RS in HS mode. But then why not use the internal clock ? :-) <br>
For all crystal > 8MHz, the RS should not be a concern.
<br>
C1 and C2 : used to maintain oscillation and provide the capacitor load (CL) supported by the crystal.<br>
Usually 15 - 22 pF for 4 - 20 MHz crystal. C1 & C2 should not in general be < 10pF, since they will not be able to filter noises efficiently, and not above 33pF since it could induce to much power in the crystal.<br>
<br>
<a href="https://ww1.microchip.com/downloads/en/appnotes/00849a.pdf">From Microchip AN849 application note</a> :<br> 

"When the circuit is ringing (ie oscillating) at the resonant (ie nominal) frequency, charge is transferred back and forth through the crystal between the capacitors. Both capacitors, then, work together to sustain the oscillation.<br>
Generally, they are chosen to be the same value to provide a symmetrical store of energy on both sides of the crystal and are chosen to match the load capacitance specified by the crystal manufacturer.<br>
The capacitors are seen as being in series by the oscillating circuit, in terms of their load, and therefore the equation is a simple formula for series capacitors :"<br>
- CL = (C1 * C2) / (C1 + C2) (+CS)<br>
 - CL is the load capacitance supported by the crystal, usually specified in its datasheet.<br>
 - Stray capacitance CS, inherent to the circuit on PCB etc. needs also to be added ; can be around 5-7pF.<br>
   The PIC pins capacitance are also a factor - unfortunately they are not documented anywhere... We could use a reasonable guess of 5pF.<br>
Suppose we use the recommended 22pF capacitors as C1 & C2 to work with a 16MHz crystal, CS = 7pF and Cpin = 5pF :<br>
- CL = ((22 + 5)(22 + 5)) / (22 + 5 + 22 + 5) + 7 = 20.45pF - within spec ; should be happy. :-) <br>
<br>
Also please note : C1 and C2 form a capacitor voltage divider that determines the degree of feedback. The feedback factor is given by f = C1 / C2.<br>
<br>
<br>
How it works :<br>
In order for an amplified circuit to oscillate, it needs 2 conditions :<br>
- The gain of the circuit must be > 1 to maintain the oscillation, otherwise it will fade away.<br>
- The overall phase shift of the oscillator circuit must be > 360° (positive feedback) :<br>
  - The inverter amplifier provide a 180° shift (ie 5V at its input => 0V at its output - the waveform is inversed).
  - From Texas Instrument note : "If C1 = C2, current through them is identical and 180 degrees out of phase from each other. Hence, for C1 = C2, the crystal provides a phase shift of 180 degrees."<br>
 <br>
 The initial oscillation is created for example by noise on the RF circuit, due to the high resistance. The initial impulse will drive progressively the oscillator circuit to the resonant frequency of the crystal, who then stabilizes at this frequency, provided we have configured the circuit correctly with :<br>
 - right amount of drive (XT or HS - optionnaly an RS).<br>
 - the right capacitance load C1 & C2.<br>
 <br>
Troubleshooting :<br>
You can probe the oscillated signal with an oscilloscope probe at OSC2 pin (don't use OSC1 pin for this ; the internal oscilloscope probe capacitance will load to much the signal).
At minimum use the x10 probe mode - x100 is recommended.
Below, from Texas Instrument note, the effect of a RS resistor on the signal :<br>
<br>

 ![oscillator waveform](https://github.com/user-attachments/assets/c2948a68-4594-457b-8fe8-85bb084d4507)
 
 <br>
 In this example :<br>
 - C1 & C2 = 30pF<br>
 - XC2 (reactance of capacitor at resonant frequency, 25Mhz) = 200Ohms.<br>
 - VCC = 3.3V<br>
 <br>
 The pink and blue waveform clearly are clipped at the top of their shape, signaling an overdrive - the VCC used here is 3.3V.
 <br>
 <br>
 <u>The program :</u><br>
 <br>
 To use an external clock, we first need to specify the clock source in the config register FOSC<2:0> bits. According to the mode selected, it activates and/or disable certain GPIO on the PIC.<br>
 For example :
  - selecting INTOSC (101) activates internal clock and will output a clock on OSC2 (thereby disabling GPIO4).<br>
  - selecting INTOSCIO (100) activates an internal clock, and leave GPIO4 & 5 available for other features.<br>
  - selecting XT (001) activates external clock with medium gain ; OSC1 & OSC2 are then reserved and all other features on these pins disabled.<br>
  - selecting HS (010) likewise activates external clock with high gain (roughly crystal > 4MHz).<br>
  <br>
  The other register to configer for a dual clock configuration is the oscillator control register (OSCCON) :<br>
  - IRCF<2:0> : control the internal clock frequency : default = 4MHz (110) ; we'll use 8MHz (111).<br>
  - SCS<0> : control if the internal clock as set in IRCF is used (if set to 1), or the clock as set in FOSC bits of config register - in our case the external clock (if set to 0).<br>
  - OSTS<3> : status bit ; read 1 = external clock specified in FOSC ; read 0 = internal clock.
  <br>
  <u> Clock fail safe mechanism</u><br>
  
 
 
