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
float Lrps = 0.0, errorL = 0.0, errorLSum = 0.0, error_oldL = 0.0, fpidLOut = 0.0;
float LKp = 60;
float LKi = 10;
float LKd = 0.1;

/*
28    42
0.02  2
0.8   0.1 
*/

volatile long Rcount_val = 0;
float Rrps = 0.0, errorR = 0.0, errorRSum = 0.0, error_oldR = 0.0, fpidROut = 0.0;
float RKp = 60.0;
float RKi = 10.0;
float RKd = 0.1;
/*
28    35
0.02  3.5
0.8   0.0 
*/

float fsetval = 1.0;
void set_L_motor(float speed) {
  float pwm_val = abs((float)speed);
  if (speed > 0) {
    digitalWrite(dirL1, LOW);
    digitalWrite(dirL2, HIGH);
    analogWrite(L_speedcontrol, pwm_val);
  } else if (speed < 0) {
    digitalWrite(dirL1, HIGH);
    digitalWrite(dirL2, LOW);
    analogWrite(L_speedcontrol, pwm_val);
  } else {
    digitalWrite(dirL1, LOW);
    digitalWrite(dirL2, LOW);
    analogWrite(L_speedcontrol, 0);
  }
}
void set_R_motor(float speed) {
  float pwm_val = abs((float)speed);
  if (speed > 0) {
    digitalWrite(dirR1, LOW);
    digitalWrite(dirR2, HIGH);
    analogWrite(R_speedcontrol, pwm_val);
  } else if (speed <0) {
    digitalWrite(dirR1, HIGH);
    digitalWrite(dirR2, LOW);
    analogWrite(R_speedcontrol, pwm_val);
  } else {
    digitalWrite(dirR1, LOW);
    digitalWrite(dirR2, LOW);
    analogWrite(R_speedcontrol, 0);
  }
}

void pid_tune() {
  if (Serial.available() > 0) {
    char type = Serial.read();        // Read the letter (P, I, D, S, or F)
    float val = Serial.parseFloat();  // Read the number following it

    if (type == 'P') {
      LKp = val;
    } else if (type == 'I') {
      LKi = val;
    } else if (type == 'D') {
      LKd = val;
    } else if (type == 'O') {
      RKp = val;
    } else if (type == 'U') {
      RKi = val;
    } else if (type == 'F') {
      RKd = val;
    } else if (type == 'S') {
      fsetval  = val;
    }
    // Confirm the change
    Serial.print("UPDATED -> LP:");
    Serial.print(LKp);
    Serial.print(" LI:");
    Serial.print(LKi);
    Serial.print(" LD:");
    Serial.print(LKd);
    Serial.print("  ");
    Serial.print("RP:");
    Serial.print(RKp);
    Serial.print(" RI:");
    Serial.print(RKi);
    Serial.print(" RD:");
    Serial.print(RKd);
    Serial.print(" Set:");
    Serial.println(fsetval);
    errorLSum = 0;
    errorRSum = 0;
  }
  //   // 2. Data for Serial Plotter
  //   Serial.print(fsetval); Serial.print(",");
  Serial.print(0);
  Serial.print(" ");
  Serial.print(fsetval);
  Serial.print("  ");
  Serial.print(Lrps);
  Serial.print("  ");
  Serial.print(Rrps);  // Scaled for better viewing
  Serial.print(" ");
  Serial.println(5);
  //FKUNGMS
  delay(100);
}
void setup() {
  // put your setup code here, to run once:
  //Serial Setup
  Serial.begin(9600);
  //Bt setup
  //btSerial.begin(9600);
  Serial.println("BT ready! give the speed to run");

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
  pinMode(2, INPUT);  // To connect to output of wheel encoder 1.
  pinMode(3, INPUT);  // To connect to output of wheel encoder 2.

  attachInterrupt(digitalPinToInterrupt(3), Lcount, RISING);
  attachInterrupt(digitalPinToInterrupt(2), Rcount, RISING);
  noInterrupts();
  TCCR1A = 0;           // Reset entire TCCR1A to 0
  TCCR1B = 0;           // Reset entire TCCR1B to 0
  TCCR1B |= B00000100;  // prescaler set to 256
  TIMSK1 |= B00000010;  //Set OCIE1A to 1 so we enable compare match A
  OCR1A = 6250;         //Finally we set compare register A to this value
  interrupts();
}

void loop() {
  pid_tune();
}
ISR(TIMER1_COMPA_vect) {
  TCNT1 = 0;                    //First, set the timer back to 0 so it resets for next interrupt
  led_state = !led_state;       //Invert LED state
  digitalWrite(13, led_state);  //Write new state to the LED on pin D5, this is to generate a pulse.

  Lrps = (Lcount_val / 96.0) * 10;  // Estimate rotation speed in rps for wheel Left       // 1/0.1 = 10
  Lcount_val = 0;                   // Reset counter 1.
  errorL = fsetval - Lrps;
  if(errorL >= 1.33){
    errorL = errorL;
  }
  else {
    errorL = 0;
  }
  errorLSum = errorLSum + errorL;
  float dErrorL = (errorL - error_oldL) / 0.1;
  fpidLOut = (LKp * errorL) + (LKi * errorLSum) + (LKd * dErrorL);
  set_L_motor(fpidLOut);
  error_oldL = errorL;

  Rrps = (Rcount_val / 96.0) * 10;  // Estimate rotation speed in rps for wheel Left       // 1/0.1 = 10
  Rcount_val = 0;                   // Reset counter 1.
  errorR = fsetval - Rrps;
  if(errorR >= 1.33){
    errorR = errorR;
  }
  else {
    errorR = 0;
  }
  errorRSum = errorRSum + errorR;
  float dErrorR = (errorR - error_oldR) / 0.1;
  fpidROut = (RKp * errorR) + (RKi * errorRSum) + (RKd * dErrorR);
  set_R_motor(fpidROut);
  error_oldR = errorR;
}
void Lcount() {
  Lcount_val++;
}
void Rcount() {
  Rcount_val++;
}
