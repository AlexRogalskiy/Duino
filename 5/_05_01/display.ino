// Процедуры вывода данных
// на дисплей
// Nokia 5110

// ************************************************
// инициализация дисплея
void ini_display() {
  display.begin();
  display.setContrast(50);
  display.clearDisplay();   // clears the screen and buffer
  display.display(); // show splashscreen

  display.setTextSize(1);
    
  Serial.println(F("start!"));
}
// ************************************************
// вывод экрана с показаниями датчиков
void view_display_sensors() {
  // очистить экран
  display.clearDisplay();   // clears the screen and buffer
  // вывод заголовка экрана
  // вывод списка датчиков
  for(int i=0;i<COUNT_SENSORS;i++) {
     display.setCursor(ysensors[i], xsensors[i]);
     display.print(strS1[i]);display.print("=");
  } 
  // вывод списка значений датчиков
 
  // вывод единиц измерения
  /*tft.setTextColor(YELLOW);  
  tft.setTextSize(2);
  for(int i=0;i<COUNT_SENSORS;i++) {
     tft.setCursor(200, xsensors[i]);
     tft.print(strS2[i]);
  }*/
  display.display(); // show splashscreen
  
}
// ************************************************
// вывести текущее значение для датчика
void set_display_data_sensor(int pos,int var,unsigned int color) {
   display.setTextColor(color);
   display.setCursor(ysensors[pos]+15, xsensors[pos]);
   display.print(var);
   display.display(); // show splashscreen
}

