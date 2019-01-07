//#include <String.h>

#define OK 1
#define NOTOK 2
#define TIMEOUT 3
#define RST 2

#define SIM900board Serial1
#define SIM800baud 9600
#define SERIALTIMEOUT 3000
/*#ifndef A6board
SoftwareSerial SIM800board (10, 3);
#define SIM800baud 9600
#else

#define SERIALTIMEOUT 3000
*/
char end_c[2];

void setup() {
  SIM800board.begin(SIM800baud);   // the GPRS baud rate
  Serial.begin(9600);    // the GPRS baud rate
  // ctrlZ String definition
  end_c[0] = 0x1a;
  end_c[1] = '\0';
  Serial.println("Start");

  pinMode(RST, OUTPUT);
  digitalWrite(RST, HIGH);
  delay(5000);
  digitalWrite(RST, LOW);
  delay(500);
  if (SIM800begin() != OK) {
    Serial.println("Error");
    while (1 == 1);
  }
}

void loop()
{

  if (Serial.available())
    switch (Serial.read())
    {
      case 't':
        Serial.println("Send SMS");
        SendTextMessage();
        break;
      case 'v':
        Serial.println("Make voice call");
        DialVoiceCall();
        break;
      case 's':
        Serial.println("----------------------Post value to Sparkfun-------------------------------");
        float batt = (float)analogRead(A0) * 5.165 / 594.0; // conversion factor measured with multimeter
        sendSparkfunGSM(1, batt);
        break;
    }
  if ( SIM800board.available())
    Serial.write( SIM800board.read());

  //delay(2000);
}


///SendTextMessage()
///this function is to send a sms message
void SendTextMessage()
{
  SIM800command("AT+CMGF=1\r", "OK", "yy", 2000, 2); //Because we want to send the SMS in text mode
  delay(100);
  SIM800command("AT+CMGS = \"+79034461752\"", ">", "yy", 20000, 2); //send sms message, be careful need to add a country code before the cellphone number
  delay(100);
  SIM800board.println("GSM test message!");//the content of the message
  SIM800board.println(end_c);//the ASCII code of the ctrl+z is 26
  SIM800board.println();
}

///DialVoiceCall
///this function is to dial a voice call
void DialVoiceCall()
{
  SIM800command("AT+SNFS=0", "OK", "yy", 20000, 2);
  SIM800command("ATD+79034461752;", "OK", "yy", 20000, 2); //dial the number
}

///SubmitHttpRequest()
///this function is submit a http request
///attention:the time of delay is very important, it must be set enough

///sparkfun()///
bool sendSparkfunGSM(byte sparkfunType, float value1) {
  String host = "data.sparkfun.com";
  String publicKey   = "9b1nm53N68cDAOvVEJgK";
  //                    9b1nm53N68cDAOvVEJgK
  String privateKey = "xv9NwElKY7ibJ57Vdaje";
  

  SIM800command("AT+CREG=0", "OK", "yy", 10000, 2);
  SIM800command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  SIM800command("AT+CGATT?", "OK", "yy", 20000, 2);
  SIM800command("AT+CGATT=1", "OK", "yy", 20000, 2);
  SIM800command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  SIM800command("AT+CSTT=\"internet.mts.ru\",\"mts\",\"mts\"", "OK", "yy", 20000, 2); //bring up wireless connection
  SIM800command("AT+CGDCONT=1,\"IP\",\"internet.mts.ru\"", "OK", "yy", 20000, 2); //bring up wireless connection
  SIM800command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  SIM800command("AT+CGACT=1,1", "OK", "yy", 10000, 2);
  SIM800command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  SIM800command("AT+CIICR", "OK", "yy", 20000, 2); //bring up wireless connection
  SIM800command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  SIM800command("AT+CIFSR", "OK", "yy", 20000, 2); //get local IP adress
  SIM800command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  SIM800command("AT+CIPSTART=\"TCP\",\"" + host + "\",80", "CONNECT OK", "yy", 25000, 2); //start up the connection
  // 
  SIM800command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  SIM800command("AT+CIPSEND", ">", "yy", 10000, 1); //begin send data to remote server
  delay(500);
  SIM800board.print("GET /input/");
  SIM800board.print(publicKey);
  SIM800board.print("?private_key=");
  SIM800board.print(privateKey);
  SIM800board.print("&battery=");
  SIM800board.print(value1, 2);
  SIM800board.print(" HTTP/1.1");
  SIM800board.print("\r\n");
  SIM800board.print("HOST: ");
  SIM800board.print(host);
  SIM800board.print("\r\n");
  SIM800board.print("\r\n");

  Serial.print("GET /input/");
  Serial.print(publicKey);
  Serial.print("?private_key=");
  Serial.print(privateKey);
  Serial.print("&battery=");
  Serial.print(value1, 2);
  Serial.print(" HTTP/1.1");
  Serial.print("\r\n");
  Serial.print("HOST: ");
  Serial.print(host);
  Serial.print("\r\n");
  Serial.print("\r\n");

  SIM800command(end_c, "HTTP/1.1", "yy", 30000, 1); //begin send data to remote server
  SIM800board.println(end_c); //sending ctrlZ
  unsigned long   entry = millis();
  SIM800command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  SIM800command("AT+CIPCLOSE", "OK", "yy", 15000, 1); //sending
  SIM800command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  delay(100);
  Serial.println("-------------------------End------------------------------");
}


byte SIM800waitFor(String response1, String response2, int timeOut) {
  unsigned long entry = millis();
  int count = 0;
  String reply = SIM800read();
  byte retVal = 99;
  do {
    reply = SIM800read();
    if (reply != "") {
      Serial.print((millis() - entry));
      Serial.print(" ms ");
      Serial.println(reply);
    }
  } while ((reply.indexOf(response1) + reply.indexOf(response2) == -2) && millis() - entry < timeOut );
  if ((millis() - entry) >= timeOut) {
    retVal = TIMEOUT;
  } else {
    if (reply.indexOf(response1) + reply.indexOf(response2) > -2) retVal = OK;
    else retVal = NOTOK;
  }
  //  Serial.print("retVal = ");
  //  Serial.println(retVal);
  return retVal;
}

byte SIM800command(String command, String response1, String response2, int timeOut, int repetitions) {
  byte returnValue = NOTOK;
  byte count = 0;
  while (count < repetitions && returnValue != OK) {
    SIM800board.println(command);
    Serial.print("Command: ");
    Serial.println(command);
    if (SIM800waitFor(response1, response2, timeOut) == OK) {
      //     Serial.println("OK");
      returnValue = OK;
    } else returnValue = NOTOK;
    count++;
  }
  return returnValue;
}




void SIM800input() {
  String hh;
  char buffer[100];
  while (1 == 1) {
    if (Serial.available()) {
      hh = Serial.readStringUntil('\n');
      hh.toCharArray(buffer, hh.length() + 1);
      if (hh.indexOf("ende") == 0) {
        SIM800board.write(end_c);
        Serial.println("ende");
      } else {
        SIM800board.write(buffer);
        SIM800board.write('\n');
      }
    }
    if (SIM800board.available()) {
      Serial.write(SIM800board.read());
    }
  }
}


bool SIM800begin() {
  SIM800board.println("AT+CREG?");
  byte hi = SIM800waitFor("1,", "5,", 1500);  // 1: registered, home network ; 5: registered, roaming
  while ( hi != OK) {
    SIM800board.println("AT+CREG?");
    hi = SIM800waitFor("1,", "5,", 1500);
  }

  if (SIM800command("AT&F0", "OK", "yy", 5000, 2) == OK) {   // Reset to factory settings
    if (SIM800command("ATE0", "OK", "yy", 5000, 2) == OK) {  // disable Echo
      if (SIM800command("AT+CMEE=2", "OK", "yy", 5000, 2) == OK) return OK;  // enable better error messages
      else return NOTOK;
    }
  }
}

void ShowSerialData()
{
  unsigned long entry = millis();
  while ( SIM800board.available() != 0 && millis() - entry < SERIALTIMEOUT)
    Serial.println(SIM800board.readStringUntil('\n'));
}

String SIM800read() {
  String reply = "";
  if (SIM800board.available())  {
    reply = SIM800board.readString();
  }
  //  Serial.print("Reply: ");
  //  Serial.println(reply);
  return reply;
}

