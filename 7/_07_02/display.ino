// Процедуры вывода данных
// на дисплей
// shield TFT 2.4 240x320

// ************************************************
// инициализация дисплея
void ini_display() {
  tft.reset();
  uint16_t identifier = tft.readID();  
  Serial.print(F("LCD driver chip: "));
  Serial.println(identifier, HEX);
  tft.begin(identifier);
  tft.fillScreen(BLACK);
  tft.setRotation(0);
  Serial.println(F("start!"));
}
// ************************************************
// вывод главного экрана
void view_display_main() {
  // очистить экран
  tft.fillScreen(BLACK);
  // вывод заголовка экрана
  tft.setTextColor(MAGENTA);  
  tft.setTextSize(3);
  for(int i=0;i<6;i++) {
     tft.setCursor(15, xmenu[i]);
     tft.print(strM1[i]);
  } 
}
// вывод экрана с показаниями датчиков
void view_display_sensors() {
  // очистить экран
  tft.fillScreen(BLACK);
  // вывод заголовка экрана
  tft.setCursor(10, 15);
  tft.setTextColor(MAGENTA);  
  tft.setTextSize(3);
  tft.println("Sensors:");
  // вывод списка датчиков
  tft.setTextColor(YELLOW);  
  tft.setTextSize(2);
  for(int i=0;i<COUNT_SENSORS;i++) {
     tft.setCursor(10, xsensors[i]);
     tft.print(strS1[i]);
  } 
  // вывод списка значений датчиков
  for(int i=0;i<COUNT_SENSORS;i++) {
     if(datasensors[i]<999 && aktivesensors[i]==1) {
        set_display_data_sensor(i,datasensors[i],YELLOW);           
     }
  }
  // вывод единиц измерения
  tft.setTextColor(YELLOW);  
  tft.setTextSize(2);
  for(int i=0;i<COUNT_SENSORS;i++) {
     tft.setCursor(200, xsensors[i]);
     tft.print(strS2[i]);

  }
  // ardyino-kit  
  tft.setTextSize(3);
  tft.setTextColor(MAGENTA);  
  tft.setCursor(10, 270);
  tft.print("arduino-kit");
  // вывод для возврата в главное меню  
}
// ************************************************
// вывести текущее значение для датчика
void set_display_data_sensor(int pos,float var,unsigned int color) {
   tft.setTextSize(2);
   tft.setTextColor(color);  
   tft.setCursor(100, xsensors[pos]);
   tft.print(var);

}

// вывод экрана с показаниями реле
void view_display_relays() {
  // очистить экран
  tft.fillScreen(BLACK);
  // вывод заголовка экрана
  tft.setCursor(10, 15);
  tft.setTextColor(MAGENTA);  
  tft.setTextSize(3);
  tft.println("Relays:");
  // вывод списка реле
  tft.setTextColor(YELLOW);  
  tft.setTextSize(2);
  for(int i=0;i<COUNT_RELAYS;i++) {
     tft.setCursor(10, xrelays[i]);
     tft.print(strR1[i]);
  } 
  // вывод списка значений 
  for(int i=0;i<COUNT_RELAYS;i++) {
     if(aktiverelays[i]==1) {
        Serial.print("COUNT_RELAYS=");Serial.println(i);
        if(datarelays[i]==1) {
            set_display_data_relay(i,datarelays[i],RED); 
        }
        else {
            set_display_data_relay(i,datarelays[i],YELLOW); 
        }        
     }
  }


  // ardyino-kit  
  tft.setTextSize(3);
  tft.setTextColor(MAGENTA);  
  tft.setCursor(10, 270);
  tft.print("arduino-kit");
  // вывод для возврата в главное меню  
}

// вывести текущее значение для реле
void set_display_data_relay(int pos,float var,unsigned int color) {
   tft.setTextSize(2);
   tft.setTextColor(color);  
   tft.setCursor(100, xrelays[pos]);
   if(var==0)
      tft.print("OFF");
   else
      tft.print("ON");
}

// вывести текущее значение для RTC
void set_display_data_RTC(unsigned int color) {
   tft.setTextSize(2);
   tft.setTextColor(color);  
   tft.setCursor(80, 50);
   if(tm.Hour<10)
      tft.print("0");
   tft.print(tm.Hour);
   tft.print(":");
   if(tm.Minute<10)
      tft.print("0");
   tft.print(tm.Minute);
      

}
