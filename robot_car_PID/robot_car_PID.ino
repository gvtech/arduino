#include <NewPing.h>
#include <Wire.h>
#include <PCF8574.h>

#include "Wheel.h"

PCF8574 expander;



// Gyeroscope
#define gyroaddr 0x1E //0011110b, I2C 7bit address of HMC5883
#define expdaddr 0x38 //0011110b, I2C 7bit address of FCF8574

// left distance controler
const int leftTrigger = 7;
const int leftEcho = 8;

// left Motor
const int leftSpeedInt = 0;
const int leftMotorCtrl1 = 0; // expand
const int leftMotorCtrl2 = 1; // expand
const int leftMotorEnable = 10;
int leftMotorSpeed = 0;

// right distance controler
const int rightTrigger = 5;
const int rightEcho = 6;

// right Motor
const int rightSpeedInt = 1;
const int rightMotorCtrl1 = 2;
const int rightMotorCtrl2 = 3;
const int rightMotorEnable = 11;
int rightMotorSpeed = 0;


// Switch
const int onoffSwitch = 4; // expand
int onoff,pressed =0;

// LED lights
const int frontLeftLight = 9;
const int frontRightLight = 4;
const int backLeftLight = 12;
const int backRightLight = 13;


NewPing leftSonar(leftTrigger,leftEcho,200); 
NewPing rightSonar(rightTrigger,rightEcho,200); 

unsigned int leftDistance,rightDistance;
int heading,prevAzimut,backTrackTempo = 0;

int getAzimut() {
  int x,y,z;
  Wire.beginTransmission(gyroaddr);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();
 
 //Read data from each axis, 2 registers per axis
  Wire.requestFrom(gyroaddr, 6);
  if(6<=Wire.available()){
    x = Wire.read()<<8; //X msb
    x |= Wire.read(); //X lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    return z;
  }
  
}

unsigned int getDistance(NewPing sonar) {
 unsigned int distance = sonar.ping_cm();
  // 0 distnace means out of range. We default it to 200 cm.
  if (distance==0)
    distance=200;
 return distance; 
}

boolean checkOnoff() {
  if (expander.digitalRead(onoffSwitch)==HIGH) {
    if (!pressed) {
      pressed=1;
      onoff=!onoff;
    }
  } 
  else
    pressed = 0;


  return onoff;
}

Wheel leftWheel = Wheel(expander,leftMotorCtrl1,leftMotorCtrl2,leftMotorEnable);
Wheel rightWheel = Wheel(expander,rightMotorCtrl1,rightMotorCtrl2,rightMotorEnable);

void leftWheelInt() {
  leftWheel.incSpeedCount();
}
void rightWheelInt() {
  rightWheel.incSpeedCount();
}

void setup() {
  // set A4 and D5 for I2C protocole
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  
  digitalWrite(A4,LOW);
  digitalWrite(A5,LOW);

  Serial.begin(9600);
  leftWheel.Setup();
  rightWheel.Setup();
  attachInterrupt(leftSpeedInt,leftWheelInt,RISING);
  attachInterrupt(rightSpeedInt,rightWheelInt,RISING);
 
  expander.begin(0x38);
  expander.pinMode(onoffSwitch,INPUT);

  Wire.begin();  
  //Put the HMC5883 IC into the correct operating mode
  Wire.beginTransmission(gyroaddr); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();

  pinMode(leftTrigger,OUTPUT);
  pinMode(leftEcho,INPUT);
  pinMode(rightTrigger,OUTPUT);
  pinMode(rightEcho,INPUT);

  pinMode(frontLeftLight,OUTPUT);
  pinMode(frontRightLight,OUTPUT);
  pinMode(backLeftLight,OUTPUT);
  pinMode(backRightLight,OUTPUT);
  heading = getAzimut();
}


void sendData(int azimut,unsigned int leftdistance,unsigned int rightdistance) {
 Serial.print("{\"azimut\":");
 Serial.print(azimut);
 Serial.print(",\"leftDistance\":");
 Serial.print(leftdistance);
 Serial.print(",\"rightDistance\":");
 Serial.print(rightdistance);
 Serial.print(",\"leftSpeedCount\":");
 Serial.print(leftWheel.getSpeedCount());
 Serial.print(",\"rightSpeedCount\":");
 Serial.print(rightWheel.getSpeedCount());
 Serial.print(",\"leftSpeed\":");
 Serial.print(leftWheel.getSpeed());
 Serial.print(",\"rightSpeed\":");
 Serial.print(rightWheel.getSpeed());
 Serial.print(",\"leftRealSpeed\":");
 Serial.print(leftWheel.getRealSpeed());
 Serial.print(",\"rightRealSpeed\":");
 Serial.print(rightWheel.getRealSpeed());
 Serial.print(",\"leftMotorSpeed\":");
 Serial.print(leftWheel.getMotorSpeed());
 Serial.print(",\"rightMotorSpeed\":");
 Serial.print(rightWheel.getMotorSpeed());
 Serial.println("}");
}

void checkCommand() {
  if (Serial.available()) {
    char cmd=char(Serial.read());
    int param = Serial.parseInt();    
     if (cmd=='M') 
       onoff=param;
     else if (cmd=='L')
       leftWheel.setSpeed(param);
     else if (cmd=='R')
       rightWheel.setSpeed(param);
     else if (cmd=='B') {
       leftWheel.setSpeed(param);
       rightWheel.setSpeed(param);
     }
     else if (cmd=='H')
       heading = param;
  }
}

void delay_check(int iter) {
  while (iter > 0) {
    checkCommand();
    if (checkOnoff()) {
      if (leftWheel.getSpeed()==0 ) {
        backTrackTempo=10;
        leftWheel.setSpeed(80);
      }
      if (rightWheel.getSpeed()==0 ) {
        backTrackTempo=10;
        rightWheel.setSpeed(80);
      }      
    }
    else {
      leftWheel.setSpeed(0);
      rightWheel.setSpeed(0);
    }
    delay(10);
    iter -=10;
  }
}

const int TURNDISTANCE=80;
const int MINDISTANCE=30;
const int LOWSPEED=30;
const int HIGHSPEED=60;

int highLowCondition(int pin,int cond) {
  if (cond )
      digitalWrite(pin,HIGH);
  else
      digitalWrite(pin,LOW);
}
void handleLight(unsigned int leftdistance,unsigned int rightdistance) {
    highLowCondition( backLeftLight,  leftdistance < MINDISTANCE);
    highLowCondition(backRightLight, rightdistance < MINDISTANCE);
    highLowCondition(frontLeftLight,  leftdistance < TURNDISTANCE);
    highLowCondition(frontRightLight,rightdistance < TURNDISTANCE);
}

void automate(int azimut,unsigned int leftdistance,unsigned int rightdistance) {
 if (backTrackTempo > 0 ) {
   backTrackTempo--;
 }
 else if (leftWheel.getRealSpeed()<5 || rightWheel.getRealSpeed()<5 ) {
  // vehicule is blocked: backtrack
         leftWheel.setSpeed( -leftWheel.getSpeed());
        rightWheel.setSpeed(-rightWheel.getSpeed());
        backTrackTempo=10;
 }

 else if (leftdistance<MINDISTANCE && (rightdistance<MINDISTANCE)) {
    if (leftdistance<rightdistance) {
         leftWheel.setSpeed(-LOWSPEED*0.8);
        rightWheel.setSpeed(-LOWSPEED*1.2);
    }
    else {
          leftWheel.setSpeed(-LOWSPEED*1.2);
        rightWheel.setSpeed(-LOWSPEED*0.8);
    }
 }
 else if (leftdistance<MINDISTANCE || (rightdistance<MINDISTANCE)) {
    if (leftdistance<MINDISTANCE) {
      if (rightWheel.getSpeed()!=0)
        rightWheel.setSpeed(-LOWSPEED);
    }
    else {
    }
    if (rightdistance<MINDISTANCE) {
      if (leftWheel.getSpeed()!=0)
        leftWheel.setSpeed(-LOWSPEED);
    }
 }
 else
  {
    if (leftdistance<TURNDISTANCE) {
      if (rightWheel.getSpeed()!=0)
        rightWheel.setSpeed(map(leftdistance,MINDISTANCE,TURNDISTANCE,LOWSPEED,HIGHSPEED));
    }
    else {
      if (rightWheel.getSpeed()!=0)
        rightWheel.setSpeed(HIGHSPEED);
    }
    if (rightdistance<TURNDISTANCE) {
      if (leftWheel.getSpeed()!=0)
       leftWheel.setSpeed(map(rightdistance,MINDISTANCE,TURNDISTANCE,LOWSPEED,HIGHSPEED));
     }
    else {
      if (leftWheel.getSpeed()!=0)
        leftWheel.setSpeed(HIGHSPEED);
    }
  }
}

void loop() {
  int azimut = getAzimut()-heading;
  unsigned int leftdistance = getDistance(leftSonar);
  unsigned int rightdistance = getDistance(rightSonar);
  handleLight(leftdistance,rightdistance);
  automate(azimut,leftdistance,rightdistance);
    
  leftWheel.loop();
  rightWheel.loop();
  
  sendData(azimut,leftdistance,rightdistance);
  prevAzimut=azimut;
  delay_check(50);
}


