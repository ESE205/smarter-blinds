
#include <Arduino.h>

//hall effect
const int hallPin = 9; //old is 12
int hallState = 0;
bool closed = false;

//String emptyString = " ";
String newString = " ";
//String inputString = " ";
String otherString = " ";

//stepper motor
int Pin0 = A2; // 8
int Pin1 = A3; //9
int Pin2 = A4; //10
int Pin3 = A5; // 11
int _step = 0;
int current_steps = 0;
int steps_needed = 478*50; //2*2038; //(4076 is one revolution) // 478 steps is 1 degree
boolean dir = true; //true = counter clockwise 


//temperature sensor
int temperaturePin = A0;
float voltage;
double temperatureC;
double temperatureF;
bool adjustment_made_ccw = false;
bool adjustment_made_cw = false;
bool isDark;
bool isClosedDarkness = false;

//photoresistor
//#define aref_voltage 3.3
const int photoresistor = A1;
int value;
int current_position;

//user input for adjusting/setting a temperature/brightness
int bright_scale = -1000;
int ideal_temp = -1000;
int ideal_bright;
int temp_two = -1000;
int adjust_input = -1000;
bool adjust_temp;
bool adjust_bright;
bool adjust_both;
int bright_scale_two = -1000;
//int ideal_bright_two;
int new_brightness = -1000;
bool new_brightness_adjusted = true;


void setup() {


  pinMode(photoresistor, INPUT);
  pinMode(hallPin, INPUT);
  pinMode(Pin0, OUTPUT);
  pinMode(Pin1, OUTPUT);
  pinMode(Pin2, OUTPUT);
  pinMode(Pin3, OUTPUT);
 dir = true;

 
}


void loop() {
    while (current_steps <= steps_needed) {
        stepperMotorFunction();
      }
  
}

float getVoltage(int pin) {
  // ble.println(analogRead(pin));
  float reading = (analogRead(pin) * 3.3);
  reading /= 1024.0;
  return (reading);
}

double getTemperature() {
  voltage = getVoltage(temperaturePin);
  //ble.println(voltage);
  temperatureC = ((voltage - .5) * 100);
  // Serial.println(temperatureC);
  temperatureF = temperatureC * (9.0 / 5.0) + 32.0;
  return temperatureF;
}

int getBrightness () {
  value = analogRead(photoresistor);
  return value;
}

void stepperMotorFunction() {
  switch (_step) {
    case 0:
      digitalWrite(Pin0, LOW);
      digitalWrite(Pin1, LOW);
      digitalWrite(Pin2, LOW);
      digitalWrite(Pin3, HIGH);
      break;
    case 1:
      digitalWrite(Pin0, LOW);
      digitalWrite(Pin1, LOW);
      digitalWrite(Pin2, HIGH);
      digitalWrite(Pin3, HIGH);
      break;
    case 2:
      digitalWrite(Pin0, LOW);
      digitalWrite(Pin1, LOW);
      digitalWrite(Pin2, HIGH);
      digitalWrite(Pin3, LOW);
      break;
    case 3:
      digitalWrite(Pin0, LOW);
      digitalWrite(Pin1, HIGH);
      digitalWrite(Pin2, HIGH);
      digitalWrite(Pin3, LOW);
      break;
    case 4:
      digitalWrite(Pin0, LOW);
      digitalWrite(Pin1, HIGH);
      digitalWrite(Pin2, LOW);
      digitalWrite(Pin3, LOW);
      break;
    case 5:
      digitalWrite(Pin0, HIGH);
      digitalWrite(Pin1, HIGH);
      digitalWrite(Pin2, LOW);
      digitalWrite(Pin3, LOW);
      break;
    case 6:
      digitalWrite(Pin0, HIGH);
      digitalWrite(Pin1, LOW);
      digitalWrite(Pin2, LOW);
      digitalWrite(Pin3, LOW);
      break;
    case 7:
      digitalWrite(Pin0, HIGH);
      digitalWrite(Pin1, LOW);
      digitalWrite(Pin2, LOW);
      digitalWrite(Pin3, HIGH);
      break;
    default:
      digitalWrite(Pin0, LOW);
      digitalWrite(Pin1, LOW);
      digitalWrite(Pin2, LOW);
      digitalWrite(Pin3, LOW);
      break;
  }
  if (dir) {
    _step++;
    current_steps++;
  } else {
    _step--;
    current_steps++;
  }
  if (_step > 7) {
    _step = 0;
  }
  if (_step < 0) {
    _step = 7;
  }
  delay(1);
  digitalWrite(Pin0, LOW);
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, LOW);
  return current_steps;
}
