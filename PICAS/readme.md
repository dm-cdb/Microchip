This directory presents some projects programmed in assembly for the Microchip PIC family.<br>
They have been developped mostly for mid-range, cheap 12F683 and 12F675, but can easely be adapted for other Microchip PIC families.<br>
MPLAB X IDE v6.x is the preferred developpement environnement with XC8/pic-as v2.40 onward.

For the linker to build the programm correctly, use the following options:
`-pisrVec=04h -Wl,-presetVec=00h`<br>

isrVec is the PSECT interrupt code.<br>
resetVec is the PSECT reset code, telling the ALU where to start.<br>

Got to "Project properties", then "pic-as global options":

<img width="1009" height="632" alt="pic-as" src="https://github.com/user-attachments/assets/d8843409-1df4-4ee4-a271-92cabc1d8725" />

- Reset should always begin at flash address 0x0000.
- Interrupt handling at flash address 0x0004.



