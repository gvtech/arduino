/* 
  Wheel control class
*/
#include <PID_v1.h>
#include <PCF8574.h>
#include <LinearRegression.h>

class Wheel {
  public:
    Wheel(PCF8574 expander,int ctrl1,int ctrl2,int enable);
    void Setup();
    void incSpeedCount();
    int getSpeedCount();
    double getRealSpeed();
    double getMotorSpeed();
    void resetSpeedCount();
    void loop();
    void setSpeed(int speed);
    int getSpeed();
  private:
    int _ctrl1,_ctrl2,_enable;
    int _speed;
    double _targetSpeed,_realSpeed,_motorSpeed;
    unsigned long _speedCount,_lastCheckCount,_lastCheckTime;
    double _Kp=2, _Ki=1, _Kd=0.2;
    PID _motorPID = PID(&_realSpeed,&_motorSpeed,&_targetSpeed,_Kp,_Ki,_Kd,DIRECT);
    RunningLinearRegression _linreg = RunningLinearRegression(5);
    PCF8574 _expander;

};


