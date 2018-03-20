#include <Servo.h>

Servo servopitch;
Servo servoyaw;

int curyaw=120;
int curpitch=120;
int targetyaw=120;
int targetpitch=120;
int speed=10;

void setup() {
  pinMode( 3,OUTPUT);
  pinMode( 5,OUTPUT);
  servopitch.attach(3);
  servoyaw.attach(5);
  Serial.begin(9600);
  servopitch.write(curpitch);
  servoyaw.write(curyaw);
}

void loop() {
    if (Serial.available()) {
      char cmd=char(Serial.read());
      int param = Serial.parseInt();
      Serial.print("command:");
      Serial.print(cmd);
      Serial.print(" param:");
      Serial.println(param);

     if (cmd=='Y')
        targetyaw=constrain(param, 20, 160);
     if (cmd=='P')
        targetpitch=constrain(param, 100, 150);
     if (cmd=='S')
        speed=constrain(param, 1, 20);
    }
   if (curpitch<targetpitch) {
      curpitch++;
      servopitch.write(curpitch);
      }
   if (curpitch>targetpitch) {
      curpitch--;
      servopitch.write(curpitch);
      }
   if (curyaw<targetyaw) {
      curyaw++;
      servoyaw.write(curyaw);
      }
   if (curyaw>targetyaw) {
      curyaw--;
      servoyaw.write(curyaw);
      }


    delay(100/speed);

}
