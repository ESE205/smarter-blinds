#include <Stepper.h>
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5
int Steps = 200;
int cstep = 0;
Stepper motor(400, 2, 3, 4, 5);

void setup() {
  // put your setup code here, to run once:
  motor.setSpeed(100);

}
//Stepper(steps, pin1, pin2)
//Stepper(steps, pin1, pin2, pin3, pin4)
//setSpeed(rpm)
//step(steps)

void loop() {
  // put your main code here, to run repeatedly:
  
  motor.step(Steps); //clockwise
  delay(500);
  motor.step(-Steps); //counter clockwise
  delay(500); 

}
//reference: https://www.arduino.cc/en/Tutorial/StepperOneRevolution
