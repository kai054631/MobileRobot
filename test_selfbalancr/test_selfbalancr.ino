// ================================================================
// ===               CRITICAL SETTINGS                          ===
// ================================================================

// 1. PIN DEFINITIONS (UPDATED)
// ERROR FIX: Pin 2 cannot be used twice. 
// I have moved DirR to Pin 3 because it was free.
const int StpL = 10;  // Left Step
const int DirL = 12;  // Left Dir
const int StpR = 11;  // Right Step
const int DirR = 13;  // Right Dir (CHANGED FROM 2 TO 3)
void setup() {
  pinMode(DirL, OUTPUT);
  pinMode(StpL, OUTPUT);
  pinMode(DirR, OUTPUT);
  pinMode(StpR, OUTPUT);

  // Set direction
  digitalWrite(DirL, LOW); 
  digitalWrite(DirR, LOW);
}

void loop() {
  // Create a pulse
  digitalWrite(StpL, HIGH);
  digitalWrite(StpR, HIGH);
  delayMicroseconds(500); // Wait
  digitalWrite(StpL, LOW);
  digitalWrite(StpR, LOW);
  delayMicroseconds(500); // Wait
  
  // If motors vibrate but don't turn: Your coil pairs are wrong.
  // If motors do nothing: You are missing 12V power or ENABLE is HIGH.
}
