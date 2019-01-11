// footballBotDirections.pde - Move forward, backward, turn right and left.
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

int servoRightPin=2;
int servoLeftPin=3;
 
void pulseServo(int servoPin, int pulseLenUs)
{
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(pulseLenUs);
  digitalWrite(servoPin, LOW);
  delay(20);
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
 
}
 
void loop()
{
moveForward();
delay(1000);
moveBack();
delay(1000);
turnRight();
delay(1000);
turnLeft();
delay(1000);
}

