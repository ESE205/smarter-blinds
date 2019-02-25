
#define aref_voltage 3.3

const int photoresistor = A0;
int tempPin = A1;
int tempReading;
int value;

void setup() {
  Serial.begin(9600);
}

void loop() {
  tempReading = analogRead(tempPin);
  value = analogRead(photoresistor);

  Serial.print("Temp reading: ");
  Serial.print(tempReading);

  float voltage = tempReading * aref_voltage;
  voltage /= 1024.0; 

  float temperatureC = (voltage - 0.5) * 100 ;  
  Serial.print(temperatureC); 
  Serial.println(" degrees C");
  
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  Serial.print(temperatureF); 
  Serial.println(" degrees F");

  value = analogRead(photoresistor);
  Serial.println(value); 
  
  delay(1000);
}
