float temp;
int tempPin = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int reading = analogRead(tempPin);
  float voltage = reading * 5.0;
  voltage /= 1024.0;
  float temperatureC = (voltage - 0.5) *100;
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  Serial.print(temperatureF); Serial.print(" degrees F ");
  delay(1000); 
  // update sensor reading each one second
  }

 // https://learn.adafruit.com/tmp36-temperature-sensor/using-a-temp-sensor
  //https://www.tutorialspoint.com/arduino/arduino_temperature_sensor.htm
