//code to crank up stepper
int runningSpeed=0; //target speed
int currentSpeed=0; //Current motor speed
String str;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()>0){//check if any value get from serial
    runningSpeed=Serial.parseInt();// set the target speed to the value get from serial
    str=Serial.readStringUntil('\n');//
  }
  Serial.print(runningSpeed);
  Serial.print(" ");
  if(currentSpeed<runningSpeed){//slowly crank up the speed to avoid sudden high frequency to motor
    currentSpeed+=5;
    delayMicroseconds(500);
  }else{
    currentSpeed=runningSpeed;// set the speed if reach the target speed
  }
  
  Serial.println(currentSpeed);
  
 
  
}
