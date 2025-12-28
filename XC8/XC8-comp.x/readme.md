This program shows how to use the comparator feature of the PIC 12F683

A comparator compares two voltage sources : Cin- and Cin+.<br>
Then, in basic mode :
- if Cin+ > Cin-, the ouput/result is 1 = VDD (usually 5V).
- if Cin+ < Cin-, the output/result is 0 = VCC (usually ground).

!! IMPORTANT : a maximum source impedance of 10 kΩ is recommended for the analog sources. !!<br>
!! ALSO IMPORTANT : when not using the comparator feature, make sure you disable it !!<br>
Default COMCON0 register is 0b00000000 => CIN pins (GP0 & GP1) are configured as analog, COUT pin (GP2) configured as I/O, Comparator output turned off.<br>
Not good for normal operation.<br>
Set COMCON0 = 0b00000111 => CIN pins are configured as I/O, COUT pin is configured as I/O, Comparator output disabled, Comparator off.<br>

The pins used on the 12F683 are shown below :

![comp-pins](https://github.com/user-attachments/assets/3a83aa58-1a4d-416f-90c1-74f4933e4c87)

- GP0 = Cin+
- GP1 = Cin-
- GP2 = Cout

  There are at least 8 different usage modes for the comparator with the 12F683 , using only one are all of the pins : sSee datasheet 8.3 COMPARATOR I/O OPERATING MODES<br>
  We will be using this mode in our project :

  ![comp-internal-vref](https://github.com/user-attachments/assets/30fe1232-9bdf-48ad-a697-2df94fc68192)

- Vref in connected to Cin+ (this means GP0 can be used for other tasks : you can see in the diagram that the i/o pin is disconnected from CIN+).
- CIN- is GP1 set as analog.
- The result of the comparison is output to the Cout pin (GP2), and the COUT bit in the comparator register (CMCON0)

Below is the content of this register :

![comp-register](https://github.com/user-attachments/assets/71f95dbc-9f23-46a3-b2f9-bf4a49faea85)


CM<2:0>: are the Comparator Mode bits ; to use our mode it needs to be set to :<br>
0b011 : CIN- pin is configured as analog, CIN+ pin is configured as I/O, COUT pin configured as Comparator output, CVREF is non-inverting input (thzt id, connected to CIN+)<br>

We are also using an internal reference voltage as CIN+ ; it is set up in the VRCON register, Voltage Reference Control Register :<br>

![vrcon-register](https://github.com/user-attachments/assets/24d024cf-66f5-4e74-86dc-a2a7cfb6393a)

VRR bit controls the VREF Range : 
- 1 = low range, that is, with VDD = 5V : 0V < Vref < 3,125V - according to the formula : (VR<3:0>/24) * VDD (reminder : VDD is the voltage that power the PIC) <br>
- 0 = high range, that is, with VDD = 5V : 1,25V < Vref < 3,6V - according to formula : VDD/4 + (VR<3:0>/32) * VDD <br>
     => one bit in VR<3:0> field adds 0,156V to the minimum 1,25V.

  Accordingly :
- If Vref > CIN-, COUT pin (GP2) is on, and COUT bit in CMCON0 register set to 1.
- If Vref < CIN-, COUT pin is off, and COUT bit in CMCON0 register set to 1.

We'll be using an interrupt routine using :
- INTCON register with GIE PEIE bits.
- PIE1 register with CMIE bit.
- PIR1 register with CMIF bit.

Also GP5 will be used with a LED to show when Vref < CIN-<br>
See schematic made with SimulIDE below (use a 10K potentiometer max):
  
![pic-comp](https://github.com/user-attachments/assets/499e9978-8404-4f37-bf1a-d235b3d55491)




   
