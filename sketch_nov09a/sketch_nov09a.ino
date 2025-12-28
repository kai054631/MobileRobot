
void setup() {
  // put your setup code here, to run once:
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  digitalWrite(4,HIGH);
  digitalWrite(5,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  delay(50);
  digitalWrite(2,LOW);
  digitalWrite(3,LOW);
  delay(50);
}
