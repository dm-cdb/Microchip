This small project uses  the capture mode of CCP1 module (GP2 on 12F683).

![pic12F683-ccp1](https://github.com/user-attachments/assets/96468384-d92a-49e0-89ac-6ad8f87a647d)

Basically GP2 is set in input mode, and count the delay bewteen raising edges or falling edges of a signal, using the TMR1 counter.<br>

<img width="850" height="421" alt="Square-signal-properties" src="https://github.com/user-attachments/assets/a71f59f9-694e-4981-beaa-4c1daea4386a" />
<br>
<br>

The TMR1 register (TMR1H:TMR1L) result is then copied to the CCPR1H:CCPR1L register, and from that we can :
- compute the period of a signal.
- compute the duty cycle of a signal (changing from raising edge mode to the falling edges mode right after the raising edge has been detected).
- output a frequency divider, based on CCP1M capture mode configration bits.

Below is the register used to program the capture mode of CCP1 :

![ccp1con-register](https://github.com/user-attachments/assets/8d170a70-a2f4-4ab5-8164-b4b99b232487)

We then set the last 4 bits :

0100 = Capture mode, every falling edge
0101 = Capture mode, every rising edge
0110 = Capture mode, every 4th rising edge
0111 = Capture mode, every 16th rising edge

On our project we set Capture mode on every 4th rising edge. This means that if the input signal is 500Hz, the output signal on GP2 will be 500 / 4 (4th rising edge) / 2 (GP2 toggle at each interrupt ) => 62,5 Hz
TMR1 prescaler is set to 1:2 : it means, with the MCU clock set at 4Mhz, that each TMR1 tick = 2 x 1µs.
To sum up :
- Fosc = 4 MHz
- Instruction clock = Fosc / 4 = 1 MHz → 1 µs per tick
- TMR1 prescaler = 1:2 =>  TMR1 tick = 2 µs

=> since TMR1 is 16bits register (65535), the maximum measurable period before overflow is : 65536 × 2 µs ≈ 131 ms<br>
=> minimum frequency without overflow : 1 / 131 = 7,6 Hz

Gneral formula for computing a period on CCP1 :<br>
- Fosc = oscillator frequency (Hz)
- P = Timer1 prescaler [1,2, 4, 8]
- N = CCP1M capture divider [1, 4, 16] (raising edge only)
- ΔC = difference between two captured values

TMR1 increment once every : (4 * P) / Fosc (because each tick = 4 Fosc cycle : with no prescaler P, TMR1 tick = 1µs @ 4MHz)<br>
-> Incoming signal period formula : (ΔC * 4P)/(N * Fosc)<br>

Example :<br>
- Fosc = 4 MHz
- P = 2 (TMR1 prescaler 1:2)
- CCP1M = capture every rising edge → N = 1

if ΔC = 1000 :<br>
Signal period = (1000 * 4 * 2)/(1 x 4000000) = 0,002 sec<br>
Frequency = 1 / 0,002 = 500Hz
