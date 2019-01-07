
#include "defines.h"

// подключение библиотеки DHT
#include "DHT.h"
// подключение библиотеки OneWire
#include <OneWire.h>
// библиотека для работы с датчиками MQ 
#include <TroykaMQ.h>
// константы
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
int posdata=0;

// массив для хранения данных пинов светодиодов
int pinleds[]={26,28,30,32,34,36,38};
int statusleds[]={0,0,0,0,0,0,0};
// массив для хранения частот и длительностей
// для звуковых оповещений
int freq[]={293,329,349,370,392,440,494};
int durations[]={3000,3000,3000,4000,4000,4000,4000};
// состояния кнопки
int lastButtons1=0;
int currentButtons1=0;
// массив для хранения пинов relays
int pinrelays[]={27,29,31,33,35,37,39};
// массив для хранения данных relays
int aktiverelays[]={1,1,1,0,0,0,0,0};
int datarelays[]={1,0,1,0,0,0,1,0};
 
// библиотеки для работы с RTC (DS3231)
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

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
  ini_mcp23017();
  
   // входы подключения к мультиплексору D5, D7, D8 (GPIO 14, 13, 15) 
   // как OUTPUT
   pinMode(14,OUTPUT);
   pinMode(13,OUTPUT);
   pinMode(15,OUTPUT);  
  
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
      }
      millis_int1=millis();
   }
   // установка значений реле
   // вентилятор
   if(aktivesensors[1]==1 && datasensors[1]<999 
         && datasensors[1]>LIMIT_DS18B20MAX) {
      datarelays[2]=1;
      set_status_relays();
   }
   else {
      datarelays[2]=0;
      set_status_relays();
   }
   // помпа
   if(aktivesensors[2]==1 && datasensors[2]<999 && datasensors[1]>MAXVALUESOILMOISTURE) {
      datarelays[1]=1;
      set_status_relays();
   }
   else {
      datarelays[1]=0;
      set_status_relays();
   }
   // нажатие кнопки включения/выключения света
   currentButtons1 = debounce(lastButtons1, BUTTONLIGHTPIN1);
   if (lastButtons1 == 0 && currentButtons1 == 1) // если нажатие...
     {
     // изменить состояние реле
     datarelays[0]=1- datarelays[0];
     // вывести в порты MCP23017
     set_status_relays();
     }
   lastButtons1 = currentButtons1;
   // проверяем наступление события для будильников
   
   if(millis()-millisalarms >= 20000) {
     for(int i=0;i<sizeof(alarms)/8;i++) {
        if(alarms[i].hours==tm.Hour && alarms[i].minutes==tm.Minute) {
           // установить реле
           digitalWrite(pinrelays[alarms[i].relay],alarms[i].value);  
        }
     }
     millisalarms=millis();
  }
   

}



