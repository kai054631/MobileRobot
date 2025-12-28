//done updated
//Bt setup
#include <SoftwareSerial.h>
SoftwareSerial btSerial(12, 11);  // RX, TX
//L298N setup
#define dirL1 9
#define dirL2 10

#define dirR1 7
#define dirR2 8

#define L_speedcontrol 5
#define R_speedcontrol 6
//Ultrasonic setup
long time_taken_R;
int distance;
#define trigR
#define echoR

#define trigR 4
#define echoR 3

float speed = 0, max_speed = 80;
float val;

//encoder Setup
int Lcount_val=0;
int Rcount_val=0;
float Lrps=0,Rrps=0;
bool led_state=true;

void motor_speed_check(){

}
void set_L_motor(int speed) {
  if (speed > 0) {
    digitalWrite(dirL1, LOW);
    digitalWrite(dirL2, HIGH);
    analogWrite(L_speedcontrol, speed);
  }else
  {
    digitalWrite(dirL1, HIGH);
    digitalWrite(dirL2, LOW);
    analogWrite(L_speedcontrol, -speed);
  }
}
void set_R_motor(int speed) {
  if (speed > 0) {
    digitalWrite(dirR1, HIGH);
    digitalWrite(dirR2, LOW);
    analogWrite(R_speedcontrol, speed);
  }else
  {
    digitalWrite(dirR1, LOW);
    digitalWrite(dirR2, HIGH);
    analogWrite(R_speedcontrol, -speed);
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

// void Ultrasonic(float speed) {
//   digitalWrite(trigR, LOW);
//   delayMicroseconds(2);

//   digitalWrite(trigR, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(trigR, LOW);
//   time_taken_R = pulseIn(echoR, HIGH);
//   distance = time_taken_R * 0.034 / 2;

//   btSerial.print("Distance :");
//   btSerial.print(distance);
//   btSerial.println("CM");
//   int i = 0;

//   if (distance > 100) {
//     i = 1;
//     if (distance < 50) {
//       i = 2;
//     } else {
//       i = 3;
//     }
//   } else {
//     i = 4;
//   }
//   switch (distance) {
//     case 1:
//       forward(speed);
//       break;
//     case 2:
//       break;
//     case 3:
//       break;
//     case 4:
//       break;
//   }
// }
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
  val=40;
}


void loop() {
  // put your main code here, to run repeatedly:
   if(Lrps>5)
    {
      val-=0.05;
    }else if(Lrps<5)
    {
      val+=0.05;
    }else if(val==5)
    {
      val=val;
    }
    set_L_motor(val);  

    

  Serial.print(Lrps);
  Serial.print("  ");
  Serial.println(val/10);
  //delay(200);

}
ISR(TIMER1_COMPA_vect)
{
  TCNT1=0;
  led_state=!led_state;
  digitalWrite(13,led_state);
  Lrps=(Lcount_val/20.0)*10;
  Lcount_val=0;
  Rrps=(Rcount_val/20.0)*10;
  Rcount_val=0;
}
void Lcount()
{
  Lcount_val++;
}
void Rcount()
{
  Rcount_val++;
}
