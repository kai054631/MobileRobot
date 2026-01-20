#include "Arduino.h"
#include "Wire.h"
#include "uRTCLib.h"
#include "uEEPROMLib.h"
#include <DHT22.h>
//Temp and Humidity sensor define
DHT22 dht22(2);
//RTC and EEprom define
uRTCLib rtc(0x68);
uEEPROMLib eeprom(0x57);
//date data array
int timedata[6] = { 0, 0, 0, 0, 0, 0 };
unsigned int pos = 0;
int state = 0;
const int butpin = 3;
const int ledpin = 7;
struct LogEntry {
  int date[6];
  float temp;
  float humidity;
};
int val = 0;
bool loggingActive = false;
unsigned int startPos = 0;  // Tracks where we began today
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  //1bit=10µs
  URTCLIB_WIRE.begin();

  pinMode(butpin, INPUT_PULLUP);
  pinMode(ledpin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(butpin), butPress, FALLING);
  digitalWrite(ledpin, LOW);
}

void butPress() {
  state = 1;


  if (state = 0) {
    val = val;
  } else if (state = 1) {
    val++;
    delay(200);
  }

  
}
float getHumidity() {  //get humidity value from dht22
  float h = dht22.getHumidity();
  return h;
}
float getTemptreture() {  //gwt temp valye from dht22
  float t = dht22.getTemperature();
  return t;
}
void getTime(int* dataArray) {  //get time and date value from rtc modules
  rtc.refresh();
  dataArray[0] = rtc.day();
  dataArray[1] = rtc.month();
  dataArray[2] = rtc.year();
  dataArray[3] = rtc.hour();
  dataArray[4] = rtc.minute();
  dataArray[5] = rtc.second();
}
void printData(int date[6], float temp, float humidity) {  // print time and date data
  for (int i = 0; i < 3; i++) {                            //print out all date data
    Serial.print(date[i]);
    if (i < 2) {
      Serial.print("/");
    } else {
      Serial.print(";");
    }
  }
  for (int i = 3; i < 6; i++) {  //print out time data
    Serial.print(date[i]);
    if (i < 5) {
      Serial.print(":");
    } else {
      Serial.print(";");
    }
  }
  Serial.print(temp);
  Serial.print(".");
  Serial.println(humidity);
}
void saveData(int date[6], float temp, float humidity) {  //save data to eeprom
  LogEntry entry;
  for (int i = 0; i < 6; i++) {
    entry.date[i] = date[i];
  }
  entry.temp = temp;
  entry.humidity = humidity;
  eeprom.eeprom_write(pos, (unsigned char*)&entry, sizeof(LogEntry));
  pos += sizeof(LogEntry);
}
void dumpData() {  // put all data in to host
  Serial.println("Date,Time,Temperature(C),Humidity(%)");
  for (unsigned int i = startPos; i < pos; i += sizeof(LogEntry)) {
    LogEntry readEntry;
    eeprom.eeprom_read(i, (unsigned char*)&readEntry, sizeof(LogEntry));
    Serial.print("Addr: ");
    Serial.print(i);
    Serial.print(" | ");
    printData(readEntry.date, readEntry.temp, readEntry.humidity);
  }
  Serial.println("--- END OF DATA ---");
  while (1)
    ;
}
void loop() {
  if (val <= 1) {
    digitalWrite(ledpin, HIGH);
    loggingActive = true;
  } else if (val >= 2) {
    Serial.println("val");
    loggingActive = false;
    digitalWrite(ledpin, LOW);
    dumpData();
  }
  if (loggingActive) {  //print data every 5 sec
    getTime(timedata);
    float t = getTemptreture();
    float h = getHumidity();
    Serial.print("Saving to Address: ");
    Serial.println(pos);  //print which address saved
    printData(timedata, t, h);
    saveData(timedata, t, h);
    delay(5000);
  }
}
