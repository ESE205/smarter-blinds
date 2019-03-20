int Pin0 = 8;
int Pin1 = 9;
int Pin2 = 10;
int Pin3 = 11;
int _step = 0;
int current_steps = 0;
int steps_needed; //2*2038; //(4076 is one revolution)
boolean dir = true;
int degrees_to_move;


void setup() {
 Serial.begin(9600);
 pinMode(Pin0, OUTPUT);
 pinMode(Pin1, OUTPUT);
 pinMode(Pin2, OUTPUT);
 pinMode(Pin3, OUTPUT);
}

void loop() {
if (Serial.available()) {
degrees_to_move = Serial.read() - 48;
Serial.print(degrees_to_move );
}
steps_needed = (478)*(degrees_to_move);
  while(current_steps<=steps_needed){
   stepperMotorFunction();
  } 
degrees_to_move=0;
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
