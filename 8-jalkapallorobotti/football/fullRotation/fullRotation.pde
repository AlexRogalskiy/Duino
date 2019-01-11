// fullRotation.pde - Turn full rotation servo clockwise, 
// counterclockwise and stop. 
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi
 
int servoPin = 2;
 
void pulseServo(int servoPin, int pulseLenUs)
{
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(pulseLenUs);
  digitalWrite(servoPin, LOW);
  delay(20);
}
 
void servoClockWise()
{
    for (int i=0; i<200; i++) {
          pulseServo(servoPin, 1200);
    }
}
 
 void servoCounterClockWise()
 {
      for (int i=0; i<200; i++) {
          pulseServo(servoPin, 1800);
    }
 }
 
 void servoStop()
 {
     for (int i=0; i<200; i++) {
          pulseServo(servoPin, 1500);
    }
 }
 
void setup()
{
	pinMode(servoPin, OUTPUT);
}
 
void loop()
{
    servoClockWise();
    servoStop();
    servoCounterClockWise();
    servoStop();
}

