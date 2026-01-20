//L298N setup
#define dirL1 4
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
#define trig_pin2 12
#define echo_pin2 13
int situation = 0;
bool led_state = true;

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
//ultrasonic sensor setup
float L_sensor, R_sensor;
float previous_R_value, previous_L_value;

void ultrasonic_sensor() {
  //ultrasonic code
  //--- 1. READ SENSOR ---
  previous_L_value = L_sensor;
  previous_R_value = R_sensor;
  L_sensor = getdistance(trig_pin2, echo_pin2, 'L');
  R_sensor = getdistance(trig_pin1, echo_pin1, 'R');
}
void automode() {
  if (L_sensor < 20 && R_sensor < 20) {  //reverse then turn right
    situation = 1;
  } else if (L_sensor < 20 && R_sensor > 20) {  // rotate 90 degree to right
    situation = 2;
  } else if (L_sensor > 20 && R_sensor < 20) {  //rotate 90 degree to left
    situation = 3;
  } else if (L_sensor > 20 && R_sensor > 20) {  //move forward
    situation = 4;
  }
  switch (situation) {
    case 1:
      setmotor(-1, -2);
    case 2:
      setmotor(1, -1);
    case 3:
      setmotor(-1, 1);
    case 4:
      setmotor(1, 1);
  }
}

float getdistance(int trig_pin, int echo_pin, char D) {
  digitalWrite(trig_pin, LOW);
  delay(4);
  digitalWrite(trig_pin, HIGH);
  delay(20);
  digitalWrite(trig_pin, LOW);
  float timing = pulseIn(echo_pin, HIGH);
  // Serial.print("Timing = ");
  // Serial.println(timing);
  float distance = (timing * 0.034);
  if(D == 'R'){
    if (distance > 350 || distance < 1.0) {
      distance = previous_R_value;
    }  
  }else if(D == 'L'){
    if (distance > 350 || distance < 1.0) {
      distance = previous_L_value;
    } 
  }

  return distance;
}

float fsetvalL = 0.0, fsetvalR = 0.0;
void set_L_motor(float speed) {
  //float pwm_val = abs((float)speed);
  if (speed > 0) {
    digitalWrite(dirL1, LOW);
    digitalWrite(dirL2, HIGH);
    analogWrite(L_speedcontrol, speed);
  } else if (speed < -40) {
    digitalWrite(dirL1, HIGH);
    digitalWrite(dirL2, LOW);
    analogWrite(L_speedcontrol, -speed);
  } else {
    digitalWrite(dirL1, LOW);
    digitalWrite(dirL2, LOW);
    analogWrite(L_speedcontrol, 0);
  }
}
void set_R_motor(float speed) {
  //float pwm_val = abs((float)speed);
  if (speed > 0) {
    digitalWrite(dirR1, LOW);
    digitalWrite(dirR2, HIGH);
    analogWrite(R_speedcontrol, speed);
  } else if (speed < -40) {
    digitalWrite(dirR1, HIGH);
    digitalWrite(dirR2, LOW);
    analogWrite(R_speedcontrol, -speed);
  } else {
    digitalWrite(dirR1, LOW);
    digitalWrite(dirR2, LOW);
    analogWrite(R_speedcontrol, 0);
  }
}
void Bluetooth_Control() {
  if (Serial.available() > 0) {
    // ultrasonic_sensor();
    // if (L_sensor > 20 && R_sensor > 20) {
      char type = Serial.read();
      //if(type == '0'){state = 0} and else if(type == '1'){state = 1}, switch(state) case '0': continue if, else statement for receive data, case '1': automode();
      Serial.println(type);
      if (type == 'F') {
        setmotor(3, 3);
      } else if (type == 'B') {
        setmotor(-1, -1);
      } else if (type == 'L') {
        setmotor(0, 3);
      } else if (type == 'R') {
        setmotor(3, 0);
      } else if (type == 'S') {
        setmotor(0, 0);
      } 
      // else if (type == 'A'){ //addition code for add automode 
      //   automode();
      // }
    }
    // else{
    //   automode();//run automatic mode since got obstacle nearby
    // }
  // }
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
    } else if (type == 'B') {
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
  interrupts();
  Bluetooth_Control();
  //ultrasonic_sensor(); to read the ultrasonic value, since automode function will not call ultrasonic to get the new value
  //automode(); for testing first;
  Serial.print(" ");
  Serial.print(Lrps);
  Serial.print(" ");
  Serial.print(Rrps);
  Serial.print(" ");
  Serial.print(errorL);
  Serial.print(" ");
  Serial.print(" ");
  Serial.print(fsetvalR);
  Serial.print(" ");
  Serial.print(fpidLOut);
  Serial.print(" ");
  Serial.println(fpidROut);

  // noInterrupts();
  // float R_sensor = getdistance(trig_pin1, echo_pin1);
  // previous_R_value = R_sensor;
  // Serial.print("Lrps :");
  // Serial.println(Lrps);
  // Serial.print("Rrps: ");
  // Serial.println(Rrps);

  // Serial.println(R_sensor);
  // if (R_sensor > 20) {  //move forward
  //   setmotor(2, 2);
  // } else {
  //   setmotor(0, 0);
  // }
}
ISR(TIMER1_COMPA_vect) {
  TCNT1 = 0;                    //First, set the timer back to 0 so it resets for next interrupt
  led_state = !led_state;       //Invert LED state
  digitalWrite(13, led_state);  //Write new state to the LED on pin D5, this is to generate a pulse.
  if (fsetvalL >= 0) {
    Lrps = (Lcount_val / 96.0) * 5;  // Estimate rotation speed in rps for wheel Left       // 1/0.1 = 10
    Lcount_val = 0;
  } else {
    Lrps = -(Lcount_val / 96.0) * 5;  // Estimate rotation speed in rps for wheel Left       // 1/0.1 = 10
    Lcount_val = 0;
  }
  // Reset counter 1.
  errorL = fsetvalL - Lrps;
  errorLSum = errorLSum + errorL;
  float dErrorL = (errorL - error_oldL) / 0.2;
  fpidLOut = (LKp * errorL) + (LKi * errorLSum) + (LKd * dErrorL);
  if (fpidLOut > 255) {
    fpidLOut = 255;
    set_L_motor(fpidLOut);
  } else {
    set_L_motor(fpidLOut);
  }
  //set_L_motor(fpidLOut);
  error_oldL = errorL;


  if (fsetvalR >= 0) {
    Rrps = (Rcount_val / 96.0) * 5;
    Rcount_val = 0;
  } else {
    Rrps = -(Rcount_val / 96.0) * 5;
    Rcount_val = 0;
  }
  errorR = fsetvalR - Rrps;
  errorRSum = errorRSum + errorR;
  float dErrorR = (errorR - error_oldR) / 0.2;
  fpidROut = (RKp * errorR) + (RKi * errorRSum) + (RKd * dErrorR);
  if (fpidROut > 255) {
    fpidROut = 255;
    set_R_motor(fpidROut);
  } else {
    set_R_motor(fpidROut);
  }
  if (fpidROut < -255) {
    fpidROut = -255;
    set_R_motor(fpidROut);
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
