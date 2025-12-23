This is a basic I2C bit banging code, useful to interface with temperature sensor or drive an I2C LCD screen etc.
Important :
- Default clock = 4MHz
- Default SDA and SDC = GPIO0 & GPIO1
- Timers are set up for the standard mode of 100 kbps.

If using different clock, adapt i2c.h

A very nice I2C protocol introduction can be found there : http://sin.lyceeleyguescouffignal.fr/basics-of-the-i2c-communication-protocol.<br>
Below is a diagram of the I2C transmission frame :


![i2c-frame-message](https://github.com/user-attachments/assets/479a8db5-6116-4d7c-8da8-a635b9a56c35)

Below are the timers from NXO (formerly Philips, the inventor of this protocol) :


![figure-8](https://github.com/user-attachments/assets/86ea17a6-1156-4dff-88da-acae312c627e)
