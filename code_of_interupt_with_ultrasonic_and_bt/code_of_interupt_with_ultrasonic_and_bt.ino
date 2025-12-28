#include <SoftwareSerial.h>
#include <string.h>

// SoftwareSerial RX, TX pins
SoftwareSerial BT(11, 10);   // RX = 11, TX = 10//Config for BT modules


// --- PIN DEFINITIONS ---
const int DirL = 4;   //Left motor
const int StpL = 2;   //Left motor
const int DirR = 5;   //Right motor
const int StpR = 3;   //Right motor

#define trig_pin1 6   //right Ultrasonic
#define echo_pin1 9   //right Ultrasonic
#define trig_pin2 8   //left UltraSonic
#define echo_pin2 13  //left Ultrasonic

float timing = 0.0;
float R_distance = 0.0;
float L_distance = 0.0;
float F_distance = 0.0;
float average_distance = 0.0;
float previous_R_distance = 0.0;
float previous_L_distance = 0.0;

// --- VARIABLES ---
volatile bool runMotorsL = false;
volatile bool runMotorsR = false;
unsigned long lastPrintTime = 0; // Helper for printing speed


int mode = 1;
void setup() {
  // Direction
  digitalWrite(DirL, LOW);
  digitalWrite(DirR, LOW);
  // Pin Modes
  pinMode(DirL, OUTPUT);
  pinMode(StpL, OUTPUT);
  pinMode(DirR, OUTPUT);
  pinMode(StpR, OUTPUT);

  pinMode(echo_pin1, INPUT);  //define Ultrasonic Left
  pinMode(trig_pin1, OUTPUT); //define Ultrasonic Left
  pinMode(trig_pin2, OUTPUT); //define Ultrasonic Right
  pinMode(echo_pin2, INPUT);  //define Ultrasonic Right

  digitalWrite(trig_pin1, LOW); //Initial the status of the Ultrasonic Left
  digitalWrite(echo_pin1, LOW); //Initial the status of the Ultrasonic Left
  digitalWrite(trig_pin2, LOW); //Initial the status of the Ultrasonic Right
  digitalWrite(echo_pin2, LOW); //Initial the status of the Ultrasonic Right

  Serial.begin(9600); // Start serial
  BT.begin(9600);     //Start bluetooth serial

  Serial.println("Ready. Use F/B/L/R/S or F1000/B500...");

  // --- MANUAL TIMER 1 SETUP ---
  noInterrupts();
  TCCR1A = 0;  //0000 0000 (COM1A1 COM1A0 COM1B1 COM1B0 - - WGM 11 WGM10)/
  TCCR1B = 0; //0000 0110  (ICNC1 ICES1 - WGM13 WGM 12 CS12 CS11 CS10)/
  TCNT1  = 0;

  // Set Speed: 399 = Moderate (approx 5000Hz).
  // Lower this number to go FASTER. Increase to go SLOWER.
  OCR1A = 399;

  TCCR1B |= (1 << WGM12);   // CTC Mode
  TCCR1B |= (1 << CS11);    // Prescaler 8
  TIMSK1 |= (1 << OCIE1A);  // Enable Interrupt
  interrupts();
}

float getdistance(int trig_pin, int echo_pin, char D) {
  digitalWrite(trig_pin, LOW);
  delay(2);
  digitalWrite(trig_pin, HIGH);
  delay(10);
  digitalWrite(trig_pin, LOW);

  timing = pulseIn(echo_pin, HIGH);
  float distance = (timing * 0.034) / 2;

  return distance;
}

float direction_value(){
      //ultrasonic code
      //--- 1. READ SENSOR ---
    L_distance = getdistance(trig_pin2, echo_pin2, 'L');
    R_distance = getdistance(trig_pin1, echo_pin1, 'R');
    return L_distance-R_distance;    
}

int btConnect()  {
  String input;
  if (BT.available() > 0)
  {
    // FIXED: Works with or without newline
    input = BT.readStringUntil("\n");
    input.trim();

    Serial.print("Received: ");
    Serial.println(input);
    if (input = "f") {
      return 1;
    }
    //      case "b":
    //        return 2;
    //        break;
    //      case "l":
    //        return 3;
    //        break;
    //      case "r":
    //        return 4;
    //        break;
    //    }

  }
}

void loop() {
  switch (mode)
  {
    case 1:
      int final_distance = direction_value();
      Serial.println(final_distance);
      // --- 2. CONTROL MOTORS ---
      // Filter out '0' which usually means sensor error/out of range
      if (final_distance> 50)
      {
        runMotorsL = false;  // Stop
        runMotorsR = true;
      } else if (final_distance< -50)
      {
        runMotorsL = true; // run
        runMotorsR = false;
      } else if
      {
        runMotorsL = true; // run
        runMotorsR = true;
      }
      // --- 3. SERIAL PRINT (Non-Blocking) ---
      // Only print every 200ms so we don't spam the screen or slow the sensor
      /*if (millis() - lastPrintTime > 200)
      {
        Serial.print("DistL: ");
        Serial.print(L_distance);
        Serial.print("DistR: ");
        Serial.println(R_distance);

        if (runMotorsL) {
          Serial.print(" L RUNNING"); // L moving
        } else {
          Serial.print("  L STOPPED");
        }
        if (runMotorsR) {
          Serial.println("  R RUNNING"); // R moving
        } else {
          Serial.println("  R STOPPED");
        }
      }
      lastPrintTime = millis(); // Reset print timer
      // Small delay for sensor stability
      delay(50);*/
      break;

      case 2: // BT Control

         
      
      break;
  }
}





// --- INTERRUPT SERVICE ROUTINE ---
// This handles the pulses in the background
ISR(TIMER1_COMPA_vect) {
  if (runMotorsL) {
    // Pulse Left
    digitalWrite(StpL, HIGH);
    digitalWrite(StpL, LOW);
  }
  if (runMotorsR) {
    // Pulse Right
    digitalWrite(StpR, HIGH);
    digitalWrite(StpR, LOW);

  }
}
//code of movement autonomes
