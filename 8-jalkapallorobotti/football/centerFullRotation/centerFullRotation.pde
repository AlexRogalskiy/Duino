// centerFullRotation.pde - Turn full rotation servo clockwise, 
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
 
void setup()
{
	pinMode(servoPin, OUTPUT);
}
 
void loop()
{
    pulseServo(servoPin, 1500);
}
