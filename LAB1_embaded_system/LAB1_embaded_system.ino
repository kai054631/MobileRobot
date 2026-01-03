#include <DHT11.h>

const int LDR = A0;    // LDR SENSOR CONNECT TO A0
const int sensor = 2;  //DHT 11 SENSOR CONNECT TO PIN 6
const int relay = 7;   //RELAY CONNECT TO PIN 7
DHT11 dht11(sensor);
float light_Val = 0.0;
int temp = 0, humidity = 0, condition = 0;
String command;

volatile bool printnow = false;
void setup() {
  // put your setup code here, to run once:
  pinMode(LDR, INPUT);
  pinMode(sensor, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;            // Good practice to reset the counter to 0
  OCR1A = 15624;        // 1 second at 1024 prescaler
  TCCR1B |= B00001101;  // CTC mode ON, Prescaler 1024
  TIMSK1 |= B00000010;  // Enable Compare Match A interrupt
  interrupts();
  digitalWrite(relay, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available() > 0) {
    command = Serial.readStringUntil("\n");
    Serial.println(command);
    if (command == "Lighton") {
      condition = 1;
    } else if (command == "Lightoff") {
      condition = 2;
    } else if (command == "Fanon") {
      condition = 3;
    } else if (command == "Fanoff") {
      condition = 4;
    } else {
      condition = 0;
    }
  }
  switch (condition) {
    case 1:
      digitalWrite(13, HIGH);
      break;
    case 2:
      digitalWrite(13, LOW);
      break;
    case 3:
      digitalWrite(relay, LOW);
      break;
    case 4:
      digitalWrite(relay, HIGH);
      break;
  }
  if (printnow) {
    int result = dht11.readTemperatureHumidity(temp, humidity);
    if (result == 0) {
      light_Val = analogRead(LDR);
      Serial.print("Light Intensity: ");
      Serial.print(light_Val);
      Serial.print(" ");

      Serial.print("Tempreture: ");
      Serial.print(temp);
      Serial.print(" ");

      Serial.print("Humidity: ");
      Serial.println(humidity);
    } else {
      Serial.println(DHT11::getErrorString(result));
    }
    printnow = false;
  }
}
ISR(TIMER1_COMPA_vect) {
  TCNT1 = 0;
  printnow = true;
}
