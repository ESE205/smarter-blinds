
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
int steps_needed = 478; //2*2038; //(4076 is one revolution) // 478 steps is 1 degree
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

//printing out on the FeatherWing
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);


void setup() {

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Command <-> Data Mode Example"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  //Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  // Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    //   Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ) {
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  // Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  // Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  //Serial.println(F("Then Enter characters to send to Bluefruit"));
  // Serial.println();

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
    //  Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  // Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));

  //end bluetooth setup

  //Featherwing Printing
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Smarter    Blinds");
  display.setCursor(0, 0);
  display.display(); // actually display all of the above

  //end Featherwing Printing

  pinMode(photoresistor, INPUT);
  pinMode(hallPin, INPUT);
  pinMode(Pin0, OUTPUT);
  pinMode(Pin1, OUTPUT);
  pinMode(Pin2, OUTPUT);
  pinMode(Pin3, OUTPUT);
  while (closed == false) {
    dir = false;
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

  ble.print("Do you want to control brightness or temperature? Enter 0 for Brightness, 1 for Temperature");
  while (adjust_input == -1000) {
    String inputString = " ";
    while (ble.available() > 0) {
      int inputChar = ble.read();
      if (isDigit(inputChar)) {
        inputString += (char)inputChar;
      }
      adjust_input = inputString.toInt();
    }
    // ble.println(adjust_input);
    delay(200);
  }

  if (adjust_input == 0) {
    adjust_bright = true;
    adjust_temp = false;
    ble.println("Adjusting Brightness");
  } else {
    adjust_bright = false;
    adjust_temp = true;
    ble.println("Adjusting Temperature");
  }

  if (adjust_temp == true) {
    ble.println(" Enter ideal temperature");
    String emptyString = " ";
    while (ideal_temp == -1000) {
      while (ble.available () > 0) {
        int tempChar = ble.read();
        if (isDigit(tempChar)) {
          emptyString += (char)tempChar;
        }
        ideal_temp = emptyString.toInt();
      }
      // ble.println(ideal_temp);
      delay(200);
    }
    ble.print("Ideal temperature: "); ble.println(ideal_temp);
    dir = true;
    current_steps = 0;
    steps_needed = (4076 * 8);
    while (current_steps <= steps_needed) {
      stepperMotorFunction();
    }
    delay(500);
  }

  if (adjust_bright == true) {
    ble.print("Enter a number on a scale of 1-5 on level of brightness, 5 being the brightest");
    while (bright_scale == -1000) {
      while (ble.available() > 0) {
        int brightChar = ble.read();
        if (isDigit(brightChar)) {
          newString += (char)brightChar;
        }
        bright_scale = newString.toInt();
        if (bright_scale > 5) {
          bright_scale = 5;
        } else {
          if (bright_scale < 1) {
            bright_scale = 1;
          }
        }
      }
      // ble.println(bright_scale);
      delay(200);
    }
    ble.print("Ideal brightness: "); ble.println(bright_scale);
    dir = true;
    steps_needed = (4076 * 5.5) + (5 - bright_scale) * (4076 * 1.1);
    while (current_steps <= steps_needed) {
      stepperMotorFunction();
    }
    current_position = bright_scale;
    ideal_bright = ((1023 / 5) * (bright_scale));
    Serial.println(ideal_bright);
    delay(1000);

  }
}


void loop() {
  while (adjust_temp == true) {
    bool new_adjustment;
    delay(2000);
    temperatureF = getTemperature();
   
    ble.println();
    delay(5000);
    ble.print("Would you like to change ideal temperature?");
    ble.println(" Enter new ideal temperature");
    delay(200);
    newString = " ";
    if (ble.available()) {
      while (temp_two == -1000) {
        while (ble.available() > 0) {
          new_adjustment = false;
          int temp2Char = ble.read();
          if (isDigit(temp2Char)) {
            newString += (char)temp2Char;
          }
          temp_two = newString.toInt();
        }
        // ble.println(temp_two);
        delay(200);
      }
      ideal_temp = temp_two;
      temp_two = -1000;
//       if (temp_two <= temperatureF + 1 && temp_two >= temperatureF - 1 && adjustment_made_cw == true && new_adjustment == false) {
//      dir = false;
//      current_steps = 0;
//      steps_needed = 4076 * 2.5;
//      while (current_steps <= steps_needed) {
//        stepperMotorFunction();
//      }
//      adjustment_made_cw = false;
//      new_adjustment = true;
//      ideal_temp = temp_two;
//      temp_two = -1000;
//    }
//    if (temp_two <= temperatureF + 1 && temp_two >= temperatureF - 1 && adjustment_made_ccw == true && new_adjustment == false) {
//      dir = true;
//      current_steps = 0;
//      steps_needed = 4076 * 2.5;
//      while (current_steps <= steps_needed) {
//        stepperMotorFunction();
//      }
//      adjustment_made_ccw = false;
//      new_adjustment = true;
//      ideal_temp = temp_two;
//      temp_two = -1000;
//    }
    }
     ble.print("ideal temperature "); ble.println(ideal_temp);
    ble.print("current temperature "); ble.println(temperatureF);
    //  ble.println(temp_two);
    // ideal_temp = temp_two;
   


    delay(2000);
    if (temperatureF <= ideal_temp + 1 && temperatureF >= ideal_temp - 1 ) { //temperature is within a 1 degree range, no movement necessary
      adjustment_made_ccw = false;
      adjustment_made_cw = false;
      delay (5000);
    } else {
      if ((temperatureF >= ideal_temp + 1) && adjustment_made_ccw == false && adjustment_made_cw == false) { // temperature is greater than ideal, and initial position, no changes made
        dir = true;
        current_steps = 0;
        steps_needed = 4076 * 2.5;
        while (current_steps <= steps_needed) {
          stepperMotorFunction();
        }
        current_steps = 0;
        adjustment_made_ccw = false;
        adjustment_made_cw = true;
        delay(2000);
      }
      if ((temperatureF <= ideal_temp - 1) && adjustment_made_ccw == false && adjustment_made_cw == false)  {
        dir = false;
        current_steps = 0;
        steps_needed = 4076 * 2.5;
        while (current_steps <= steps_needed) {
          stepperMotorFunction();
        }
        current_steps = 0;
        adjustment_made_cw = false;
        adjustment_made_ccw = true;
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
    delay(2000);
    value = getBrightness();
    if (value < 300) {
      isDark = true;
    } if (value >= 300) {
      isDark = false;
    }

    if (isClosedDarkness == false && isDark == true) {
      dir = true;
      steps_needed = (5 - current_position) * (1.1 * 4076);
      current_steps = 0;
      while (current_steps <= steps_needed) {
        stepperMotorFunction();
      }
      isClosedDarkness = true;
      // current_position = 1;
    }

    if (isClosedDarkness == true && isDark == false) {
      dir = false;
      steps_needed = (current_position) * (1.1 * 4076);
      current_steps = 0;
      while (current_steps <= steps_needed) {
        stepperMotorFunction();
      }
      isClosedDarkness = false;
    }
    //  ble.print("ideal brightness "); ble.println(ideal_bright);
    ble.print("current brightness "); ble.println(value);
    delay(5000);
    ble.println("Would you like to change the brightness?");
    ble.println(" If so, enter a number on a scale of 1-5 (5 is bright)");
    delay(200);
    if (ble.available()) {
      while (bright_scale_two == -1000) {
        while (ble.available() > 0) {
          //Serial.flush();
          int brightChar = ble.read();
          if (isDigit(brightChar)) {
            newString = (char)brightChar;
          }
          bright_scale_two = newString.toInt();
        }
        new_brightness = bright_scale_two;
        //bright_scale_two = -1000;
        // Serial.println(bright_scale_two);
        delay(200);
      }
      ble.println(new_brightness);
      bright_scale_two = -1000;
      new_brightness_adjusted = false;
      ble.print(new_brightness_adjusted);
    }


    if ((new_brightness != current_position) && (new_brightness_adjusted == false) && isClosedDarkness == false) {
      if (new_brightness > current_position) {
        dir = false;
        current_steps = 0;
        steps_needed = (new_brightness - bright_scale) * (1.1 * 4076);
        while (current_steps <= steps_needed) {
          stepperMotorFunction();
        }
      } if (new_brightness < current_position) {
        dir = true;
        current_steps = 0;
        steps_needed = (current_position - new_brightness) * (1.1 * 4076);
        while (current_steps <= steps_needed) {
          stepperMotorFunction();
        }
      }
      current_position = new_brightness;
      new_brightness_adjusted = true;
    } else {
      current_position = bright_scale;
      delay (2000);
    }
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
