#include <SPI.h>
#include <MFRC522.h>
// пин для подключения реле
#define RELAY_RC522_PIN 41
// уровни для включения/выключения реле
#define RELAY_ON 0
#define RELAY_OFF 1
// Инициализация MFRC522
#define SS_PIN 53
#define RST_PIN 5

MFRC522 mfrc522(SS_PIN, RST_PIN);        // Create MFRC522 instance.
// константы подключения контактов SS и RST
//MFRC522 mfrc522(); // Create MFRC522 instance.
// массив разрешенных uid
byte uidok[][4]={
                 {0xE0, 0x2A, 0x87, 0x1B},
                 {0xD9, 0xFA, 0x90, 0x55 }
};
 
void setup() {
   Serial.begin(9600); // инициализация последовательного порта
   SPI.begin(); // инициализация SPI
   mfrc522.PCD_Init(); // инициализация MFRC522
   // сконфигурировать вывод реле как OUTPUT
   pinMode(RELAY_RC522_PIN,OUTPUT);
   digitalWrite(RELAY_RC522_PIN, RELAY_OFF);
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent()) {
   // чтение карты
   if ( mfrc522.PICC_ReadCardSerial()) {     
      // показать результат чтения UID и тип метки
      Serial.print(F("Card UID:"));
      if(compare_uid(mfrc522.uid.uidByte, mfrc522.uid.size)) {
          Serial.println();
          Serial.println(" – ok !!");
          // включить реле
          digitalWrite(RELAY_RC522_PIN, RELAY_ON);
          delay(4000);
          digitalWrite(RELAY_RC522_PIN, RELAY_OFF);
      } else {
          Serial.println();
          Serial.println(" – false !!");
          delay(1000);
      }
   }
  }
}
// поиск считанного uid в списке разрешенных
boolean compare_uid(byte *buffer, byte bufferSize) {
int bytes_ok=0;
   for(int i1=0;i1<2;i1++) {
      bytes_ok=0;
      for (byte i2 = 0; i2 < 4; i2++) {
         Serial.print(buffer[i2] < 0x10 ? " 0" : " ");
         Serial.print(buffer[i2], HEX);
         if(buffer[i2]== uidok[i1][i2]) {
            bytes_ok= bytes_ok+1;
         }
      }
      if(bytes_ok==4) {
         return true;
      }
   }
   return false;
}
