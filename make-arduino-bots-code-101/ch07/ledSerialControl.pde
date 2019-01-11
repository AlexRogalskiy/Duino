// ledSerialControl.pde - Control led via serial
// (c) Kimmo Karvinen & Tero Karvinen http://BotBook.com
 
int relayPin=13;

void setup()
{
    pinMode(relayPin, OUTPUT);
    Serial.begin(9600);
    Serial.println("Control relay with serial - ready. www.BotBook.com");
}

void loop()
{
    char ch = Serial.read();
    if (ch=='2') {
        digitalWrite(relayPin, LOW);
    }
    if (ch=='8') {
        digitalWrite(relayPin, HIGH);
    }
}
