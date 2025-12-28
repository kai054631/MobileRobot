#include <SoftwareSerial.h>
#include <string.h>

// SoftwareSerial RX, TX pins
SoftwareSerial BT(10, 11);   // RX = 10, TX = 11

//400/22=18.18for 1cm
//Motor Left
const int DirL=3;
const int StpL=2;

//motor Right
const int DirR=4;
const int StpR=5;


int speed=1500;  //us(micro seconds) for stepper phase change 1200-3000
int step=2;   //Microstepping for stepper

void setup() {
  Serial.begin(9600);  // Monitor
  BT.begin(9600);      // HC-06 baud rate
  pinMode(DirL,OUTPUT);
  pinMode(DirR,OUTPUT);
  pinMode(StpL,OUTPUT);
  pinMode(StpR,OUTPUT);
  Serial.println("HC-06 Bluetooth Ready! Connect using your phone.");
}

void forward(int distance){ //call and give cm wanted to move.
  int val=18.18*distance
  digitalWrite(DirL,LOW);  //Left motor direction set to FORWARD 
  digitalWrite(DirR,LOW);  //Left motor direction set to FORWARD
  for(int x = 0; x < val; x++) { //Rotation for 1 
    digitalWrite(StpL,HIGH);  //Left stepper motor step on
    digitalWrite(StpR,HIGH);  //Right stepper motor step on
    delayMicroseconds(speed); 
    digitalWrite(StpL,LOW);   //Left stepper motor step off
    digitalWrite(StpR,LOW);   //Right stepper motor step off
    delayMicroseconds(speed);
  }
}

void Backward(){
  digitalWrite(DirL,HIGH);  //Left motor direction set to FORWARD 
  digitalWrite(DirR,HIGH);  //Left motor direction set to FORWARD
  for(int x = 0; x < 200*step; x++) { //Rotation for 1 
    digitalWrite(StpL,HIGH);  //Left stepper motor step on
    digitalWrite(StpR,HIGH);  //Right stepper motor step on
    delayMicroseconds(speed); 
    digitalWrite(StpL,LOW);   //Left stepper motor step off
    digitalWrite(StpR,LOW);   //Right stepper motor step off
    delayMicroseconds(speed);
  }
}
void Left_Turn() { //left turn(left motor static)
  digitalWrite(DirL,LOW);  //Left motor direction set to FORWARD 
  digitalWrite(DirR,LOW);  //Left motor direction set to FORWARD
  for(int x = 0; x < 200*step; x++) { //Rotation for 1 
    digitalWrite(StpR,HIGH);  //Right stepper motor step on
    delayMicroseconds(speed); 
    digitalWrite(StpR,LOW);   //Right stepper motor step off
    delayMicroseconds(speed);
  }
}

void Right_Turn() { //left turn(left motor static)
  digitalWrite(DirL,LOW);  //Left motor direction set to FORWARD 
  digitalWrite(DirR,LOW);  //Left motor direction set to FORWARD
  for(int x = 0; x < 200*step; x++) { //Rotation for 1 
    digitalWrite(StpL,HIGH);  //Right stepper motor step on
    delayMicroseconds(speed); 
    digitalWrite(StpL,LOW);   //Right stepper motor step off
    delayMicroseconds(speed);
  }
}
void Left_UTurn() { //left turn(left motor static)
  digitalWrite(DirL,HIGH);  //Left motor direction set to BACKWARD 
  digitalWrite(DirR,LOW);  //Left motor direction set to FORWARD
  for(int x = 0; x < 200*step; x++) { //Rotation for 1 
    digitalWrite(StpL,HIGH);  //Left stepper motor step on
    digitalWrite(StpR,HIGH);  //Right stepper motor step on
    delayMicroseconds(speed); 
    digitalWrite(StpL,LOW);   //Left stepper motor step off
    digitalWrite(StpR,LOW);   //Right stepper motor step off
    delayMicroseconds(speed);
  }
}

void Right_UTurn() { //left turn(left motor static)
  digitalWrite(DirL,LOW);  //Left motor direction set to FORWARD 
  digitalWrite(DirR,HIGH);  //Left motor direction set to BACKWARD
  for(int x = 0; x < 200*step; x++) { //Rotation for 1 
    digitalWrite(StpL,HIGH);  //Left stepper motor step on
    digitalWrite(StpR,HIGH);  //Right stepper motor step on
    delayMicroseconds(speed); 
    digitalWrite(StpL,LOW);   //Left stepper motor step off
    digitalWrite(StpR,LOW);   //Right stepper motor step off
    delayMicroseconds(speed);
  }
}

void loop() {
  // Receive from phone → PC
  char c;
  if (BT.available()) {
    c = BT.read();
    Serial.print("Phone: ");
    Serial.println(c);
  }
  if( c == 'F'){
    forward();
    delay(50000);
    c="l";
  }
  else if( c == 'B'){
    Backward();
    //delay(5000);
    c="l";
  }
  else if( c == 'L'){
    Left_Turn();
    //delay(5000);
    c="l";
  }
  else if( c == 'R'){
    Right_Turn();
    //delay(5000);
    c="l";
  }
  // Send from PC → phone
  if (Serial.available()) {
    char c = Serial.read();
    BT.print(c);
    }
}
