This project use the PWM functionality of the PIC 12F683 CCP1 pin, that is GPIO2 pin.<br>
This GPIO features the CCP module, that is CAPTURE/COMPARE/PWM :
- The Capture mode allows the timing of the duration of an event.
- The Compare mode allows the user to trigger an external event when a predetermined amount of time has expired.
  
PWM uses TMR2 register (and PR2 register) to control the period (the HIGH + LOW duration), and CCPR1L register to control the pulse width (HIGH duration only).<br>
The picture below from the datasheet shows how it is done :<br>
<br>
![pic-pulse-timers](https://github.com/user-attachments/assets/674cdbeb-a6b7-4823-aed8-8bb96c284509)
<br>

The period (or 1/frequency) is set by the PR2 register :<br>
When TMR2 is equal to PR2, TMR2 is cleared and the CCP1 pin (GPIO2 on a 12F683) is set.

The pulse width will then depend on the duration set in the CCPR1L register + 2 bits in the CCP1CON register.<br>
- The CCP1 pin (GPIO2) will then be high for the duration set in CCPR1L register + 2 bits.
- The pulse width timing should be kept <= PR2 timing.
 
The datasheet specifies a few equations for precise control : <br>
<br>
PWM Period = ((PR2) + 1) * 4 * TOSC * (TMR2 Prescale Value)<br>
Note : TOSC stands for Time of clock oscillation ; we use here simply the internal 4MHz clock for TMR2.<br>
So :<br>
- TOSC = 1/4 000000 = 0,250µs = 0,00000025 sec<br>
- TMR2 prescaler is set with the T2CKPS1 bits of T2CON register : ie 11 -> Prescaler 1:16.<br>

If we set PR2 = 124, we have :
- PWM Period = (124 + 1) * 4 * (1/4000000) * 16 = 0,002sec (500Hz)
- the * 4 simply means we need 4 clock oscillations to increment TMR2 ; 16 is the prescaler we set before.

Suppose we want a 500Hz period ; we rearrange the equation to find the correct PR2 :<br>
PR2 = (PWM Period) / ( 4 * TOSC * (TMR2 Prescale Value)) -1<br>
PR2 = (0,002) / (4 * 0,00000025 * 16) -1 = 124  
  
Now calculating the pulse width is also tricky ; the equation is given by : <br>

Pulse Width = (CCPR1L:CCP1CON<5:4>) * TOSC * (TMR2 Prescale Value) <br>

CCPR1L is a regular 8 bit register ; it is concatenated with the 2 bits of the CCP1CON register (DC1B bits) to form a 10bits value.<br>
Suppose we leave CCP1CON<5:4> bits clear and set CCPR1L to binary : 0b00111110 (62in decimal) ; the ten bits value will be CCPR1L + CCP1CON<5:4> = 0b0011111000 = 248 in decimal.<br>

Pulse Width = 248 * 0,000000250 * 16 = 0,000992 , ie roughly = 0.001 = 1msec = 50% (duty cycle) of the 2msec period set by PR2 and the prescaler.<br>

When the TMR2 counter register (+ 2 other bits) matches the CCPR1L + CCP1CON<DC1B>, the CCP1 pin is cleared/LOW, until TMR2 counter eventually matches PR2, for another cycle. <br>
<br>
To sum up The code initiate a 0,002sec PWM signal, with around 50% duty cycle with :<br>
 - PR2 = 125 <br>
 - CCPR1L = 62 <br>

The following steps should be followed :
- Disable the PWM pin (CCP1) output drivers by setting the associated TRIS bit.
-  Set the PWM period by loading the PR2 register.
-  Configure the CCP module for the PWM mode by loading the CCP1CON register with the appropriate values.
-  Set the PWM duty cycle by loading the CCPR1L register and (optionaly) DC1B bits of the CCP1CON register.
-  Configure and start Timer2:
    • Clear the TMR2IF interrupt flag bit of the PIR1 register.
    • Set the Timer2 prescale value by loading the  T2CKPS bits of the T2CON register.
    • Enable Timer2 by setting the TMR2ON bit of the T2CON register.
- Enable PWM output after a new PWM cycle has started:
  • Wait until Timer2 overflows (TMR2IF bit of the PIR1 register is set).
  • Enable the CCP1 pin output driver by clearing the associated TRIS bit.
  
To add some fun, GPIO4 pin is set as analog input and ADC channel to convert a voltage value into ADRESH register (left justified).<br>
The ADRESH value is then copied to CCPR1L. We therefore have a voltage controled PWM on CCP1/GPIO2 pin.<br>
<br>

![pic-pulse width-modulated](https://github.com/user-attachments/assets/c1413076-d4b4-4ac7-9980-4ad24b57aec2)


