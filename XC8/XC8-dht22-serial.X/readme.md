This is a XC8 code for a PIC 12F683 to read a DHT22 sensor and output the result to a serial monitor.<br>
Below is a basic schematic from Simulide :

![pic-dht22-simulide](https://github.com/user-attachments/assets/7a3c2a0f-ee37-42e3-a00b-852fe7a22661)

Please note that this code will not work with an MCU clocking < 8MHz ; otherwise it could miss some timings when the DHT begins to transmit its 40 bits.<br>
The 50µs between reading HIGH levels determining a 0 or a 1 may prove to short a delay between function calls and various data manipulations at 4Mhz for example.

This code has been tested with positive T°C value as well as negative T°C ; unlike the DHT11, it decrements correctly the negative value without bugging between -0,9 decimal and the
next negative integer.<br>
See picture below :

![pic-dht22-negative](https://github.com/user-attachments/assets/c1212dbb-b3e4-4f8d-8274-b12dec3bd678)

