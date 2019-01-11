// footballrobot.pde - Footballrobot for cellphone control 
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi
 
int servoPin = 2;
 
int servoRightPin=2;
int servoLeftPin=3;
int servoKickPin=4;
 
int kickNow=0; 
int kickNeutral=380;
int kickKick=1500;
int neutral=1500;
float speedToPulse=170/60; 
// maxPulse=neutral+170; minPulse=neutral-170; minSpeed=-60; maxSpeed=60
int durationLeft=0;
int durationRight=0;
 
int leftSpeed=0;
int rightSpeed=0;
 
int ledPin=13; 
 
void calibrateKick()
{
    for (int i=0; i<=15; i++) {
      pulseServo(servoKickPin, kickNeutral);
      Serial.flush();
    }
}
  
void kick()
{
     for (int i=0; i<=6; i++) {
      pulseServo(servoKickPin, kickKick);
      Serial.flush();
     }
     delay(100);
  calibrateKick();
}
 
void move()
{
      for (int i=0; i<=4; i++) {
        pulseServo(servoLeftPin, (int) neutral+leftSpeed*speedToPulse);
        pulseServo(servoRightPin, (int) neutral-rightSpeed*speedToPulse);
    }
}
 
void pulseServo(int servoPin, int pulseLenUs)
{
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(pulseLenUs);
  digitalWrite(servoPin, LOW);
  delay(20);
}
 
void setup()
{
        pinMode(ledPin, OUTPUT);
        digitalWrite(ledPin, HIGH);
        pinMode(servoLeftPin, OUTPUT);
        pinMode(servoRightPin, OUTPUT);
        pinMode(servoKickPin, OUTPUT);
        Serial.begin(115200);
        calibrateKick();
        // Serial.println("SockerBot initialized - sulautetut.fi");
        digitalWrite(ledPin, LOW);
}
 
void loop()
{
  Serial.print("L");
	if ( Serial.available()) {
		char chInit = Serial.read();
		if ('S'==chInit && Serial.available()) {
                          char chLeft=Serial.read();
			  char chRight=Serial.read();
                          char chKick=Serial.read();
                          char chEnd=Serial.read();
			  if ('U'==chEnd) {
                                leftSpeed  = int(chLeft)-60;
                                rightSpeed = int(chRight)-60;
                                if ('k'==chKick) {
                                   kick();
                                   Serial.flush();
                                }
			}
		} else {
                  // Serial.println("Ignored malformed data.");
                  Serial.flush();
                } 
	} 
	move();
        }
