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

bool led_state = true;
//encoder Setup
volatile long Lcount_val = 0;
float Lrps_smooth = 0.0;
float Lrps_hist[5] = { 0, 0, 0, 0, 0};
float errorL = 0.0, errorLSum = 0.0, error_old = 0.0, fpidLOut = 0.0;
// NEW TUNING FOR 3S DIRECT POWER
float LKp = 0.0;  // Lowered to prevent 12V "panic"
float LKi = 0.0;  // Lowered for stability
float LKd = 0.0;
float fsetval = 2.0;

void set_L_motor(int speed) {
  int pwm_val = abs((int)speed);
  if (pwm_val > 150) {
    pwm_val = 150;
  }
  
  if (speed > 0.1) {
    if (pwm_val > 0 && pwm_val < 60) {
      pwm_val = 60;
    }
    digitalWrite(dirL1, LOW);
    digitalWrite(dirL2, HIGH);
    analogWrite(L_speedcontrol, pwm_val);
  } else if (speed < -15) {
    if (pwm_val <55) {
      pwm_val = 55;
    }
    digitalWrite(dirL1, HIGH);
    digitalWrite(dirL2, LOW);
    analogWrite(L_speedcontrol, pwm_val);
  } else {
    digitalWrite(dirL1, LOW);
    digitalWrite(dirL2, LOW);
    analogWrite(L_speedcontrol, 0);
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
  pinMode(2, INPUT_PULLUP);  // To connect to output of wheel encoder 1.
  pinMode(3, INPUT_PULLUP);  // To connect to output of wheel encoder 2.

  attachInterrupt(digitalPinToInterrupt(3), Lcount, RISING);
  //attachInterrupt(digitalPinToInterrupt(2), Rcount, RISING);
  noInterrupts();
  TCCR1A = 0;           // Reset entire TCCR1A to 0
  TCCR1B = 0;           // Reset entire TCCR1B to 0
  TCCR1B |= B00000100;  // prescaler set to 256
  TIMSK1 |= B00000010;  //Set OCIE1A to 1 so we enable compare match A
  OCR1A = 12500;        //Finally we set compare register A to this value
  interrupts();
}

void loop() {
// 1. Handle Serial Commands for Tuning
  if (Serial.available() > 0) {
    char type = Serial.read(); // Read the letter (P, I, D, S, or F)
    float val = Serial.parseFloat(); // Read the number following it

    if (type == 'P') LKp = val;
    else if (type == 'I') LKi = val;
    else if (type == 'D') LKd = val;
    else if (type == 'S') fsetval = val;
    // Confirm the change
    Serial.print("UPDATED -> P:"); Serial.print(LKp);
    Serial.print(" I:"); Serial.print(LKi);
    Serial.print(" D:"); Serial.print(LKd);
    Serial.print(" Set:"); Serial.print(fsetval);
  }

  // 2. Data for Serial Plotter
  Serial.print(fsetval); Serial.print(",");
  Serial.print(Lrps_smooth); Serial.print(",");
  Serial.println(fpidLOut / 10.0); // Scaled for better viewing
  delay(200);
}
ISR(TIMER1_COMPA_vect) {
  TCNT1 = 0;                    //First, set the timer back to 0 so it resets for next interrupt
  led_state = !led_state;       //Invert LED state
  digitalWrite(13, led_state);  //Write new state to the LED on pin D5, this is to generate a pulse.

  float current_rps = (Lcount_val / 20.0) * 5;  // Estimate rotation speed in rps for wheel Left       // 1/0.1 = 10
  Lcount_val = 0;                                // Reset counter 1.
  if (fpidLOut < 0) {
    current_rps = -current_rps;
  }
  for (int i = 4; i > 0; i--) {
    Lrps_hist[i] = Lrps_hist[i - 1];
  }
  Lrps_hist[0] = current_rps;
  float sum = 0;
  for (int i = 0; i < 5; i++) sum += Lrps_hist[i];
  Lrps_smooth = sum / 5.0;
  errorL = fsetval - Lrps_smooth;
  if(abs(errorL)<0.4){
    if (errorLSum > 0) errorLSum -= 0.5;
    if (errorLSum < 0) errorLSum += 0.5;
  }else {
    errorLSum += errorL;
  }
  errorLSum = constrain(errorLSum, -300, 300);
  float dError = (errorL - error_old) / 0.2;
  fpidLOut = (LKp * errorL) + (LKi * errorLSum) + (LKd * dError);
  
  error_old = errorL;
  set_L_motor(fpidLOut);
}
void Lcount() {
  Lcount_val++;
}
void Rcount() {
  //Rcount_val++;
}
