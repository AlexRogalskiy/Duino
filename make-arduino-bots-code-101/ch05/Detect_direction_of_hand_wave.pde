/* Interactive Painting - Detect direction of hand wave
 Based on code by
 David A. Mellis, Tom Igoe,
 Kimmo Karvinen, and Tero Karvinen
 Updated by Joe Saavedra, 2010
 http://BotBook.com
 */

int slide = 0;

boolean left=false;
boolean center=false;
boolean right=false;

int leftPing = 2; 
int centerPing = 3;
int rightPing = 4;

int ledPin = 13;
int leftLedPin = 5; 
int centerLedPin = 6;
int rightLedPin = 7;

int maxD = 20; // cm 

long int lastTouch = -1; // ms
int resetAfter = 2000; // ms
int afterSlideDelay = 500; //ms; all slides ignored after successful slide
int afterSlideOppositeDelay = 1500;
// left slides ignored after successful right slide

int SLIDELEFT_BEGIN      = -1; // Motion was detected from right
int SLIDELEFT_TO_CENTER  = -2; // Motion was detected from right to center

int SLIDENONE = 0;             // No motion detected

int SLIDERIGHT_BEGIN     = 1;  // Motion was detected from left
int SLIDERIGHT_TO_CENTER = 2;  // Motion was detected from left to center

void setup() {
  Serial.begin(9600); // bit/s
  pinMode(leftLedPin, OUTPUT);  
  pinMode(centerLedPin, OUTPUT);
  pinMode(rightLedPin, OUTPUT);
}

void loop() {
  left = ping(leftPing, leftLedPin);
  center = ping(centerPing, centerLedPin);
  right = ping(rightPing, rightLedPin);

  if (left || center || right) {
    lastTouch=millis();
  }

  if (millis()-lastTouch>resetAfter ) {
    slide=0;
    digitalWrite(ledPin, LOW);
    // Serial.println("Reset slide and timer. ");
  }

  if (slide >= SLIDENONE) { // only if we are not already in opposite move
    if ( (left) && (!right) )
      slide = SLIDERIGHT_BEGIN;
    if (center && (slide == SLIDERIGHT_BEGIN))
      slide = SLIDERIGHT_TO_CENTER;
    if (right && (slide == SLIDERIGHT_TO_CENTER))
      slideNow('R');
  }

  if (slide <= SLIDENONE) {    
    if (right && (!left))
      slide = SLIDELEFT_BEGIN;
    if (center && slide == SLIDELEFT_BEGIN)
      slide = SLIDELEFT_TO_CENTER;
    if (left && slide == SLIDELEFT_TO_CENTER) {
      slideNow('L');
    }
  }
  delay(50);
}

boolean ping(int pingPin, int ledPin) {  bk

  int d = getDistance(pingPin); //cm  bl
  boolean pinActivated = false;
  if (d < maxD) {
    digitalWrite(ledPin, HIGH);
    pinActivated = true;
  } 
  else {
    digitalWrite(ledPin, LOW);
    pinActivated = false;
  }
  return pinActivated;
}


int getDistance(int pingPin) { 

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
  
  //return(inches);
  return(cm);
}


void slideNow(char direction) {
  if ('R' == direction) bm
     Serial.println("F");
  if ('L' == direction)
     Serial.println("B");
  digitalWrite(ledPin, HIGH);
  delay(afterSlideDelay); bn
  slide = SLIDENONE;
}


long microsecondsToInches(long microseconds) {

  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {

  return microseconds / 29 / 2;
}
