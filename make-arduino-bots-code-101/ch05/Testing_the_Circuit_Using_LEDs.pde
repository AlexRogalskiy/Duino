/* Read 3 Ping Sensors
 Based on code by
 David A. Mellis, Tom Igoe,
 Kimmo Karvinen and Tero Karvinen
 Updated by Joe Saavedra, 2010
 http://BotBook.com
 */

const int leftPing = 2; 
const int centerPing = 3;
const int rightPing = 4;

const int leftLed = 5;
const int centerLed = 6;
const int rightLed = 7;

const int maxD = 20; // cm; maximum hand distance

void setup()
{
  Serial.begin(9600);
  pinMode(leftLed, OUTPUT);
  pinMode(centerLed, OUTPUT);
  pinMode(rightLed, OUTPUT);
}

void loop()
{
  ping(leftPing, leftLed);
  ping(centerPing, centerLed); 
  ping(rightPing, rightLed); 
  delay(50);
}

boolean ping(int pingPin, int ledPin)
{
  int d = getDistance(pingPin); // cm
  boolean pinActivated = false;
  if (d < maxD) { 
    digitalWrite(ledPin, HIGH);
    pinActivated = true;
  } else {
    digitalWrite(ledPin, LOW);
    pinActivated = false;
  }
  return pinActivated;
}

int getDistance(int pingPin)
{
  long duration, inches, cm;

  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  
  return(cm); // You could also return inches
}

long microsecondsToInches(long microseconds)
{
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}
