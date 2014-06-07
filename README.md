EV3SensorArduino
================

Arduino Interface for EV3 Sensors

This program is for interfacing the Lego EV3 Sensors with Arduino using SoftwareSerial

Note that the program doesn't support AutoID.
The importance is given to using the minimum resources and getting the sensor into Data Mode as soon as possible. 
The sensor is acknowledged multiple times and tried to setup in least time.
This program was originally made to interface the EV3 Sensors with BrickPi.
This program doesn't support the modes having multiple data sets yet. 

Credits: lawrie(Github) for EV3UARTSensor library that helped me understand the protocol easily. 
