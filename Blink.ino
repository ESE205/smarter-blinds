#define LED 13
void setup() {
  pinMode(LED, OUTPUT);
  pinMode(3,INPUT);
}

void loop() {
  if (digitalRead(3)) {
    digitalWrite(LED,HIGH);
    delay(300);
    digitalWrite(LED,LOW);
    delay(300);
  } else {
  digitalWrite(LED,HIGH);
  delay(1000);
  digitalWrite(LED,LOW);
  delay(1000);
  }
}
