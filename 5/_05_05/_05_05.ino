
#include "defines.h"

// Code provided by Smoke And Wires
// http://www.smokeandwires.co.nz
// This code has been taken from the Adafruit TFT Library and modified
//  by us for use with our TFT Shields / Modules
// For original code / licensing please refer to
// https://github.com/adafruit/TFTLCD-Library

#include <Adafruit_GFX.h>    // Core graphics library
#include "SWTFT.h" // Hardware-specific library

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
// #define LCD_CS A3 // Chip Select goes to Analog 3
// #define LCD_CD A2 // Command/Data goes to Analog 2
// #define LCD_WR A1 // LCD Write goes to Analog 1
// #define LCD_RD A0 // LCD Read goes to Analog 0

// #define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
//#define	BLACK   0x0000
//#define	BLUE    0x001F
//#define	RED     0xF800
//#define	GREEN   0x07E0
//#define CYAN    0x07FF
//#define MAGENTA 0xF81F
//#define YELLOW  0xFFE0
//#define WHITE   0xFFFF

SWTFT tft;
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// SWTFT tft;


// подключение библиотеки DHT
#include "DHT.h"
// подключение библиотеки OneWire
#include <OneWire.h>
// библиотека для работы с датчиками MQ 
#include <TroykaMQ.h>
// константы
//#define DHTPIN 22           // пин подключения контакта DHT22
//#define DS18B20PIN 23           // пин подключения контакта DS18B20
//#define DHTTYPE DHT11      // датчик DHT 11
//#define INTERVAL_GET_DATA 2000  // интервала измерений, мс
// создание экземпляра объекта DHT
DHT dht(DHTPIN,DHTTYPE);
// создание экземпляра объекта OneWire
OneWire ds(DS18B20PIN);

// создаём объект для работы с датчиком MQ-2
MQ2 mq2(MQ2PIN);
// создаём объект для работы с датчиком MQ-7
MQ7 mq7(MQ7PIN);

// переменная для интервала измерений
unsigned long millis_int1=0;
int teksensor=7;
// массив для хранения данных датчиков
float datasensors[]={999,999,999,999,999,999,999,999,999,999};
int aktivesensors[]={0,1,1,1,0,1,1,1,1,0};
int ysensors[]={100,100,100,100,100,100,100,100,100,100};
int xsensors[]={60,80,100,120,140,160,180,200,220,240};
int posdata=0;




void setup(void) {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
  ini_display();
  // вывод главного экрана 
  //view_display_clock();
  // вывод экрана сенсоров
  view_display_sensors();  
  // калибровка mq-2
  mq2.calibrate();
  mq2.getRo();
  // калибровка mq7
  mq7.calibrate();
  mq7.getRo();
}

void loop(void) {
   // показания с датчиков
   if(millis()-millis_int1 >= INTERVAL_GET_DATA) {
      teksensor=(teksensor+1)%COUNT_SENSORS;
      if(aktivesensors[teksensor]==1)  {
         float fvar=0;
         switch(teksensor) {
            case 0:  // DHT22
                fvar=get_data_humidity();
                Serial.print("humidity=");Serial.println(fvar);
              break;
            case 1:  // DS18B20
                fvar=get_data_ds18b20();
                Serial.print("tds18b20=");Serial.println(fvar);
              break;
            case 2:  // Soil Moisture
                fvar=get_data_soilmoisture();
                Serial.print("soilmoisture=");Serial.println(fvar);
              break;
            case 3:  // level water
                fvar=get_data_levelwater();
                Serial.print("levelwater=");Serial.println(fvar);
             break;
            case 5:  // mq-2 propan
                fvar=get_data_ppmpropan();
                Serial.print("ppmpropan=");Serial.println(fvar);
              break;
            case 6:  // mq-2 methan
                fvar=get_data_ppmmethan();
                Serial.print("ppmmethan=");Serial.println(fvar);
              break;
            case 7:  // mq-2 smoke
                fvar=get_data_ppmsmoke();
                Serial.print("ppmsmoke=");Serial.println(fvar);
              break;
            case 8:  // mq-7 CO
                fvar=get_data_ppmcarbonmonoxide();
                Serial.print("ppmcarbonmonoxide=");Serial.println(fvar);
              break;
            default:  // другие датчики
              break;
         } 
         if(fvar<999) {
            set_display_data_sensor(teksensor,datasensors[teksensor],BLACK);
            datasensors[teksensor]=fvar;
            set_display_data_sensor(teksensor,datasensors[teksensor],YELLOW);           
         }
      }
      millis_int1=millis();
   }
   

}



