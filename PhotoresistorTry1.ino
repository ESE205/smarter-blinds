const int photoresistor = 12;
int value;

void setup() {
  pinMode(photoresistor, INPUT);
}

void loop() {
  value = analogRead(photoresistor);
  Serial.println(value);
  
}

//reference: https://forum.arduino.cc/index.php?topic=243117.0
