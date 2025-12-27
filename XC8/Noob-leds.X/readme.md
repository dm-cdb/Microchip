This small project shows how to use five different methods to access Special Function Register (SFR) on a PIC 12F683.<br>
Here we simply use the GPIO register.

1) The bit macro defined in pic12f683.h
2) The direct bit name
3) The bitwise &, |, ^, ~ operators (respectivily AND, OR, XOR, NOT)
4) The bit shifting method
5) The inline ASM method

I'll explain more in details later how the bitwise works ;-) Or you can ask your favorite AI...
In short :
- Use | to set a bit.
- Use & + ~ to clear a bit
- Use ^ to toggle a bit

Below is the schematic :

![noob-leds](https://github.com/user-attachments/assets/edc4caf7-d718-48af-ae21-1775dd732e90)

