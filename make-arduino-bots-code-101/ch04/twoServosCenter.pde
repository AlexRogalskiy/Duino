//twoServosCenter.pde - Center two servos
// (c) Kimmo Karvinen & Tero Karvinen http://BotBook.com
//updated - Joe Saavedra, 2010
#include <Servo.h> 
 
Servo frontServo;  
Servo rearServo;

void setup() 
{ 
  frontServo.attach(2); 
  rearServo.attach(3);
  frontServo.write(90);
  rearServo.write(90);
} 
 
void loop() 
{ 
    delay(100);
}