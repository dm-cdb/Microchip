PIC 12F683 EEPROM operations<br>
<br>
This firmware shows how to perform eeprom write/read operations.<br>
Read operations are very straight forward, but write operations need to follow a strict protocol :
- Enable write operation with EECON1<2> WREN bit set.
- It is highly recommended to globaly disable all interrupt, with clear GIE bit of INTCON register.
- Write address to EEADR register : the 12F683 has 256 bytes EEPROM, so addresses : 0x00 -> 0xFF.
- Write bytes data to EEDATA register.
- Write 0x55 to EECON2 register, then
- Write 0xAA to EECON2 register.
- Start the write operation with bit WR of EECON1<1> set.
- Wait for WR bit of EECON1 register to clear, or EEIF bit flag to be set in PIR1 register <8>.
<br>
Note : EECON1 WR bit is automatically cleared in hardware. It cannot be cleared by software.<br>
Repeate the process from step #3 for each byte to be written.<br>
After write operations are completed,  clear the Enable write operation with EECON1<2> WREN to prevent accidental eeprom writes.
As per datasheet, DC characteristics, D122, an Erase/Write Cycle Time (TDEW) takes around 5ms.<br>
<br>
A read operation is far more simple :<br>
- Write address to be read in EEADR registe< br>
- Set RD bit of EECON1 register <br>
- Data is then copied from EEPROM address to EEDATA register. It takes just a TCY cycle and the data is available at the next instruction. <br>
- RD bit is cleared in hardware only.<br>
<br>
Warning : The EECON1, EEDAT and EEADR registers should not be modified during a data EEPROM write (WR bit = 1).
<br>
This project includes a small LCD 2x16 library with a 74HC164 shift register - see below for wiring.<br>
Below is a schematic of the project : the led is active during write operation.
<br><br>

![pic-eeprom](https://github.com/user-attachments/assets/b1307967-2775-4759-83d3-0b718090a080)
