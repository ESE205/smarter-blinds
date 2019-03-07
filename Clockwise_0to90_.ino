int Pin0 = 8;
int Pin1 = 9;
int Pin2 = 10;
int Pin3 = 11;
int _step = 0;
int current_steps = 0;
int steps_needed = 4076; //44836 = 0 to 90 degrees
boolean dir = true;
int total_rev = 11;
int current_rev=0;


void setup() {
 pinMode(Pin0, OUTPUT);
 pinMode(Pin1, OUTPUT);
 pinMode(Pin2, OUTPUT);
 pinMode(Pin3, OUTPUT);
}

void loop() {
 while(current_rev < total_rev){
  while(current_steps<=steps_needed){
   stepperMotorFunction();
  } 
  current_rev++;
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
