// ledRelay.pde - Control led with a relay
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi
 
int relayPin = 2;
 void setup()
{
  pinMode(relayPin, OUTPUT);
}
 void loop()
{
  digitalWrite(relayPin, HIGH);
  delay(1000);
  digitalWrite(relayPin, LOW);
  delay(1000);
}
