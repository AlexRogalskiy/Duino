// footballBotForward.pde - Move forward
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
 
void setup()
{
        pinMode(servoLeftPin, OUTPUT);
        pinMode(servoRightPin, OUTPUT);
 
}
 void loop()
{
moveForward();
delay(1000);
}
