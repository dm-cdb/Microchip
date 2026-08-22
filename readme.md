Various Microchip projects for mid base PIC 12F683 written with pic-as or XC8 compilers.

## PIC 12F683:

![pic12f683-pins](https://github.com/user-attachments/assets/bdf6ee1d-45e3-45e6-b566-f4d95bd624b5)

This PIC has an internal 8MHz oscillor, with software selectable frequency from 125KHz, and support an external oscillator up to 20MHz. 
It also has 2048 words (14 bits) of program memory space (flash), 128 bytes of general purpose registers (aka data memory space - static RAM) and 256 bytes of eeprom.

## PIC 12F675:

<img width="837" height="284" alt="PIC12F675" src="https://github.com/user-attachments/assets/d11091af-8515-4f7d-b769-7d2f1939a4a5" />

This PIC has an internal 4MHz oscillor and support an external oscillator up to 20MHz. 
It also has 1024 words (14 bits) of program memory space (flash), 64 bytes of general purpose registers (aka data memory space - static RAM) and 128 bytes of eeprom.

CAREFUL: the programming of this pic will overwrite the oscillator calibration value (OSCCAL) and the bandgap calibration bit in the config word.<br>
Therefore, always backup these values and restore them, especially the OSCCAL value.<br>
The OSCCAL value is stored in the last FLASH memory address, that is 0x03FF. <br>
The default value of a FLASH address is 0x3FFF = 11 1111 1111 1111 in binary - this will be value stored at each address when erasing a flash memory.<br>
Flash address 0x03FF should instead contain a value in the form 0x34[OSCCAL], 0x34 being the opcode for a RETLW instruction, and OSCCAL being the 6 bits calibration value that must be stored in the OSCCAL register.<br>
For example 0x344C will return 0x4C as the calibration value. Therefore the first instruction of the code should ALWAYS be :

`   bsf RP0 `<br>
`   call 0x03ff  ; get oscillator calibration value`<br>
`   movwf OSCCAL`<br>

See page 18 and 56 of PIC 12F675 datasheet.<br>

If you don't reprogram correctly this address with 0x34xx, your code will jump to address 0x03FF and perform a addlw 255, then copy this value in the OSCCAL register (that is the maximum frequency)...<br>
I will propose later a simple method to recover the OSCCAL value in case it has been erased and not recovered.


If you program the PIC with a K150 programmer :<br>
- insert the chip to the correct location
- click "CALIB" button - a warning message then pops up telling the return value should be in the "0x34" form.
- Click YES to change this value.
- Next windows, click the "Read" button ; it will read the current band gap (most often 01) and oscillator values stored in the chip.
- Click then "Yes" button and confirm to insert value in HEX file.

You should then read the correct OSCCAL value at address 0x03FF.


<img width="634" height="435" alt="calib-1" src="https://github.com/user-attachments/assets/954a406e-a1c3-43cd-af4e-41a67b0a5e25" />








