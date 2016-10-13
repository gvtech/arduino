/* 
  Wheel control class impl
*/

#include "Wheel.h"


Wheel::Wheel(PCF8574 expander,int ctrl1,int ctrl2,int enable) {
  _ctrl1 = ctrl1;
  _ctrl2 = ctrl2;
  _enable = enable;
  _targetSpeed = 0.0;
  _realSpeed = 0.0;
  _motorSpeed = 0.0;
  _speedCount = 0;
  _lastCheckCount = 0;
  _lastCheckTime = micros(); 
  _expander = expander;
}

void Wheel::Setup() {
  _expander.pinMode(_ctrl1,OUTPUT);
  _expander.pinMode(_ctrl2,OUTPUT);
  pinMode(_enable,OUTPUT);
  _speed=0;
  _speedCount = 0;
  analogWrite(_enable,0);
   _lastCheckTime = micros();
  _motorPID.SetMode(AUTOMATIC);

}

void Wheel::incSpeedCount() {
    _speedCount+=1; 
}

void Wheel::resetSpeedCount() {
    _speedCount=0;
    _lastCheckCount=0;
    _lastCheckTime = micros();
}


int Wheel::getSpeedCount() {
  return _speedCount;
}

double Wheel::getRealSpeed() {
   return _realSpeed;
}

double Wheel::getMotorSpeed() {
   return _motorSpeed;
}

void Wheel::loop() {
  unsigned long now = micros();
  if (now >_lastCheckTime+100000 ) {
    _linreg.add(double(now),1E6*double(_speedCount));
    _realSpeed = _linreg.getCoefficient();
    _lastCheckTime = now;
    _lastCheckCount = _speedCount;
    if (_speed==0)
      _motorSpeed=0.0;
    else
      _motorPID.Compute();
    analogWrite(_enable,_motorSpeed);
  }
}

void Wheel::setSpeed(int newspeed) {
  if (_speed==newspeed) return;
  _speed=newspeed;
  if (_speed == 0 ) {
    _expander.digitalWrite(_ctrl1,LOW);
    _expander.digitalWrite(_ctrl2,LOW);
    _targetSpeed=0;
  }
  if (_speed > 0 ) {
    _expander.digitalWrite(_ctrl1,HIGH);
    _expander.digitalWrite(_ctrl2,LOW);
    _targetSpeed=_speed;
  }
  else {
    _expander.digitalWrite(_ctrl1,LOW);
    _expander.digitalWrite(_ctrl2,HIGH);
    _targetSpeed= - _speed;
  }
}
int Wheel::getSpeed() {
  return _speed;
}

