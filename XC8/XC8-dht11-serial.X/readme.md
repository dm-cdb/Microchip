This is a XC8 code for a PIC 12F683 to read a DHT11 sensor and output the result to a serial monitor.<br>
Below is a basic schematic from Simulide :

![Microchip-DHT11](https://github.com/user-attachments/assets/b82d07b1-f7d7-4089-a8e5-064b2b43a29b)

Please note that this code will not work with an MCU clocking < 8MHz ; otherwise it could miss some timings when the DHT begins to transmit its 40 bits.<br>
The 50µs between reading the HIGH level determining a 0 or a 1 may prove to short a delay between function calls and various data manipulations.

The code looks sane, as far as I could test it, and at the same time remains simple as is. There is certainly better way to optimize it ;-) <br>
Below is a comparison between an arduino code and Microchip PIC 12F683 with two different DHT measured at the same time ; they are pretty close to each other :

![dht-11-pic-arduino](https://github.com/user-attachments/assets/831b5e98-971f-4fef-852e-7f25930d220f)

Please note : this code does not test for negative bit in the decimal part of T°.
You can find the DHT11 datasheet used in the pdf included. The translation in English is terrible but still gives the idea...
