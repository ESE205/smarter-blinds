
#define aref_voltage 5

const int photoresistor = A0;
int tempPin = A1;
int tempReading;
int value;
int current_steps = 0;
int steps_needed = 2038;
boolean dir = true;
int Pin0 = 8;
int Pin1 = 9;
int Pin2 = 10;
int Pin3 = 11;
int _step = 0;

void setup() {
 Serial.begin(9600);
 pinMode(Pin0, OUTPUT);
 pinMode(Pin1, OUTPUT);
 pinMode(Pin2, OUTPUT);
 pinMode(Pin3, OUTPUT);
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

  if (value < 170) {
   while(current_steps<=steps_needed){
   stepperMotorFunction();
   }
   current_steps=0; 
  }
}

void stepperMotorFunction(){

  switch(_step){
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
 if(dir){
 _step++;
 current_steps++;
 }else{
 _step--;
 current_steps++;
 }
 if(_step>7){
 _step=0;
 }
 if(_step<0){
 _step=7;
 }
 delay(1);
 return current_steps;
}
