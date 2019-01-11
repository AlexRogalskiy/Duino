// footballBotSerialControl.pde - Call move functions from serial console.
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

int servoRightPin=2;
int servoLeftPin=3;
int servoKickPin=4;
int kickNeutral=380;
int kickKick=1500;
 
void pulseServo(int servoPin, int pulseLenUs)
{
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(pulseLenUs);
  digitalWrite(servoPin, LOW);
  delay(20);
}
 
void calibrateKick()
{
    for (int i=0; i<=25; i++) {
      pulseServo(servoKickPin, kickNeutral);
    }
}
 void kick()
{
    for (int i=0; i<=10; i++) {
      pulseServo(servoKickPin, kickKick);
    }
    delay(500);
    calibrateKick();
}
  
void moveForward()
{
    for (int i=0; i<=15; i++) {
     pulseServo(servoLeftPin, 1800);
    pulseServo(servoRightPin, 1200);
    }
}

 void moveBack()
{
   for (int i=0; i<=15; i++) {
     pulseServo(servoLeftPin, 1200);
    pulseServo(servoRightPin, 1800);
    }
}
void turnLeft()
{
     for (int i=0; i<=15; i++) {
      pulseServo(servoLeftPin, 1200);
      pulseServo(servoRightPin, 1200);
    }
}
 void turnRight()
{
     for (int i=0; i<=15; i++) {
      pulseServo(servoLeftPin, 1800);
      pulseServo(servoRightPin, 1800);
    }
} 
 
void setup()
{
         pinMode(servoLeftPin, OUTPUT);
        pinMode(servoRightPin, OUTPUT);
        pinMode(servoKickPin, OUTPUT);
         calibrateKick();
         Serial.begin(115200);
        Serial.println("Football robot. (c) 2008 Karvinen");
 }
  
void loop()
{
 if ( Serial.available()) {
    char ch = Serial.read();
    if ('8'==ch){
      moveForward();
      }
     else if ('2'==ch){
       moveBack();
       }
      else if ('6'==ch){
       turnRight();
       }
      else if ('4'==ch){
       turnLeft();
       }
      else if ('5'==ch){
       kick();
       }
    }
 }

