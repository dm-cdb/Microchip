This is a tutorial that shows how to use TMR2 timer.<br>
As usual the PIC clock is left at 4MHz ; so it should normally increment TMR2 by one tick / µs.<br>
However we also set a prescaler to 1:16 ; which means TMR2 will have wait for 16 ticks before incrementing.<br>
Also we set a limit within TMR2 with the help of the PR2 register.<br>

![TMR2-registers](https://github.com/user-attachments/assets/0b168518-7132-4964-8870-9c3021775ade)

Ie if PR2 is set to 125 (0x7d), then when TMR2 reaches this limit, it will reset to 0 and set the TMR2IF bit flag in the PIR1 register.<br>
The flag will be set every 1µs * 16 (prescaler) * 125 (PR2) = 2ms<br>
We also show how to use an interrupt routine to check the TMR2IF bit flag and toggle GPIO0 :<br>
In this routine, GPIO0 will toggled HIGH and LOW on every interrupts. ; this gives a frequency of 1/(0.002 *2) = 250Hz, whith a 50% duty cycle.<br>

An interrupt, when used, stop the main routine program which stores the next instruction address on a stack,  and jump to specially defined interrupt function.
When this interrupt function returns, the programm jump back to the next instruction address saved on the stack and continue until the next interrupt.

See below simulation created with SimulIDE :

![PIC-TMR2](https://github.com/user-attachments/assets/bf147619-1569-4a6d-9a23-c8176ee59817)

