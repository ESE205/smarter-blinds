const int hallPin = 12;
int hallState = 0;

void setup() {
  Serial.begin(9600);
  pinMode(hallPin,INPUT);
}

void loop() {
  hallState = digitalRead(hallPin);
  if (hallState == LOW) {
    Serial.println("detect");
  }
  else {
    Serial.println();
  }
  delay(1000);
}
