// Проверка данных на превышение граничных
// и вывод на светодиоды


// ************************************************
// проверка данных на критичность
void control_data_limit(int par) {
   float val;
  
   switch(par) {
      case 0:  // DHT22
           val=datasensors[0];
           if((val<LIMIT_DHT22MIN || val>LIMIT_DHT22MAX) && aktivesensors[0]==1) {
             set_status_leds(0,HIGH);
             set_status_sound(0);
           }
           else
              set_status_leds(0,LOW);
         break;        
      case 1:  // DS18B20
           val=datasensors[1];
           if((val<LIMIT_DS18B20MIN || val>LIMIT_DS18B20MAX) && aktivesensors[1]==1) {
             set_status_leds(1,HIGH);
             set_status_sound(1);
           }
           else
              set_status_leds(1,LOW);
         break;        
      case 2:  // SoilMoisture
           val=datasensors[2];
           if((val<LIMIT_SOILMOISTUREMIN || val>LIMIT_SOILMOISTUREMAX) && aktivesensors[2]==1) {
             set_status_leds(2,HIGH);
             set_status_sound(2);
           }
           else
              set_status_leds(2,LOW);
         break;        
      case 3:  // waterlevel
           val=datasensors[3];
           if(val>LIMIT_LEVELWATER && aktivesensors[3]==1) {
             set_status_leds(3,HIGH);
             set_status_sound(3);
           }
           else
              set_status_leds(3,LOW);
         break;        
      case 4:  // fire
           val=datasensors[4];
           if(val>LIMIT_FAIR && aktivesensors[4]==1) {
             set_status_leds(4,HIGH);
             set_status_sound(4);
           }
           else
              set_status_leds(4,LOW);
         break;        
      case 5:  // mq2
           val=datasensors[5];
           if((datasensors[5]>LIMIT_MQ2P || datasensors[6]>LIMIT_MQ2M || datasensors[7]>LIMIT_MQ2S) && aktivesensors[5]==1) {
             set_status_leds(5,HIGH);
             set_status_sound(5);
           }
           else
              set_status_leds(5,LOW);
         break;   
      case 6:  // mq7
           val=datasensors[8];
           if(val>LIMIT_MQ7 && aktivesensors[6]==1) {
             set_status_leds(6,HIGH);
             set_status_sound(6);
           }
           else
              set_status_leds(6,LOW);
         break;   
      default:     
         break;
   }
}

// ************************************************
// индикация на светодиоды
void set_status_leds(int par,int val) {
     digitalWrite(pinleds[par],val);   
}

// ************************************************
// звуковая сигнализация
void set_status_sound(int par) {
   tone(SOUNDPIN,freq[par],durations[par]);
}

