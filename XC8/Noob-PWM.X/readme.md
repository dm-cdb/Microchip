This project use the PWM functionality of the PIC 12F683 CCP1 pin, that is GP2 pin.<br>
<br>
PWM uses TIMER2 to control the frequency and pulse width. The picture below from the datasheet shows how it is done :<br>
<br>
![pic-pulse-timers](https://github.com/user-attachments/assets/674cdbeb-a6b7-4823-aed8-8bb96c284509)
<br>

The periode (or frequency, 1/period), is set by the PR2 register :<br>
When timer2 counter TMR2 is equal to PR2, TMR2 is cleared and The CCP1 pin (GP2 on a 12F683) is set.<br>
The CCP1 pin will then be high for the duration set in CCPR1L register + 2bits from DC1B<1:0> bits of the CCP1CON register - for a possible total of 10 bits.<br>
The pulse width timing should be kept <= PIR2 timing. Thus the higher the frequency of the PWM signal, the lower the available resolution.<br>
The datasheet specifies a few equations for precise control : <br>
<br>
PWM Period = ((PR2) + 1) * 4 * TOSC * (TMR2 Prescale Value)
<br>
- TOSC = 1 / PIC device clock ; for default internal 4MHz TOSC = 1/4 000000 = 250ns (= device clock period).<br>
- TMR2 prescaler is set with the T2CKPS1 bits of T2CON register : 11 -> Prescaler 1:16.<br>
  Rearranging the equation, for PWM period of 500Hz, we should have : PR2 = (4MHz / (4 * 16 * 500Hz)) -1 = 124 <br>
  Or : Period = (124 + 1) * 4 * 2.5 * 10^-7 * 16 = 0.002 = 2ms => 1/0.002 = 500Hz <br>
<br>
Calculating the pulse width is more tricky ; the equation is given by : <br>
Pulse Width = (CCPR1L:CCP1CON<5:4>) * TOSC * (TMR2 Prescale Value) <br>
CCPR1L is a regular 8 bit register ; it is concatenated with 2 bits of the CCP1CON register (DC1B bits) to form a 10bits value.<br>
Suppose we leave CCP1CON<5:4> bits cleared and set CCPR1L to binary : 0b00111110 ; the ten bits value will be 0b0011111000 = 248 in decimal.<br>
Pulse Width = 248 * 2.5 * 10^-7 * 16 = 0,000992 , ie roughly = 0.001 = 1msec = 50% (duty cycle) of the 2msec period set by PR2.<br>
<br>
When the TMR2 counter register (+ 2 other bits) matches the CCPR1L + CCP1CON<DC1B>, the CCP1 pin is cleared/LOW, until TMR2 counter eventually matches PR2, for another cycle. <br>
<br>
The code initiate a 500Hz PWM signal (device internal clock = 4MHz), with around 50% duty cycle.<br>
 - PR2 = 125 <br>
 - CCPR1L = 62 <br>
<br>
GP4 pin is set as analog input and ADC channel to convert a voltage value into ADRESH register (left justified).<br>
The ADRESH value is then copied to CCPR1L. We then have a voltage controled PWM on CCP1 pin.<br>
<br>

![pic-pulse width-modulated](https://github.com/user-attachments/assets/c1413076-d4b4-4ac7-9980-4ad24b57aec2)


