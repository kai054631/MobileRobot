//Bt setup
#include <SoftwareSerial.h>
SoftwareSerial btSerial(12, 11);  // RX, TX

//L298N setup
#define dirL1 10
#define dirL2 9

#define dirR1 8
#define dirR2 7

#define L_speedcontrol 6
#define R_speedcontrol 5

//Ultrasonic setup
long time_taken_R;
int distance;
#define trigR
#define echoR

#define trigR 4
#define echoR 3

float speed1 = 0,speed2=0, max_speed = 80;
float val;

//encoder Setup
int Lcount_val=0;
int Rcount_val=0;
float Lrps=0,Rrps=0;
int PPR=20;
bool led_state=true;
volatile int gnDuration1 = 0; // Left count
volatile int gnDuration2 = 0; // Right count
float error1;
void set_L_motor(int speed1) {
  if (speed1 > 0) {
    digitalWrite(dirL1, LOW);
    digitalWrite(dirL2, HIGH);
    analogWrite(L_speedcontrol, speed1);
  }else
  {
    digitalWrite(dirL1, HIGH);
    digitalWrite(dirL2, LOW);
    analogWrite(L_speedcontrol, -speed1);
  }
}
void set_R_motor(int speed2) {
  if (speed2 > 0) {
    digitalWrite(dirR1, HIGH);
    digitalWrite(dirR2, LOW);
    analogWrite(R_speedcontrol, speed2);
  }else
  {
    digitalWrite(dirR1, LOW);
    digitalWrite(dirR2, HIGH);
    analogWrite(R_speedcontrol, -speed2);
  }
}

void Forward(float speed) {
  set_L_motor(speed);
  set_R_motor(speed);
}
void Backward(float speed) {
  set_L_motor(speed);
  set_R_motor(speed);
}
void Left_Turn(float speed) {
  set_L_motor(0);
  set_R_motor(speed);
}
void Right_Turn(float speed) {
  set_L_motor(speed);
  set_R_motor(0);
}
void Stop() {
  set_L_motor(0);
  set_R_motor(0);
}
void btcode(){
  if (btSerial.available()) {
    val = btSerial.parseFloat();
    if(val>=max_speed)
    {
      float lval=max_speed;
    }else{
      val=val;
    }
    set_L_motor(val);
    btSerial.print("The L motor speed has been set to:");
    btSerial.println(val);
    set_R_motor(val);
    btSerial.print("The R motor speed has been set to:");
    btSerial.println(val);
  }
}

void Ultrasonic(float speed) {
  digitalWrite(trigR, LOW);
  delayMicroseconds(2);

  digitalWrite(trigR, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigR, LOW);
  time_taken_R = pulseIn(echoR, HIGH);
  distance = time_taken_R * 0.034 / 2;

  btSerial.print("Distance :");
  btSerial.print(distance);
  btSerial.println("CM");
  int i = 0;

  if (distance > 100) {
    i = 1;
    if (distance < 50) {
      i = 2;
    } else {
      i = 3;
    }
  } else {
    i = 4;
  }
  switch (distance) {
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
  }
}
void setup() {
  // put your setup code here, to run once:
  //Serial Setup
  Serial.begin(19200);
  //Bt setup
  btSerial.begin(9600);
  btSerial.println("BT ready! give the speed to run");

  //L28n Setup
  pinMode(dirL1, OUTPUT);
  pinMode(dirL2, OUTPUT);

  pinMode(dirR1, OUTPUT);
  pinMode(dirR2, OUTPUT);

  pinMode(L_speedcontrol, OUTPUT);
  pinMode(R_speedcontrol, OUTPUT);

  //ultrasonic setup
  pinMode(trigR, OUTPUT);
  pinMode(echoR, INPUT);
  pinMode(13,OUTPUT);
  //encoder setup
  pinMode(2, INPUT_PULLUP);     // To connect to output of wheel encoder 1.
  pinMode(3, INPUT_PULLUP);     // To connect to output of wheel encoder 2.
  attachInterrupt(digitalPinToInterrupt(2), Lcount, RISING); 
  attachInterrupt(digitalPinToInterrupt(3), Rcount, RISING);
  noInterrupts();
  TCCR1A = 0;                 // Reset entire TCCR1A to 0 
  TCCR1B = 0;                 // Reset entire TCCR1B to 0 
  TCCR1B |= B00000100;        // prescaler set to 256
  TIMSK1 |= B00000010;        //Set OCIE1A to 1 so we enable compare match A 
  OCR1A = 6250;             //Finally we set compare register A to this value   
  interrupts();
}


void loop() {
  // put your main code here, to run repeatedly:
  set_R_motor(0);
  set_L_motor(40);
  Serial.print(Lrps);
  Serial.print("  ");
  //Serial.print(Rrps);
  //Serial.print("  ");
  Serial.print(error1);
  Serial.println("  ");
  delay(200);
}
ISR(TIMER1_COMPA_vect)
{
  TCNT1  = 0;                  //First, set the timer back to 0 so it resets for next interrupt
  led_state = !led_state;      //Invert LED state
  digitalWrite(13,led_state);   //Write new state to the LED on pin D5, this is to generate a pulse.

  // Calculate the rotation speed of the wheel encoders.
  // Number of slots on the encoder wheel is 20. So one rotation will generate 20 pulses.
  Lrps = Lcount_val/20.0;  // Estimate rotation speed in rps for wheel 1.  
  Lrps = Lrps*10.0;   // 1/0.1 = 10
  Lcount_val = 0;             // Reset counter 1.

  Rrps = (Rcount_val/PPR);  // Estimate rotation speed in rps for wheel 2.
  Rrps = Rrps*10.0;   // 1/0.1 = 10  
  Rcount_val = 0;
  int setval=2;
  error1=setval-Rrps;
}
void Lcount()
{
  Lcount_val++;
}
void Rcount()
{
  Rcount_val++;
}
