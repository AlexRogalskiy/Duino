// servoCenter.pde - Center servo
// (c) Kimmo Karvinen & Tero Karvinen http://BotBook.com
// updated – Joe Saavedra, 2010
#include <Servo.h>
 
Servo myServo;
 
void setup() 
{ 
  myServo.attach(2);
  myServo.write(90);
} 
 
void loop() 
{ 
  delay(100); 
} 
