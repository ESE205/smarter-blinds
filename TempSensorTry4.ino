int temperaturePin =0;
void setup() {
  Serial.begin(9600);
}
void loop() {
  float voltage, temperatureC, temperatureF;
  voltage = getVoltage(temperaturePin);
  temperatureC = (voltage-0.5)*100.0;
  temperatureF= temperatureC *(9.0/5.0)+32.0;
  
  Serial.print("voltage: ");
  Serial.print(voltage);
  Serial.print("  deg C: ");
  Serial.print(temperatureC);
  Serial.print("  deg F: ");
  Serial.println(temperatureF);
  delay(2000);
}

float getVoltage(int pin) { 
  return (analogRead(pin) * (5.0/1024.0));
}
