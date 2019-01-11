// footballBotKick.pde - Kick
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

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
 
void setup()
{
 
     pinMode(servoKickPin, OUTPUT);
     calibrateKick();
}
 
void loop()
{
kick();
delay(1000);
}
