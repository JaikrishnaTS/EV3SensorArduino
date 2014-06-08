
/*
Jaikrishna T S
t.s.jaikrishna<at>gmail.com

Initial Date: June 6, 2014
Last Updated: June 7, 2014

This program is for interfacing the Lego EV3 Sensors with Arduino using SoftwareSerial

Note that the program doesn't support AutoID.
The importance is given to using the minimum resources and getting the sensor into Data Mode as soon as possible. 
The sensor is acknowledged multiple times and tried to setup in least time.
This program was originally made to interface the EV3 Sensors with BrickPi.

Credits: lawrie(Github) for EV3UARTSensor library that helped me understand the protocol easily. 

EV3 Sensors Mode Data
---------------------
0 US-DIST-CM  Data16  cm  1 0.00 - 2550.00
1 US-DIST-IN  Data16  inch  1 0.00 - 1000.00
2 US-LISTEN Data8   1 0.00 - 1.00
3 US-SI-CM  Data16  cm  1 0.00 - 2550.00
4 US-SI-IN  Data16  inch  1 0.00 - 1000.00
5 US-DC-CM  Data16  cm  1 0.00 - 2550.00
6 US-DC-IN  Data16  inch  1 0.00 - 1000.00

0 COL-REFLECT Data8 pct 1 0.00 - 100.00
1 COL-AMBIENT Data8 pct 1 0.00 - 100.00
2 COL-COLOR Data8 col 1 0.00 - 8.00
3 REF-RAW Data16    2 0.00 - 1020.19
4 RGB-RAW Data16    3 0.00 - 1020.19
5 COL-CAL Data16    4 0.00 - 65535.00

0 GYRO-ANG  Data16  deg 1 -180.00 - 180.00
1 GYRO-RATE Data16  d/s 1 -440.00 - 440.00
2 GYRO-FAS  Data16    1 -2000.00 - 2000.00
3 GYRO-G&A  Data16    2 -180.00 - 180.00
4 GYRO-CAL  Data16    4 0.00 - 65535.00

0 IR-PROX Data8 pct 1 0.00 - 100.00
1 IR-SEEK Data8 pct 8 -100.00 - 100.00
2 IR-REMOTE Data8 btn 4 0.00 - 10.00
3 IR-REM-A  Data16    1 0.00 - 65535.00
4 IR-S-ALT  Data8 pct 4 0.00 - 100.00
5 IR-CAL  Data16    2 0.00 - 65535.00

*/

#include <SoftwareSerial.h>

bool dat16[] = { 1,1,0,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,0,0,0,1,0,1 };
byte setsd[] = { 1,1,1,1,1,1,1,1,1,1,2,3,4,1,1,1,2,4,1,8,4,1,4,2 };

#define   BYTE_ACK                      0x04
#define   BYTE_NACK                     0x02
#define   CMD_SELECT                    0x43
#define   CMD_MASK                      0xC0
#define   CMD_DATA                      0xC0

#define TYPE_SENSOR_EV3_US_M0          43
#define TYPE_SENSOR_EV3_US_M1          44
#define TYPE_SENSOR_EV3_US_M2          45
#define TYPE_SENSOR_EV3_US_M3          46
#define TYPE_SENSOR_EV3_US_M4          47
#define TYPE_SENSOR_EV3_US_M5          48
#define TYPE_SENSOR_EV3_US_M6          49

#define TYPE_SENSOR_EV3_COLOR_M0       50
#define TYPE_SENSOR_EV3_COLOR_M1       51
#define TYPE_SENSOR_EV3_COLOR_M2       52
#define TYPE_SENSOR_EV3_COLOR_M3       53
#define TYPE_SENSOR_EV3_COLOR_M4       54
#define TYPE_SENSOR_EV3_COLOR_M5       55

#define TYPE_SENSOR_EV3_GYRO_M0        56
#define TYPE_SENSOR_EV3_GYRO_M1        57
#define TYPE_SENSOR_EV3_GYRO_M2        58
#define TYPE_SENSOR_EV3_GYRO_M3        59
#define TYPE_SENSOR_EV3_GYRO_M4        60

#define TYPE_SENSOR_EV3_INFRARED_M0    61
#define TYPE_SENSOR_EV3_INFRARED_M1    62
#define TYPE_SENSOR_EV3_INFRARED_M2    63
#define TYPE_SENSOR_EV3_INFRARED_M3    64
#define TYPE_SENSOR_EV3_INFRARED_M4    65
#define TYPE_SENSOR_EV3_INFRARED_M5    66

byte s,l[8],m[8],sets;
bool setupDone=false;
bool data16;
int r;
byte type = TYPE_SENSOR_EV3_GYRO_M0;

SoftwareSerial sensor(14,16);

void setup(){
  Serial.begin(115200);
  Serial.println("Trying to Setup");
  setupSensor();
  Serial.println("Setup Done!");
  
}

void setupSensor(){
  uint8_t mode;
  if(type < TYPE_SENSOR_EV3_COLOR_M0)
    mode = type - TYPE_SENSOR_EV3_US_M0;
  else if(type < TYPE_SENSOR_EV3_GYRO_M0)
    mode = type - TYPE_SENSOR_EV3_COLOR_M0;
  else if(type < TYPE_SENSOR_EV3_INFRARED_M0)
    mode = type - TYPE_SENSOR_EV3_GYRO_M0;
  else if(type < TYPE_SENSOR_EV3_INFRARED_M5+1)
    mode = type - TYPE_SENSOR_EV3_INFRARED_M0;
  else mode = 0;
  data16 = dat16[type-43];
  sets = setsd[type-43];
  sensor.begin(2400);
  sensor.write(BYTE_ACK);
  setupDone=false;
  while(!setupDone){
    if(sensor.available()){
      if(sensor.read() == BYTE_ACK){
        delay(1);
        sensor.write(BYTE_ACK);
        delay(1);
        sensor.write(BYTE_ACK);
        sensor.end();
        sensor.flush();
        sensor.begin(57600);
        delay(10);
        if((sensor.read() & CMD_MASK) == CMD_DATA){
          sensor.write(BYTE_NACK);
          //if(check(sensor.read(),sensor.read(),sensor.read())==sensor.read())
            setupDone=true;
        }
        else {
          sensor.end();
          sensor.begin(2400);
        }
      }
    }
  }

    sensor.write(BYTE_NACK);
    if(mode != 0){
      sensor.write(CMD_SELECT);
      sensor.write(mode);
      sensor.write(0xff^CMD_SELECT^mode);
  }
  sensor.write(BYTE_NACK);
}

void loop(){
  sensor.write(BYTE_NACK);
  if(sensor.available()){
    s = sensor.read();
    if ( (s & CMD_MASK) == CMD_DATA ){
      byte chk = 0xff^s;
      for(byte n=0 ; n < sets ; n++ ){
        l[n] = sensor.read();
        m[n] = (data16)? sensor.read() : 0;
        chk = chk^l[n]^m[n];
      }
      if (sensor.read() == chk){
        for(byte n=0 ; n < sets ; n++ ){
          r = (m[n]<<8) | l[n] ;
          Serial.print(r);
          Serial.print(" ");
        }
        Serial.println();
      } //else Serial.println("ERROR");
      sensor.flush();
    }
  }
  delay(100);
}
