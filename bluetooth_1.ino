/*********************************************************************
  This is an example for our nRF51822 based Bluefruit LE modules
*********************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#if SOFTWARE_ble_AVAILABLE
#include <Softwareble.h>
#endif



    #define FACTORYRESET_ENABLE         1
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
  /*=========================================================================*/
/* ...or hardware ble, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWble_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  ble.println(err);
  while (1);
}


//hall effect
const int hallPin = 9; //old is 12
int hallState = 0;
bool closed = false;

String emptyString = " ";
String newString = " ";
String inputString = " ";
String otherString = " ";

//stepper motor
int Pin0 = A2; // 8
int Pin1 = A3; //9
int Pin2 = A4; //10
int Pin3 = A5; // 11
int _step = 0;
int current_steps = 0;
int steps_needed = 478; //2*2038; //(4076 is one revolution) // 478 steps is 1 degree
boolean dir = true;
int degrees_to_move;
double amount_over;

//temperature sensor
int temperaturePin = A0;
float voltage;
float temperatureC;
float temperatureF;
bool adjustment_made_ccw;
bool adjustment_made_cw;
bool isDark;
bool isClosedDarkness;

//photoresistor
//#define aref_voltage 3.3
const int photoresistor = A1;
int value;
int new_position;

//user input for adjusting/setting a temperature/brightness
int bright_scale = -1000;
int ideal_temp = -1000;
int ideal_bright;
int temp_two = -1000;
//int adjust_temp_input = -1000;
//int adjust_bright_input = -1000;
int adjust_input = -1000;
bool adjust_temp;
bool adjust_bright;
bool adjust_both;
int bright_scale_two = -1000;
int ideal_bright_two;

void setup() {

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Command <-> Data Mode Example"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ) {
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
    delay(500);
  }

  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));

  //end bluetooth setup



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

    //    dir = false;
    //    steps_needed = (4076 * 5.5);
    //    while (current_steps <= steps_needed) {
    //      stepperMotorFunction();
    //    }
  }

  ble.print("Do you want to control brightness or temperature? (Enter 0 for Brightness, 1 for Temperature)");
  while (adjust_input == -1000) {
    while (ble.available() > 0) {
      int inputChar = ble.read();
      if (isDigit(inputChar)) {
        inputString += (char)inputChar;
      }
      adjust_input = inputString.toInt();
    }
    ble.println(adjust_input);
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
    ble.println(" Enter ideal temperature");
    while (ideal_temp == -1000) {
      while (ble.available () > 0) {
        int tempChar = ble.read();
        if (isDigit(tempChar)) {
          emptyString += (char)tempChar;
        }
        ideal_temp = emptyString.toInt();
      }
      ble.println(ideal_temp);
      delay(200);
    }
    dir = false;
    steps_needed = (4076 * 5.5);
    while (current_steps <= steps_needed) {
      stepperMotorFunction();
    }
  }

  if (adjust_bright == true) {
    ble.print("Enter a number on a scale of 1-5 on level of brightness");
    while (bright_scale == -1000) {
      while (ble.available() > 0) {
        int brightChar = ble.read();
        if (isDigit(brightChar)) {
          newString += (char)brightChar;
        }
        bright_scale = newString.toInt();
      }
      ble.println(bright_scale);
      delay(200);
    }
    dir = false;
    steps_needed = (4076 * 5.5) + (5 - bright_scale) * (4076);
    while (current_steps <= steps_needed) {
      stepperMotorFunction();
    }

    ideal_bright = ((1023 / 5) * (bright_scale));
    Serial.println(ideal_bright);
    delay(1000);

  }
}


void loop() {
  while (adjust_temp == true) {
    temperatureF = getTemperature();
    Serial.print("ideal temperature "); Serial.println(ideal_temp);
    Serial.print("current temperature "); Serial.println(temperatureF);
    Serial.println();
    Serial.print("Would you like to change ideal temperature?");
    Serial.println(" Enter ideal temperature");
    if (Serial.available()) {
      while (temp_two == -1000) {
        while (Serial.available() > 0) {
          int temp2Char = Serial.read();
          if (isDigit(temp2Char)) {
            newString = (char)temp2Char;
          }
          temp_two = newString.toInt();
        }
        Serial.println(temp_two);
        delay(200);
      }
      ideal_temp = temp_two;
    }

    delay(2000);
    if (temperatureF <= ideal_temp + 1 && temperatureF >= ideal_temp - 1 ) { //temperature is within a 1 degree range, no movement necessary
      adjustment_made_ccw = false;
      adjustment_made_cw = false;
      delay (5000);
    } else {
      if ((temperatureF >= ideal_temp + 1) && adjustment_made_ccw == false && adjustment_made_cw == false) { // temperature is greater than ideal, and initial position, no changes made
        dir = false;
        current_steps = 0;
        steps_needed = 4076 * 2.5;
        while (current_steps <= steps_needed) {
          stepperMotorFunction();
        }
        current_steps = 0;
        adjustment_made_ccw = true;
        adjustment_made_cw = false;
        delay(2000);
      }
      if ((temperatureF <= ideal_temp - 1) && adjustment_made_ccw == false && adjustment_made_cw == false)  {
        dir = true;
        current_steps = 0;
        steps_needed = 4076 * 2.5;
        while (current_steps <= steps_needed) {
          stepperMotorFunction();
        }
        current_steps = 0;
        adjustment_made_cw = true;
        adjustment_made_ccw = false;
        delay(2000);
      }

      if ((temperatureF <= ideal_temp + 1) && (temperatureF >= ideal_temp - 1) && (adjustment_made_ccw == true)) {
        current_steps = 0;
        dir = true;
        steps_needed = 4076 * 2.5;
        while (current_steps <= steps_needed) {
          stepperMotorFunction();
        }
        current_steps = 0;
        adjustment_made_cw = false;
        adjustment_made_ccw = false;
        delay(2000);
      }

      if ((temperatureF <= ideal_temp + 1) && (temperatureF >= ideal_temp - 1) && (adjustment_made_cw == true)) {
        dir = false;
        current_steps = 0;
        steps_needed = 4076 * 2.5;
        while (current_steps <= steps_needed) {
          stepperMotorFunction();
        }
        current_steps = 0;
        adjustment_made_cw = false;
        adjustment_made_ccw = false;
        delay(2000);
      }
    }
  }

  while (adjust_bright == true) {
    value = getBrightness();
    if (value < 300) {
      isDark = true;
      isClosedDarkness = false;
    } else {
      isDark = false;
      isClosedDarkness = false;
    }
    Serial.print("ideal brightness "); Serial.println(ideal_bright);
    Serial.print("current brightness "); Serial.println(value);
    Serial.println("Would you like to change the brightness?");
    Serial.println(" If so, enter a number on a scale of 1-5 (5 is bright)");
    if (Serial.available()) {
      while (bright_scale_two == -1000) {
        while (Serial.available() > 0) {
          Serial.flush();
          int brightChar = Serial.read();
          if (isDigit(brightChar)) {
            newString = (char)brightChar;
          }
          bright_scale_two = newString.toInt();
        }
        Serial.println(bright_scale_two);
        delay(200);
      }
    }

    //    ideal_bright_two = ((1023 / 5) * (bright_scale_two));
    //    Serial.println(ideal_bright_two);
    //    delay(1000);

    Serial.println();
    delay(1000);


    //using the first input of ideal temperature
    if (isClosedDarkness == false && bright_scale_two == -1000) {
      dir = false;
      steps_needed = (5 - bright_scale) * 4076;
      while (current_steps <= steps_needed) {
        stepperMotorFunction();
      }
    }

    if ((isDark == true) && (isClosedDarkness == false)) {
      current_steps = 0;
      steps_needed = 4076 * (5 - new_position);
      while (current_steps <= steps_needed) {
        stepperMotorFunction();
      }
      isClosedDarkness = true;
    } else {

    }

    if (isClosedDarkness == true) {
      dir = true;
      steps_needed = (5.5) * (4076);
      current_steps = 0;
      while (current_steps <= steps_needed) {
        stepperMotorFunction();
      }
      isClosedDarkness = false;
    }

    if ((bright_scale_two != bright_scale) && (bright_scale_two != -1000) && isClosedDarkness == false) {
      if (bright_scale_two > bright_scale) {
        dir = false;
        current_steps = 0;
        delay(2000);
        steps_needed = (bright_scale_two - bright_scale) * 4076;
        while (current_steps <= steps_needed) {
          stepperMotorFunction();
        }
        new_position = bright_scale_two;
      } else {
        dir = true;
        current_steps = 0;
        steps_needed = (bright_scale - bright_scale_two) * 4076;
        delay(2000);
        while (current_steps <= steps_needed) {
          stepperMotorFunction();
        }
        new_position = bright_scale;
      }
    } else {
      delay (2000);
    }
  }
}


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
  Serial.println(analogRead(pin));
  float reading = (analogRead(pin) * 3.3);
  reading /= 1024.0;
  return (reading);
}

float getTemperature() {
  voltage = getVoltage(temperaturePin);
  Serial.println(voltage);
  temperatureC = ((voltage - .5) * 100);
  Serial.println(temperatureC);
  temperatureF = temperatureC * (9.0 / 5.0) + 32.0;
  return temperatureF;
}

float getBrightness () {
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
