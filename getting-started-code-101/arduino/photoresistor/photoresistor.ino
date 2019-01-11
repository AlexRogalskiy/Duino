// photoresistor.ino - blink faster in dark, slower in the light
// (c) BotBook.com - Karvinen, Karvinen, Valtokari

int photoPin=A0;
int ledPin=13;
int x=-1; // 0..1023

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  x=analogRead(photoPin);
  digitalWrite(ledPin, HIGH);
  delay(x/10); // <5>
  digitalWrite(ledPin, LOW);
  delay(x/10);
}

