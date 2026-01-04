//Bt setup
#include <SoftwareSerial.h>
SoftwareSerial btSerial(12, 11);  // RX, TX

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


//encoder Setup
float Lcount_val = 0.0, 
      Lrps = 0.0, 
      errorL = 0.0, 
      errorLSum = 0.0, 
      error_old = 0.0, 
      errortest=0.0,
      fpidLOut = 0.0;
float LKp = 6.0, LKi = 0.6, LKd = 0.0;  //pid value for left motor
float Rcount_val = 0.0, Rrps = 0.0, errorR = 0.0, errorRSum = 0.0, errorR_old = 0.0, fpidROut = 0.0;
float RKp = 2.0, RKi = 0.2, RKd = 0.0;  //pid value for left motor
/*
Count_val = pulse count by Encoder
rps       = Wheel rotation pre seconder
error     = error between RPS set and actual motor speed.(kp)
Error sum = total error(ki)
error old = error of last time frame 
fpidout   = Pid value to drive motor 
*/
float PPR = 20.0;       //pulse per rotation
bool led_state = true;  //Heart beat of the interrupt
float fsetval = 15.0;   //set motor speed to 5round per seconds


void set_L_motor(int speed) {
  int pwm_val = constrain(speed, 0, 255);
  if (pwm_val > 0 && pwm_val < 50) {
    pwm_val = 60;
  }
  if (speed > 0) {
    digitalWrite(dirL1, LOW);
    digitalWrite(dirL2, HIGH);
    analogWrite(L_speedcontrol, pwm_val);
  } else if (speed <0) {
    digitalWrite(dirL1, LOW);
    digitalWrite(dirL2, HIGH);
    analogWrite(L_speedcontrol, pwm_val);
  } else {
    analogWrite(L_speedcontrol, 0);
  }
}
void set_R_motor(int speed) {
  int pwm_val = constrain(speed, 60, 255);
  if (pwm_val > 0 && pwm_val < 50) {
    pwm_val = 60;
  }
  if (speed > 0) {
    digitalWrite(dirR1, LOW);
    digitalWrite(dirR2, HIGH);
    analogWrite(R_speedcontrol, pwm_val);
  } else if (speed < -10) {
    digitalWrite(dirR1, LOW);
    digitalWrite(dirR2, HIGH);
    analogWrite(R_speedcontrol, pwm_val);
  } else {
    analogWrite(R_speedcontrol, 0);
  }
}
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
  pinMode(13, OUTPUT);
  //encoder setup
  pinMode(2, INPUT_PULLUP);  // To connect to output of wheel encoder 1.
  pinMode(3, INPUT_PULLUP);  // To connect to output of wheel encoder 2.

  attachInterrupt(digitalPinToInterrupt(3), Lcount, RISING);
  attachInterrupt(digitalPinToInterrupt(2), Rcount, RISING);
  noInterrupts();
  TCCR1A = 0;           // Reset entire TCCR1A to 0
  TCCR1B = 0;           // Reset entire TCCR1B to 0
  TCCR1B |= B00000100;  // prescaler set to 256
  TIMSK1 |= B00000010;  //Set OCIE1A to 1 so we enable compare match A
  OCR1A = 18750;        //Finally we set compare register A to this value
  interrupts();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(Lrps);
  Serial.print("  ");
  Serial.print(Rrps);
  Serial.print("  ");
  Serial.print(errorL);
  Serial.print("  ");
  Serial.print(errorR);
  Serial.print("  ");
  Serial.print(fpidLOut);
  Serial.print("  ");
  Serial.print(fpidROut);
  Serial.print("  ");
  Serial.println(fsetval);
  delay(100);
}
ISR(TIMER1_COMPA_vect) {
  TCNT1 = 0;                    //First, set the timer back to 0 so it resets for next interrupt
  led_state = !led_state;       //Invert LED state
  digitalWrite(13, led_state);  //Write new state to the LED on pin D5, this is to generate a pulse.

  Lrps = Lcount_val / 20.0;  // Estimate rotation speed in rps for wheel 1.
  Lrps = Lrps * 3.33;        // 1/0.3 = 3.33
  Lcount_val = 0;            // Reset counter 1.

  errorL = fsetval - Lrps;
  errorLSum += errorL;
  errortest=(errorL-error_old)/0.3;
  fpidLOut = (LKp * errorL) + (LKi * errorLSum)+(LKd*errortest);
  set_L_motor(fpidLOut);
  error_old=errorL;

  Rrps = Rcount_val / 20.0;  // Estimate rotation speed in rps for wheel 2.
  Rrps = Rrps * 3.33;        //  1/0.3 = 3.33
  Rcount_val = 0;

  errorR = fsetval - Rrps;
  errorRSum += errorR;
  errorRSum = constrain(errorRSum, -30, 30);
  fpidROut = (RKp * errorR) + (RKi * errorRSum);
  //set_R_motor(fpidROut);
}
void Lcount() {
  Lcount_val++;
}
void Rcount() {
  Rcount_val++;
}


so this is my code