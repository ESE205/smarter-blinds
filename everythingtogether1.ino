
#define aref_voltage 5

const int photoresistor = 1;
int temperaturePin = 0;
int current_steps = 0;
int steps_needed = 2038; //update here 
boolean dir = true;
int Pin0 = 8;
int Pin1 = 9;
int Pin2 = 10;
int Pin3 = 11;
int _step = 0;
int steps = 0;
int value;
boolean userInput = false;

void setup() {
 Serial.begin(9600);
 pinMode(Pin0, OUTPUT);
 pinMode(Pin1, OUTPUT);
 pinMode(Pin2, OUTPUT);
 pinMode(Pin3, OUTPUT);
}

void loop() {
  float voltage, temperatureC, temperatureF;
  voltage = getVoltage(temperaturePin);
  temperatureC = (voltage-0.5)*100.0;
  temperatureF= temperatureC *(9.0/5.0)+32.0;
  value = analogRead(photoresistor);

  Serial.print("voltage: ");
  Serial.print(voltage);
  Serial.print("  deg C: ");
  Serial.print(temperatureC);
  Serial.print("  deg F: ");
  Serial.print(temperatureF);
  Serial.print(" Photoresistor: ");
  Serial.println(value);


  
  delay(1000);
  if (userInput) {
    //
  } else {
    //Increase Temperature, Open Blinds
    //Increase Brightness, Open blinds
  if (value < 180 || temperatureF > 80) {
    //update steps_needed here 
    dir = true;
  stepperMotorFunction();
   current_steps=0; 
  } 
    //Decrease Temperature, Close Blinds
    //Decrease Brightness, Close Blinds
  if (value > 160 || temperatureF < 70){
   dir = false;
   stepperMotorFunction();
  }
}

}

void stepperMotorFunction(){
for (int i =0; i < steps_needed; i++){
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
}
 return current_steps;
}

float getVoltage(int pin) { 
  return (analogRead(pin) * (5.0/1024.0));
}
