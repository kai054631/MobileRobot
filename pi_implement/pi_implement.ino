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

// #define trigR 4
// #define echoR 3

int speed1 = 0,speed2=0, max_speed = 80;
float val;

//encoder Setup
float Lcount_val=0, Rcount_val=0;
float Lrps=0,Rrps=0;
float PPR=20.0;
bool led_state=true;
float fsetval=5.0;
float error1,errorSum=0.0,error1_old;
float fpidOut;
float Kp=1.0,Ki=0.05;
void set_L_motor(int speed) {
  int pwm_val = constrain(speed, 0, 255);
  if(pwm_val>0 &&pwm_val<50)
  {
    pwm_val=60;
  }
  if (speed > 0) {
    digitalWrite(dirL1, LOW);
    digitalWrite(dirL2, HIGH);
    analogWrite(L_speedcontrol, pwm_val);
  }else if(speed<-20){
    digitalWrite(dirL1, LOW);
    digitalWrite(dirL2, HIGH);
    analogWrite(L_speedcontrol, pwm_val);
    
  } else{
    analogWrite(L_speedcontrol, 0);
  }

}
// void set_R_motor(int speed2) {
//   if (speed2 > 0) {
//     digitalWrite(dirR1, HIGH);
//     digitalWrite(dirR2, LOW);
//     analogWrite(R_speedcontrol, speed2);
//   }else
//   {
//     digitalWrite(dirR1, LOW);
//     digitalWrite(dirR2, HIGH);
//     analogWrite(R_speedcontrol, -speed2);
//   }
// }
void setup() {
  // put your setup code here, to run once:
  //Serial Setup
  Serial.begin(9600);
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
  // pinMode(trigR, OUTPUT);
  // pinMode(echoR, INPUT);
  pinMode(13,OUTPUT);
  //encoder setup
  pinMode(2, INPUT_PULLUP);     // To connect to output of wheel encoder 1.
  pinMode(3, INPUT_PULLUP);     // To connect to output of wheel encoder 2.
  attachInterrupt(digitalPinToInterrupt(3), Lcount, RISING); 
  attachInterrupt(digitalPinToInterrupt(2), Rcount, RISING);
  noInterrupts();
  TCCR1A = 0;                 // Reset entire TCCR1A to 0 
  TCCR1B = 0;                 // Reset entire TCCR1B to 0 
  TCCR1B |= B00000100;        // prescaler set to 256
  TIMSK1 |= B00000010;        //Set OCIE1A to 1 so we enable compare match A 
  OCR1A = 18750;             //Finally we set compare register A to this value   
  interrupts();
}

void loop() {
  // put your main code here, to run repeatedly:
  //set_L_motor(100);
  //Serial.print(-5);
  Serial.print("  ");
  //Serial.print("Wheel Speed: ");
  Serial.print(Lrps);
  Serial.print("  ");
  //Serial.print("error ");
  Serial.print(error1);
  Serial.print("  ");
  Serial.print(fsetval);
  Serial.println(" ");
  //Serial.println(5);
  delay(100);
  
}
ISR(TIMER1_COMPA_vect)
{
  TCNT1  = 0;                  //First, set the timer back to 0 so it resets for next interrupt
  led_state = !led_state;      //Invert LED state
  digitalWrite(13,led_state);   //Write new state to the LED on pin D5, this is to generate a pulse.

  // Calculate the rotation speed of the wheel encoders.
  // Number of slots on the encoder wheel is 20. So one rotation will generate 20 pulses.
  Lrps = Lcount_val/20.0;  // Estimate rotation speed in rps for wheel 1.  
  Lrps = Lrps*3.33;   // 1/0.2 = 10
  Lcount_val = 0;             // Reset counter 1.

  Rrps = Rcount_val/20.0;  // Estimate rotation speed in rps for wheel 2.
  Rrps = Rrps*3.33;   // 1/0.2 = 5  
  Rcount_val = 0;

  error1=fsetval-Lrps;
  errorSum+=error1;
  errorSum = constrain(errorSum, -30, 30);
  fpidOut = (Kp*error1)+(Ki*errorSum);
  set_L_motor(fpidOut);
}
void Lcount()
{
  Lcount_val++;
}
void Rcount()
{
  Rcount_val++;
}
