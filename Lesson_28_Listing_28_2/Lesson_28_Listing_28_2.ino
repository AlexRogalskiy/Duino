// Подключение библиотек
#include <SPI.h>
#include <MFRC522.h>
// константы подключения контактов SS и RST
#define RST_PIN 9
#define SS_PIN 10
// Инициализация MFRC522
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
MFRC522:MIFARE_Key key;
byte sector = 1;
byte blockAddr = 4;
byte dataBlock[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
byte trailerBlock = 7;
byte status;
byte buffer[18];
byte size = sizeof(buffer);
void setup()
{
Serial.begin(9600); // инициализация последовательного порта
SPI.begin(); // инициализация SPI
mfrc522.PCD_Init(); // инициализация MFRC522
// Значение ключа (A или B) – FFFFFFFFFFFFh значение с завода
for (byte i = 0; i < 6; i++)
key.keyByte[i] = 0xFF;
}
void loop()
{
if ( ! mfrc522.PICC_IsNewCardPresent())
return;
// чтение карты
if ( ! mfrc522.PICC_ReadCardSerial())
return;
// показать результат чтения UID и тип метки
Serial.print(F("Card UID:"));
dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
Serial.println();
Serial.print(F("PICC type: "));
byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
Serial.println(mfrc522.PICC_GetTypeName(piccType));
// Чтение данных из блока 4
Serial.print(F("Reading data from block "));
Serial.print(blockAddr);
Serial.println(F(" ..."));
Serial.print(F("Data for count ")); Serial.print(blockAddr);
Serial.println(F(":"));
dump_byte_array(buffer, 2); Serial.println();
Serial.println();
for (byte i = 0; i < 16; i++) // запись в buffer[]
dataBlock[i]=buffer[i];
// получение байт счетчика (0 и 1)
int count1=(buffer[0]<<8)+buffer[1];
Serial.print("count1=");Serial.println(count1);
count1=count1+1; // инкремент счетчика
dataBlock[0]=highByte(count1);
dataBlock[1]=lowByte(count1);
// Аутентификация key B
Serial.println(F("Authenticating again using key B..."));
// Запись данных в блок
Serial.print(F("Writing data into block "));
Serial.print(blockAddr);
Serial.println(F(" ..."));
dump_byte_array(dataBlock, 2); Serial.println();
}
// Вывод результата чтения данных в HEX-виде
void dump_byte_array(byte *buffer, byte bufferSize)
{
for (byte i = 0; i < bufferSize; i++)
{
Serial.print(buffer[i] < 0x10 ? " 0" : " ");
Serial.print(buffer[i], HEX);
}
}
