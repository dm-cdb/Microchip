This is a XC8 code for a PIC 12F683 to read a DHT11 sensor and output the result to a serial monitor.<br>
Below is a basic schematic from Simulide :

![Microchip-DHT11](https://github.com/user-attachments/assets/b82d07b1-f7d7-4089-a8e5-064b2b43a29b)

Please note that this code will not work with an MCU clocking < 8MHz ; otherwise it could miss some timings when the DHT begins to transmit its 40 bits.<br>
The 50µs between reading HIGH levels determining a 0 or a 1 may prove to short a delay between function calls and various data manipulations at 4Mhz for example.

Below is a comparison between an Arduino (Nano) code and Microchip PIC 12F683 (com6) with two different DHT11 measured at the same time ; they are pretty close to each other :

![dht-11-pic-arduino](https://github.com/user-attachments/assets/831b5e98-971f-4fef-852e-7f25930d220f)

This code does test the negative bit, with the same hardware bug already observed with an Arduino ATMega 328P : it does not know how to decrement for example from -n,9 to next negative integer, and will go from -10,9 to -10,0 instead of -11,0 ...<br>
See picture below :


![pic-dht11-bug](https://github.com/user-attachments/assets/b5f6d9d6-3cce-48fe-a6ff-df0a237e5b27)
