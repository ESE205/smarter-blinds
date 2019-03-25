//hall effect
const int hallPin = 12;
int hallState = 0;
bool closed = false;

//stepper motor
int Pin0 = 8;
int Pin1 = 9;
int Pin2 = 10;
int Pin3 = 11;
int _step = 0;
int current_steps = 0;
int steps_needed = 478; //2*2038; //(4076 is one revolution) // 478 steps is 1 degree
boolean dir = true;
int degrees_to_move;

//variables for user input
//int temp = 0;
//int light = 0;
//int userTemp = 0;
//int userLight = 0;

void setup() {
  Serial.begin(9600);
  pinMode(hallPin, INPUT);
  pinMode(Pin0, OUTPUT);
  pinMode(Pin1, OUTPUT);
  pinMode(Pin2, OUTPUT);
  pinMode(Pin3, OUTPUT);
  while (closed == false) {
  current_steps=0;
    hallState = digitalRead(hallPin);
    if (hallState == LOW) {
      Serial.println("detect");
      bool closed = true;
      break;
    } else {
      Serial.println();
      bool closed = false;
      // steps_needed = 478;
      while (current_steps <= steps_needed) {
       stepperMotorFunction();
      }
      delay(100);
    }
  }
  //userTemp = digitalRead();
  //userLight = digitalRead();
}

void loop() {
//  if (closed == true) {
//    dir = false;
//     stepperMotorFunction();
//  }
  
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
//  if (current_steps = steps_needed) {
//    Serial.print("exit");
//    break;
//  }
  delay(1);   
 // Serial.print("stepper function");
  return current_steps;
  
}
