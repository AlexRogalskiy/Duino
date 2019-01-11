// helloSerial.pde - Print message to serial port repeatitly
// (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi
 
void setup()
{
  Serial.begin(115200);
}
 
void loop()
{
  Serial.println("Hello serial - Sulautetut.fi");
  delay(500);
}
