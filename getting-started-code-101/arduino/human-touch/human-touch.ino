// human-touch.ino - detect human touch with Keyes capacitive touch sensor
// (c) BotBook.com - Karvinen, Karvinen, Valtokari


int touchTriggerPin = 8;	// <1>
int touchProximityPin = A0;	// <2>
int proximity = -1;	// <3>
int touch = 0;	// <4>

void setup() {	// <5>
  Serial.begin(115200);	// <6>  
  pinMode(touchTriggerPin, INPUT);	// <7>
}

void loop() {	// <8>
  proximity = analogRead(touchProximityPin);	// <9> //0 -1024, ~100-200 direct touch.
  Serial.print("Proximity: ");	// <10>
  Serial.println(proximity);	// <11>
  touch = digitalRead(touchTriggerPin);	// <12>
  Serial.print("Touch: ");	// <13>
  Serial.println(touch);	// <14>
  delay(100);	// <15>
}



