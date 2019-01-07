
#include "defines.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
// Software SPI (slower updates, more flexible pin options):
// pin 3 - Serial clock out (SCLK)
// pin 4 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 7 - LCD chip select (CS)
// pin 6 - LCD reset (RST)

Adafruit_PCD8544 display = Adafruit_PCD8544(3, 4, 5, 7, 6);


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

// x y display для сенсоров
int ysensors[]={100,100,100,100,100,100,100,100,100,100};
int xsensors[]={60,80,100,120,140,160,180,200,220,240};
int posdata=0;
// x y display для главного меню
int ymenu[]={100,100,100,100,100,100,100,100,100,100};
int xmenu[]={50,90,130,170,270};

// массив для хранения данных пинов светодиодов
int pinleds[]={26,28,30,32,34,36,38};
// массив для хранения частот и длительностей
// для звуковых оповещений
int freq[]={293,329,349,370,392,440,494};
int durations[]={3000,3000,3000,4000,4000,4000,4000};

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
  ini_display();
view_display_sensors(); 
  //view_display_main();
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
         if(fvar<999 ) {
            set_display_data_sensor(teksensor,datasensors[teksensor],BLACK);
            datasensors[teksensor]=fvar;
            set_display_data_sensor(teksensor,datasensors[teksensor],YELLOW);           
         }
      }
      millis_int1=millis();
   }

   

}



