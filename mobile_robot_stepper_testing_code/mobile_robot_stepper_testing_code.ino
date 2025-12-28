//MOBILE ROBOT AND DRONES
//Motor Left
const int DirL=4;
const int StpL=2;

//motor Right
const int DirR=5;
const int StpR=3;

//frequency caculate from 
int speed = 1500;  //us(micro seconds) for stepper phase change 1200-3000
int step = 2;   //Microstepping for stepper


void setup() {
  // put your setup code here, to run once:
  pinMode(DirL,OUTPUT);
  pinMode(DirR,OUTPUT);
  pinMode(StpL,OUTPUT);
  pinMode(StpR,OUTPUT);
}
void forward(){
  digitalWrite(DirL,LOW);  //Left motor direction set to FORWARD 
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
// put your main code here, to run repeatedly:
  forward();
  delay(10000);
  Left_Turn();
  delay(10000);
  Right_UTurn();
  delay(10000);
}
