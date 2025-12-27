This is a tutorial on how to use Analog to Digital Conversion feature of the 12F683.

!! Important : The maximum recommended impedance for analog sources is 10 kΩ. !!<br>
=> do not put a 1M Ohms potentiometer to measure the resulting voltage.<br>
One of the reason is that the ADC circuit uses an internal capacitor (around 10pF), with additional internal resistors (around 10k Ohms).

!! Also important !! <br>
After enabling the ADC module, do not start a conversion immediatly ; wait for the ADC module to settle :<br>
Tacq = Time acquisition<br>
Tacq = Amplifier Settling Time + Hold Capacitor Charging Time + Temperature Coefficient <br>
The Hold Capacitor Charging Time (10pF) is dependant on impedance / resistors in serie with it.<br>
A delay of 6µ sec is reasonable.

GP0, GP1, GP2 and GP4 can act as ADC input pin.

There are quite a few registers involved :
- TRISIO : set the desired analog PIN to input mode.
- ANSEL  : configure the ADC clock, and the input pin to analog
- ADCON0 : set the conversion format,  voltage reference, select the analog pin, enable the ADC, start ADC conversion process
- ADRESH : stores the first 8 bits of the conversion
- ADRESL : stores the last 2 bits of the conversion

Note : 
- Setting a pin to an analog input automatically disables the digital input circuitry, weak pull-ups and interrupt-on-change, if available.

Below are the different steps :

 1. Configure GPIO Port:

  • Disable pin output driver (See TRIS register)<br>
  • Configure the ADC conversion clock (See ANSEL register) : for 4MHz MCU clock, recommended FOSC is FOSC/16 (Tad = 4µs)<br>
  Below the conversion time cycle :

![conversion_cycle](https://github.com/user-attachments/assets/966ea045-2292-43e9-9d78-14d5f0589c42)

  • Configure pin as analog<br>
  
2. Configure the ADC module: (ADCON0 register)

  • Configure voltage reference : default is VCC, usually 5V<br>
  • Select ADC input channel<br>
  • Select result format : default : "Left justified"<br>
  • Turn on ADC module
  
3. Configure ADC interrupt (optional): not used in this tutorial

  • Clear ADC interrupt flag<br>
  • Enable ADC interrupt<br>
  • Enable peripheral interrupt<br>
  • Enable global interrupt
  
4. Wait the required acquisition (6µs is safe margin)

5. Start conversion by setting the GO/DONE bit.<br>
6. Wait for ADC conversion to complete by one of the following:<br>
  • Polling the GO/DONE bit (ADCON0 register)<br>
  • Waiting for the ADC interrupt (if interrupts enabled)<br>
7. Read ADC Result

So quite a few operations here. Below is the summary of the different registers used in the whole process :

![ADC-registers](https://github.com/user-attachments/assets/7720cfe3-42e3-4e28-8681-51529bcdda52)

When using default result format "left justified", the ADRESH and ADREL registers are populated this way :

![adc_result_registers](https://github.com/user-attachments/assets/5e1f53c0-593c-470c-92d9-d3b99a968972)

So basically we declare a 16bits (short) variable, and do the following operation :
- Copy ADRESH register to variable, then in the variable shift two bits to the left.
- Shift ADRESL 6 bits to the right, and "OR" with the variable.

With VREF = 5V, the resolution is 5 / 1024 = 0,00488 ; ie 1 bit (or 1 LSB) = 5mV<br>
If ADC returns 512 => 512 * 0,00488 = 2,5V

Below is the project schematic :

  ![pic12F683-adc](https://github.com/user-attachments/assets/d2ce49ca-df72-41f0-a0a1-a08a14d2f071)

