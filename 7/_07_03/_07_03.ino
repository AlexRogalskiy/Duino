
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
#include <stdint.h>
#include "TouchScreen.h"

// подключение библиотеки DHT
#include "DHT.h"
// подключение библиотеки OneWire
#include <OneWire.h>
// библиотека для работы с датчиками MQ 
#include <TroykaMQ.h>
// библиотеки для работы с RTC (DS3231)
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

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


// x y display для сенсоров
int ysensors[]={100,100,100,100,100,100,100,100,100,100};
int xsensors[]={60,80,100,120,140,160,180,200,220,240};
int posdata=0;
// x y display для главного меню
int ymenu[]={100,100,100,100,100,100,100,100,100,100};
int xmenu[]={50,90,130,170,270};
// x y display для relays
int yrelays[]={100,100,100,100,100,100,100,100,100,100};
int xrelays[]={60,80,100,120,140,160,180,200,220,240};


// массив для хранения пинов relays
int pinrelays[]={27,29,31,33,35,37,39};
// массив для хранения данных relays
int aktiverelays[]={1,1,1,0,0,0,0,0};
int datarelays[]={1,0,1,0,0,0,1,0};


// массив для хранения данных пинов светодиодов
int pinleds[]={26,28,30,32,34,36,38};
// массив для хранения частот и длительностей
// для звуковых оповещений
int freq[]={293,329,349,370,392,440,494};
int durations[]={3000,3000,3000,4000,4000,4000,4000};

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
int screen=0;      // 0 - датчики
                   // 1 - реле
                   // 2 - будильники
                   
// создаём объект для работы с RTC
tmElements_t tm;
unsigned long millisds3231;

// список будильников
struct ALARM{
  int hours;
  int minutes;
  int relay;
  int value;
};
ALARM alarms[]={{7,30,2,1},
                {7,35,2,0},
                {8,30,1,1},
                {8,50,1,0}
              };
unsigned long millisalarms;

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
  ini_display();
  // вывод главного экрана 
  //view_display_clock();
  // вывод экрана сенсоров
//view_display_sensors(); 
  view_display_main();
  // калибровка mq-2
  mq2.calibrate();
  mq2.getRo();
  // калибровка mq7
  mq7.calibrate();
  mq7.getRo();
  // пины leds и sound на вывод
  for(int i=0;i<7;i++) {
     pinMode(pinleds[i],OUTPUT);
     digitalWrite(pinleds[i],LOW);
  }
  pinMode(SOUNDPIN,OUTPUT);
  digitalWrite(SOUNDPIN,LOW);
  // время RTC
  RTC.read(tm);
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
                control_data_limit(0);
              break;
            case 1:  // DS18B20
                fvar=get_data_ds18b20();
                Serial.print("tds18b20=");Serial.println(fvar);
                control_data_limit(1);
              break;
            case 2:  // Soil Moisture
                fvar=get_data_soilmoisture();
                Serial.print("soilmoisture=");Serial.println(fvar);
                control_data_limit(2);
              break;
            case 3:  // level water
                fvar=get_data_levelwater();
                Serial.print("levelwater=");Serial.println(fvar);
                control_data_limit(3);
             break;
            case 5:  // mq-2 propan
                fvar=get_data_ppmpropan();
                Serial.print("ppmpropan=");Serial.println(fvar);
                control_data_limit(5);
              break;
            case 6:  // mq-2 methan
                fvar=get_data_ppmmethan();
                Serial.print("ppmmethan=");Serial.println(fvar);
                control_data_limit(5);
              break;
            case 7:  // mq-2 smoke
                fvar=get_data_ppmsmoke();
                Serial.print("ppmsmoke=");Serial.println(fvar);
                control_data_limit(5);
              break;
            case 8:  // mq-7 CO
                fvar=get_data_ppmcarbonmonoxide();
                Serial.print("ppmcarbonmonoxide=");Serial.println(fvar);
                control_data_limit(6);
              break;
            default:  // другие датчики
              break;
         } 
         if(fvar<999 && screen==1) {
            set_display_data_sensor(teksensor,datasensors[teksensor],BLACK);
            datasensors[teksensor]=fvar;
            set_display_data_sensor(teksensor,datasensors[teksensor],YELLOW);           
         }
      }
      millis_int1=millis();
   }
   // установка значений реле
   // вентилятор
   
   if(aktivesensors[1]==1 && datasensors[1]<999 && datasensors[1]>LIMIT_DS18B20MAX) {
      digitalWrite(pinrelays[2],RELAY_ON);
      if( datarelays[2]<1 && screen==2)
        set_display_data_relay(2,datarelays[2],RED); 
      datarelays[2]=1;
   }
   else {
      digitalWrite(pinrelays[2],RELAY_OFF);
      if( datarelays[2]>0 && screen==2)
        set_display_data_relay(2,datarelays[2],YELLOW); 
      datarelays[2]=0;
   }
   // помпа
   if(aktivesensors[2]==1 && datasensors[2]<999 && datasensors[1]>MAXVALUESOILMOISTURE) {
      digitalWrite(pinrelays[1],RELAY_ON);
      if( datarelays[1]<1 && screen==2)
        set_display_data_relay(1,datarelays[1],RED); 
      datarelays[1]=1;
   }
   else {
      digitalWrite(pinrelays[1],RELAY_OFF);
      if( datarelays[1]>0 && screen==2)
        set_display_data_relay(1,datarelays[1],YELLOW); 
      datarelays[1]=0;
   }
      
   // определяем нажатие на экран
  int tt=ts.pressure();
  if(tt > MINPRESSURE && tt < MAXPRESSURE) {
     Serial.println("touch");
     int x=ts.readTouchX();int y=ts.readTouchY();
     Serial.print("X = "); Serial.print(x);
     Serial.print("\tY = "); Serial.print(y);
     pinMode(XM, OUTPUT);pinMode(YP, OUTPUT);
     pinMode(XP, OUTPUT);pinMode(YM, OUTPUT);
     do_for_touch(x,y,screen) ;
     delay(1000);
  }   
  pinMode(XM, OUTPUT);pinMode(YP, OUTPUT);
  pinMode(XP, OUTPUT);pinMode(YM, OUTPUT);
  // показания RTC
  if(millis()-millisds3231 >= 5000 && screen==0) {
     set_display_data_RTC(BLACK);
     RTC.read(tm);
     set_display_data_RTC(MAGENTA);
     millisds3231=millis();
  }
  // проверяем наступление события для будильников
  if(millis()-millisalarms >= 20000) {
     for(int i=0;i<sizeof(alarms)/8;i++) {
        if(alarms[i].hours==tm.Hour && alarms[i].minutes==tm.Minute) {
           digitalWrite(pinrelays[alarms[i].relay],alarms[i].value);  
        }
     }
     millisalarms=millis();
  }
    

}



