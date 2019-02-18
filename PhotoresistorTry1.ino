const int photoresistor = A0;
int value;

void setup() {
  pinMode(photoresistor, INPUT);
  Serial.begin(9600);
}

void loop() {
  value = analogRead(photoresistor);
  Serial.println(value);
  
}

//reference: https://forum.arduino.cc/index.php?topic=243117.0
