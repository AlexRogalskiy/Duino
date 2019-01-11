// (c) Kimmo Karvinen & Tero Karvinen http://BotBook.com

int ledPin = 2;

void setup()
{
    pinMode(ledPin, OUTPUT); 
}
void loop()
{
    digitalWrite(ledPin, HIGH); 
    delay(500); 4
    digitalWrite(ledPin, LOW); 
    delay(500);
} 
