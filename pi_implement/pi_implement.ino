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
#define trig_pin1 10
#define echo_pin1 11


bool led_state = true;
float previous_R_value;

//encoder Setup
volatile long Lcount_val = 0;
float Lrps = 0.0, errorL = 0.0, errorLSum = 0.0, error_oldL = 0.0, fpidLOut = 0.0;
float LKp = 30.0;
float LKi = 2.20;
float LKd = 0.0;

volatile long Rcount_val = 0;
float Rrps = 0.0, errorR = 0.0, errorRSum = 0.0, error_oldR = 0.0, fpidROut = 0.0;
float RKp = 30.0;
float RKi = 2.20;
float RKd = 0.0;

// void direction_value(){
//       //ultrasonic code
//       //--- 1. READ SENSOR ---
//     float L_sensor = getdistance(trig_pin2, echo_pin2);
//     float R_sensor = getdistance(trig_pin1, echo_pin1);
//     if(L_sensor < 20 && R_sensor < 20){ //reverse then turn right
//       situation = 1;
//     }else if(L_sensor < 20 && R_sensor > 20){ // rotate 90 degree to right
//       situation = 2;
//     }else if(L_sensor > 20 && R_sensor < 20){ //rotate 90 degree to left
//       situation = 3;
//     }else if(L_sensor > 20 && R_sensor > 20){ //move forward
//       situation = 4;
//     }

//     switch(situation){
//       case 1:

//       case 2:

//       case 3:

//       case 4:
//         setmotor(2, 2);
//     }
// }

float getdistance(int trig_pin, int echo_pin){
  digitalWrite(trig_pin, LOW);
  delay(2);
  digitalWrite(trig_pin, HIGH);
  delay(10);
  digitalWrite(trig_pin, LOW);

  float timing = pulseIn(echo_pin, HIGH);
  float distance = (timing * 0.034) / 2;
  if(distance > 150){
    distance = previous_R_value;
  }

  return distance;
}

float fsetvalL = 1.0, fsetvalR = 1.0;
void set_L_motor(float speed) {
  float pwm_val = abs((float)speed);
  if (speed > 0) {
    digitalWrite(dirL1, LOW);
    digitalWrite(dirL2, HIGH);
    analogWrite(L_speedcontrol, pwm_val);
  } else if (speed < -15) {
    digitalWrite(dirL1, HIGH);
    digitalWrite(dirL2, LOW);
    analogWrite(L_speedcontrol, -pwm_val);
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
  } else if (speed < -15) {
    digitalWrite(dirR1, HIGH);
    digitalWrite(dirR2, LOW);
    analogWrite(R_speedcontrol, -pwm_val);
  } else {
    digitalWrite(dirR1, LOW);
    digitalWrite(dirR2, LOW);
    analogWrite(R_speedcontrol, 0);
  }
}
void Bluetooth_Control() {
  if (Serial.available() > 0) {
    char type = Serial.read();
    if (type == 'W') {
      set_L_motor(255);
      set_R_motor(255);
      delay(1000);
    } else if (type == 'A') {
      set_L_motor(100);
      set_R_motor(255);
      delay(1000);
    } else if (type == 'P') {
      set_L_motor(255);
      set_R_motor(100);
      delay(1000);
    } else if (type == 'S') {
      set_L_motor(-255);
      set_R_motor(-255);
      delay(1000);
    } else {
      set_L_motor(0);
      set_R_motor(0);
    }
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
      fsetvalL = val;
      fsetvalR = val;
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
    errorLSum = 0;
    errorRSum = 0;
  }
  //   // 2. Data for Serial Plotter
  //   Serial.print(fsetval); Serial.print(",");
  Serial.print(0);
  Serial.print(" ");
  Serial.print("  ");
  Serial.print(Lrps);
  Serial.print("  ");
  Serial.print(fpidLOut);
  Serial.print("  ");
  Serial.print(Rrps);  // Scaled for better viewing
  Serial.print("  ");
  Serial.print(errorRSum);
  Serial.print(" ");
  Serial.println(5);
  //FKUNGMS
  delay(100);
}
void setmotor_Command() {
  if (Serial.available() > 0) {
    char type = Serial.read();        // Read the letter (P, I, D, S, or F)
    float val = Serial.parseFloat();  // Read the number following it
    if (type == 'L') {
      fsetvalL = val;
    } else if (type == 'R') {
      fsetvalR = val;
    }else if (type == 'B') {
      fsetvalL = val;
      fsetvalR = val;
    }
  }
}
void setmotor(float speedL, float speedR) {
  fsetvalL = speedL;
  fsetvalR = speedR;
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
  OCR1A = 6250 * 2;     //Finally we set compare register A to this value
  interrupts();
}

void loop() {
  setmotor_Command();
  float R_sensor = getdistance(trig_pin1, echo_pin1);
  previous_R_value = R_sensor;
  Serial.println(R_sensor);
  if (R_sensor > 20) {  //move forward
    setmotor(2, 2);
  }
  else {
    setmotor(0, 0);
  }
}
ISR(TIMER1_COMPA_vect) {
  TCNT1 = 0;                    //First, set the timer back to 0 so it resets for next interrupt
  led_state = !led_state;       //Invert LED state
  digitalWrite(13, led_state);  //Write new state to the LED on pin D5, this is to generate a pulse.

  Lrps = (Lcount_val / 96.0) * 5;  // Estimate rotation speed in rps for wheel Left       // 1/0.1 = 10
  Lcount_val = 0;                  // Reset counter 1.
  errorL = fsetvalL - Lrps;
  errorLSum = errorLSum + errorL;
  float dErrorL = (errorL - error_oldL) / 0.2;
  fpidLOut = (LKp * errorL) + (LKi * errorLSum) + (LKd * dErrorL);
  if (fpidLOut > 255) {
    set_L_motor(255);
  } else {
    set_L_motor(fpidLOut);
  }
  //set_L_motor(fpidLOut);
  error_oldL = errorL;

  Rrps = (Rcount_val / 96.0) * 5;  // Estimate rotation speed in rps for wheel Left       // 1/0.1 = 10
  Rcount_val = 0;                  // Reset counter 1.
  errorR = fsetvalR - Rrps;
  errorRSum = errorRSum + errorR;
  float dErrorR = (errorR - error_oldR) / 0.2;
  fpidROut = (RKp * errorR) + (RKi * errorRSum) + (RKd * dErrorR);
  if (fpidROut > 255) {
    set_R_motor(255);
  } else {
    set_R_motor(fpidROut);
  }
  error_oldR = errorR;
}
void Lcount() {
  Lcount_val++;
}
void Rcount() {
  Rcount_val++;
}
