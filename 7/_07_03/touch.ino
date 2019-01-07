// Процедуры работы с touch
// shield TFT 2.4 240x320

// ************************************************
// по нажатии по экрану
void do_for_touch(int x,int y,int res) {
    // главный экран
    if(res==0) {
        // на sensors
        if(x<370 && x>320 && y<680 && y>280 ) {
           screen=1;
           view_display_sensors();
           Serial.println("11111111111111");
        }
        // на relays
        if(x<470 && x>420 && y<680 && y>280 ) {
           screen=2;
           view_display_relays();
           Serial.println("2222222");
        }
        // на relays
        if(x<570 && x>520 && y<680 && y>280 ) {
           screen=2;
           view_display_alarms();
           Serial.println("333333");
        }
    }
    // экран sensors
    else if(res==1) {
        // на main
        if(x<820 && x>780 && y<800 && y>160 ) {
           screen=0;
           view_display_main();
           Serial.println("00000000000");
       }
    }
    // экран relays
    else if(res==2) {
        // на main
        if(x<820 && x>780 && y<800 && y>160 ) {
           screen=0;
           view_display_main();
           Serial.println("00000000000");
        }
        else if(x<290 && x>270 && y<520 && y>440 ) {
           set_display_data_relay(0,datarelays[0],BLACK);
           datarelays[0]=1-datarelays[0];
           if(datarelays[0]==0) {
              digitalWrite(pinrelays[1],RELAY_OFF);
              set_display_data_relay(0,datarelays[0],YELLOW);
           }
           else {
              digitalWrite(pinrelays[1],RELAY_ON);
              set_display_data_relay(0,datarelays[0],RED);
           }
           Serial.println("22222200000");
       }
    
    }    
    // экран alarms
    else if(res==3) {
        // на main
        if(x<820 && x>780 && y<800 && y>160 ) {
           screen=0;
           view_display_main();
           Serial.println("00000000000");
        }
    
    }    
    // 
    else ;
}
// ************************************************

