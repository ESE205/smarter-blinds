//hall effect
const int hallPin = 12;
int hallState = 0;
bool closed = false;

String emptyString = " ";
String newString = " ";
String inputString = " ";
String otherString = " ";



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
double amount_over;

//temperature sensor
int temperaturePin = 0;
float voltage;
float temperatureC;
float temperatureF;

//photoresistor
#define aref_voltage 3.3
const int photoresistor = A1;
int value;

//user input for adjusting/setting a temperature/brightness
int bright_scale = -1000;
int ideal_temp = -1000;
int ideal_bright;
//int adjust_temp_input = -1000;
//int adjust_bright_input = -1000;
int adjust_input = -1000;
bool adjust_temp;
bool adjust_bright;
bool adjust_both;

void setup() {
  Serial.begin(9600);
  pinMode(photoresistor, INPUT);
  pinMode(hallPin, INPUT);
  pinMode(Pin0, OUTPUT);
  pinMode(Pin1, OUTPUT);
  pinMode(Pin2, OUTPUT);
  pinMode(Pin3, OUTPUT);
  while (closed == false) {
    current_steps = 0;
    hallState = digitalRead(hallPin);
    if (hallState == LOW) {
      Serial.println("detect");
      closed = true;
      //break;
    } else {
      Serial.println();
      closed = false;
      // steps_needed = 478;
      while (current_steps <= steps_needed) {
        stepperMotorFunction();
      }
      delay(100);
    }
  }

  Serial.print("Do you want to control brightness or temperature? (Enter 0 for Brightness, 1 for Temperature)");
  while (adjust_input == -1000) {
    while (Serial.available() > 0) {
      int inputChar = Serial.read();
      if (isDigit(inputChar)) {
        inputString += (char)inputChar;
      }
      adjust_input = inputString.toInt();
    }
    Serial.println(adjust_input);
    delay(200);
  }

  if (adjust_input == 0) {
    adjust_bright = true;
    adjust_temp = false;
  } else {
    adjust_bright = false;
    adjust_temp = true;
  }

  if (adjust_temp == true) {
    Serial.println(" Enter ideal temperature");
    Serial.print(Serial.available());
    while (ideal_temp == -1000) {
      while (Serial.available () > 0) {
        int tempChar = Serial.read();
        if (isDigit(tempChar)) {
          emptyString += (char)tempChar;
        }
        ideal_temp = emptyString.toInt();
      }
      Serial.println(ideal_temp);
      delay(200);
    }
  }

  if (adjust_bright == true) {
    Serial.print("Enter a number on a scale of 1-10 on level of brightness");
    Serial.print(Serial.available());
    while (bright_scale == -1000) {
      while (Serial.available() > 0) {
        int brightChar = Serial.read();
        if (isDigit(brightChar)) {
          newString += (char)brightChar;
        }
        bright_scale = newString.toInt();
      }
      Serial.println(bright_scale);
      delay(200);
    }

    ideal_bright = ((1023 / 10) * (bright_scale));
    Serial.println(ideal_bright);
    delay(1000);
  }
}


void loop() {

  /* if (Serial.available()) {
     adjust_bright_input = Serial.read() - 48;
     Serial.print(adjust_bright_input);
     break;
    }
     // adjust_temp_input = Serial.read()- 48;
    if (Serial.available()) {
     adjust_temp_input = Serial.read() - 48;
     Serial.print(adjust_temp_input);

    }

    if (adjust_bright_input == 1) {
     adjust_bright = true;
    }
    if (adjust_temp_input == 1) {
     adjust_temp = true;
    } if (adjust_temp && adjust_bright) {
     adjust_both = true;
    } else {
     adjust_both = false;
    }
  */
  /*
    if (adjust_both == true) {
      setBrightness();
      setTemperature();
    } else {
      if (adjust_temp == true && adjust_bright == false) {
        setTemperature();
      } else {
        setBrightness();
      }
    }
    delay(2000);
  */

  if (adjust_temp == true) {
    getTemperature();
    Serial.print("ideal temperature "); Serial.println(ideal_temp);
    Serial.print("current temperature "); Serial.println(temperatureF);
    Serial.println();
    delay(2000);
  }
  if (adjust_bright == true) {
    getBrightness();
    Serial.print("ideal brightness "); Serial.println(ideal_bright);
    Serial.print("current brightness "); Serial.println(value);
    Serial.println();
    delay(1000);
  }

  if (value - ideal_bright < 0) {
    dir = true;
    amount_over = (ideal_bright / value);
    steps_needed = 44836 * (1 - amount_over);
    current_steps = 0;
    while (current_steps <= steps_needed) {
      stepperMotorFunction();
    }
  } else {
    if (ideal_bright - value < 0) {
      dir = false;
      steps_needed = (44836 * (1 - (ideal_bright / value)));
      current_steps = 0;
      while (current_steps <= steps_needed) {
        stepperMotorFunction();
      }
    }
  }


}
//}

//steps should never go over max number 
//have temperature check every large enough value to process changing values 
//photoresistor more frequently 

void setBrightness () {
  Serial.print("Enter a number on a scale of 1-10 on level of brightness");
  if (Serial.available()) {
    bright_scale = Serial.read();
  }
  ideal_bright = ((1023 / 10) * (bright_scale));
  return ideal_bright;
}

void setTemperature () {
  Serial.println(" Enter ideal temperature");
  if (Serial.available ()) {
    ideal_temp = Serial.read();
  }
  return ideal_temp;
}

float getVoltage(int pin) {
  return (analogRead(pin) * (5.0 / 1024.0));
}

void getTemperature() {
  voltage = getVoltage(temperaturePin);
  temperatureC = (voltage - 0.5) * 100.0;
  temperatureF = temperatureC * (9.0 / 5.0) + 32.0;
  return temperatureF;
}

void getBrightness () {
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
  return current_steps;
}
