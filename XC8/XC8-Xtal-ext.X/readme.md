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
The crystal is connected across :<br>
 - OSC1 pin, located at pin2 (GP5) <br>
 - OSC2 pin, located at pin3 (GP4) <br>

 A crystal and two capacitors are then pretty much all is needed. The rest is software configuration.<br>
 Capacitors C1 on OSC1 and ground, and C2 on OSC2 and ground, are used to maintain a stable oscillation, and some typicals values are provided by the table below, from the datasheet :<br>
 <br>
  ![capacitors for xtal](https://github.com/user-attachments/assets/d344e37d-e252-4226-a082-f7fc42f0ff1f)
  <br>
 
  The topology used by the Pic to drive the crystal is a basic Pierce oscillation cicrcuit : it relies on a simple inverting amplifier, a feedback resistor RF, and the oscillator circuit with the crystal and the two capacitors C1 and C2.<br>
  An optionnal RS resistor can be added if the "drive" (or current, or gain) provided by the Pic amplifier is too powerful for the crystal, and needs to be somehow lowered.<br>
  <br>
 <a href="https://www.ti.com/lit/an/szza043/szza043.pdf?ts=1728842789584"> From Texas Instrument application note</a> :<br>
 
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
You can probe the oscillated signal with an oscilloscope probe at OSC2 pin (don't use OSC1 pin for this ; the internal oscilloscope probe capacitance will load to much the signal).<br>
At minimum use a x10 probe mode - x100 is recommended for minimal interference with the circuit.<br>
Below, from Texas Instrument note, the effect of a RS resistor on the signal :<br>
<br>

 ![oscillator waveform](https://github.com/user-attachments/assets/c2948a68-4594-457b-8fe8-85bb084d4507)
 
 <br>
 In this example :<br>
 - C1 & C2 = 30pF<br>
 - XC2 (reactance of 30pF capacitor at resonant frequency, 25Mhz) = 200Ohms.<br>
 - VCC = 3.3V<br>
 <br>
 The pink and blue waveform clearly are clipped at the top of their shape, signaling a slight overdrive - the VCC used here is 3.3V.
 <br>
 <br>
 <u>The program :</u><br>
 <br>
 To use an external clock, we first need to specify the clock source in the config register FOSC<2:0> bits. According to the mode selected, it activates and/or disable certain GPIO's on the PIC.<br>
 For example :
  - selecting INTOSC (101) activates internal clock and will output a clock signal on OSC2 (thereby disabling GPIO4).<br>
  - selecting INTOSCIO (100) activates an internal clock, and leave GPIO4 & 5 available for other features.<br>
  - selecting XT (001) activates external clock with medium gain ; OSC1 & OSC2 are then reserved and all other features on these pins are disabled.<br>
  - selecting HS (010) likewise activates external clock with high gain (roughly crystal > 4MHz).<br>
  <br>
  The other register to configure for a dual clock configuration is the oscillator control register (OSCCON) :<br>
  - IRCF<2:0> : control the internal clock frequency : default = 4MHz (110) ; we'll use 8MHz (111).<br>
  - SCS<0> : enable the programmer to to control the clock source. If set to 1,the internal clock as set in IRCF bits is used ; If set to 0, the clock defined in FOSC bits of config register - in our case the external clock.<br>
  - OSTS<3> : a status bit ; read 1 = device run on external clock specified in FOSC ; read 0 = internal clock.
 <br>
  <u> Clock fail safe mechanism :</u><br>
  <br>
  Setting the FCMEN<11> bit in the configuration register activate a clock fail safe mechanisme.<br>
  The PIC will monitor the clock signal, and if error conditions occur, will fall back on the internal clock as configured in OSSCON <IRCF> bits.
  When an external clock failure is detected, the PIC switchover the internal clock and set the OSFIF bit of the PIR1 register. If the interrupt is enabled for this event, an ISR routine can be called to handle the situation.<br>
  <br>
  <u>Recovery :</u><br>
  <br>
  A recovery can be tried writing the SCS bit of OSCCON register to 0 (clock from FOSC config) - a kind of soft reset for transient failure (try connect a probe on osc1 port - even a simple jumper).<br>
  Alternatively, we can put the device on SLEEP ; this will stop the external clock. If watchdog is set, the clock used is the internal low frequency LFINTOSC @ 31kHz, and will wake up the device after a specified delay.<br>
  The device will then tries to reinitiate the external clock.<br>
  <br>
 <u>Watchdog timer :</u><br>
 The watchdog timer (WDT) can be activated through the WDTCON register.<br>
 - Setting SWDTEN <0> to 1 activate the watchdog timer.<br>
  - WDTPS<3:0>: Watchdog Timer Period Select bits (select the prescaler). Note the watchdog uses exclusively the LPINTOSC clock (31kHz - 31µs period). The default prescaler 1:512 => 31 * 512 = 16ms. 1:2048 => 64ms.<br>
  <br>
  When a program instruction is "stuck", the device will be reset after the watchdog delay.<br>
  When in sleep mode, if WDT has not been disabled, the device will wake up after the watchdog delay (Watchdog Timer wake-up). WDT is not disabled during SLEEP mode.<br>
  After waking up, the device will use the internal clock, tests that the external clock is stable, and switchover if all is cleared (the test takes around 1024 instruction cycles)<br>
  The OSTS bit of OSCCON register is monitoried to check that the device is really working on the external clock.<br>
  <br>
  <u>Other peripherals :</u><br>
  Three leds or connected to GPIO0-1-2 (for example green, orange, red).<br>
  - GPIO0 HIGH when device run on external clock normally.<br>
  - GPIO2 HIGH when a failure has been detected by the device fail safe mechanisme. The interrupt is caught, a "fail" flag is marked, and the program run a delay before trying to recover.<br>
    If after 5 recovery attempts the device is still in failed external clock mode, a SLEEP is initiated with the watchdog delay.<br>
    During all this time, the device run on internal clock as specified in IRCF of OSCCON register bits.<br>
 - GPIO1 is HIGH when running on internal clock with fail flag cleared.
  
  
 
 
