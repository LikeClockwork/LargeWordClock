#include <Wire.h>
#include <RTClib.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "Clock16x16mapping.h"


RTC_DS1307 RTC;
Adafruit_8x16matrix matrix = Adafruit_8x16matrix();
Adafruit_8x16matrix matrix2 = Adafruit_8x16matrix();


uint16_t active[16];  //Holds active matrix
uint16_t last[16];  //Holds previous matrix

unsigned int curHours;
unsigned int hours = 99;
unsigned int curMins;
unsigned int mins = 99;
int aniChoice = 0;

void setup() { 
  //Serial.begin(57600);

  
  RTC.begin();
  delay(100);
 
matrix.begin(0x71);  // pass in the address
matrix2.begin(0x70);  // pass in the address
delay(100);
 
 matrix.clear();
 matrix2.clear();
 matrix.writeDisplay();
 matrix2.writeDisplay();
 matrix2.drawPixel(8, 15, LED_ON);
 matrix2.writeDisplay();  
 matrix2.drawPixel(8, 15, LED_OFF);
 matrix2.writeDisplay();  
 matrix2.drawPixel(8, 15, LED_ON);
 matrix2.writeDisplay();  
 matrix2.drawPixel(8, 15, LED_OFF);
 matrix2.writeDisplay(); 

 pinMode(5, INPUT);
 pinMode(6, INPUT);
 pinMode(7, INPUT);

  if(!RTC.isrunning()) {
    RTC.adjust(DateTime(2015,4,18,0,0,0));
  }

  DateTime now = RTC.now();

  hours = now.hour();
  mins = now.minute(); 
  doAnimation();  
  delay(100);

}

unsigned long currentMillis = millis();
unsigned long previousMillis = 0; 
unsigned long resetMillis = 0; 
unsigned long pressCheckA0 = 0;
unsigned long pressCheckA1 = 0;
unsigned long pressCheckA2 = 0;


bool A0pressed = false;
bool A1pressed = false;
bool A2pressed = false;
bool A0read = false;
bool A1read = false;
bool A2read = false;

void loop () {
  
 
   if((unsigned long)(currentMillis - previousMillis) >= 5000)  {
   
    //Serial.print(hours);
    //Serial.print(':');
    //Serial.println(mins);
    checkTime();
    previousMillis = currentMillis; 
     
  }
   
  
  if( digitalRead(5) == HIGH && A0pressed == false) {
     A0pressed = true;
     pressCheckA0 = currentMillis;
     A0read = false;
  }  

  if( digitalRead(6) == HIGH && A1pressed == false) {
      A1pressed = true;
      pressCheckA1 = currentMillis;
      A1read = false;
  }  


  if( digitalRead(7) == HIGH && A2pressed == false) {
      A2pressed = true;
      pressCheckA2 = currentMillis;
      A2read = false;
  }

  if( A0pressed == true && A0read == false && (unsigned long)(currentMillis - pressCheckA0) >= 50) {
      DateTime now = RTC.now();
      int newHour = now.hour() + 1;
      if (newHour > 23) {
         newHour = 0; 
      }
      RTC.adjust(DateTime(now.year(),now.month(),now.day(),newHour,now.minute(),now.second()));
      hours = newHour;
      mins = now.minute(); 
      curHours = newHour;
      curMins = now.minute(); 
      wipe();
      assemble();
      drawDisplay();
      A0read = true;
  }
  
  if( A1pressed == true && A1read == false && (unsigned long)(currentMillis - pressCheckA1) >= 50) {
      DateTime now = RTC.now();   
      int newMinute = now.minute() + 1;
      if (newMinute > 59) {
         newMinute = 0; 
      }
      RTC.adjust(DateTime(now.year(),now.month(),now.day(),now.hour(),newMinute,now.second()));
      hours = now.hour();
      mins = newMinute; 
      curHours = now.hour();
      curMins = newMinute;
      wipe();
      assemble();
      drawDisplay();
      A1read = true;
  }


  if( A2pressed == true && A2read == false && (unsigned long)(currentMillis - pressCheckA2) >= 50) {  
      aniChoice++;
      sampleAnimation();
      A2read = true;
  }


  if( digitalRead(5) == LOW) {
     A0pressed = false; 
     A0read = false;
  }
   
  if( digitalRead(6) == LOW) {
     A1pressed = false; 
     A1read = false;
  } 
 
  if( digitalRead(7) == LOW) {
     A2pressed = false; 
     A2read = false;
  } 

  wipe();
  assemble();
  delay(10); 
  currentMillis = millis();
}

void checkTime () {
  DateTime now = RTC.now();
  curHours = now.hour();
  curMins = now.minute();  

  if (curHours != hours || curMins != mins) {
    hours = curHours;
    mins = curMins;
   //software_Reset();
    doAnimation();

  }  
  
}



void assemble() {
  
  for( int i = 0; i < 16; i++ ) {
    active[i] = active[i] | pgm_read_word(&(it_is[i]));
  }  
  
  
  //flag midnight   
  if (hours == 0 && mins == 0) {
    for( int i = 0; i < 16; i++ ) {
      active[i] = active[i] | pgm_read_word(&(midnight[i]));
    }     
  } else
  
  //flag noon   
  if (hours == 12 && mins == 0) {
    for( int i = 0; i < 16; i++ ) {
      active[i] = active[i] | pgm_read_word(&(noon[i]));
    }     
  } else {
  
    //If it's between 12:01 and 5:59 in the PM, flag in the afternoon   
    if (hours > 12 && hours < 18 || ( hours == 12 && mins > 0)) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(in_the_afternoon[i]));
        }
    } else
    
    //If it's between 6 and 8:59 in the PM, flag in the evening   
    if (hours > 17 && hours < 21) {
      for( int i = 0; i < 16; i++ ) {
        active[i] = active[i] | pgm_read_word(&(in_the_evening[i]));
      }     
    } else
    
    
    //If it's between 9 and midnight in the PM, flag at night   
    if (hours > 20) {
      for( int i = 0; i < 16; i++ ) {
        if(hours == 23 && mins == 45) {} else {
          active[i] = active[i] | pgm_read_word(&(at_night[i]));
        }
      }     
    } else
    
    //If it's between 12:01 and 11:59 in the AM, flag in the morning  
    if ( ((hours > 0 && hours < 12) || (hours == 0 && mins > 0)) ) {
        if(hours == 11 && mins == 45) {
        } else {
          for( int i = 0; i < 16; i++ ) {
            active[i] = active[i] | pgm_read_word(&(in_the_morning[i]));
          }   
        }
    }
      
    if ( hours == 12 && mins != 0) {
      for( int i = 0; i < 16; i++ ) {
        if (mins == 45) {
          active[i] = active[i] | pgm_read_word(&(h1[i]));
        } else {
          active[i] = active[i] | pgm_read_word(&(h12[i]));
        }
      }
    }  else
    
    if ( hours == 0 && mins != 0) {
      for( int i = 0; i < 16; i++ ) {
        if (mins == 45) {
          active[i] = active[i] | pgm_read_word(&(h1[i]));
        } else {
          active[i] = active[i] | pgm_read_word(&(h12[i]));
        }
      }
    }  else
    
    if ( hours == 1 || hours == 13) {
      for( int i = 0; i < 16; i++ ) {
        if (mins == 45) {
          active[i] = active[i] | pgm_read_word(&(h2[i]));
        } else {
          active[i] = active[i] | pgm_read_word(&(h1[i]));
        }
      }
    } else
    
    if ( hours == 2 || hours == 14) {
      for( int i = 0; i < 16; i++ ) {
       if (mins == 45) {
          active[i] = active[i] | pgm_read_word(&(h3[i]));
        } else {
          active[i] = active[i] | pgm_read_word(&(h2[i]));
        }
      }
    } else
    
    if ( hours == 3 || hours == 15) {
      for( int i = 0; i < 16; i++ ) {
        if (mins == 45) {
          active[i] = active[i] | pgm_read_word(&(h4[i]));
        } else {
          active[i] = active[i] | pgm_read_word(&(h3[i]));
        }
      }
    } else
    
    if ( hours == 4 || hours == 16) {
      for( int i = 0; i < 16; i++ ) {
       if (mins == 45) {
          active[i] = active[i] | pgm_read_word(&(h5[i]));
        } else {
          active[i] = active[i] | pgm_read_word(&(h4[i]));
        }
      }
    } else
    
    if ( hours == 1) {
      for( int i = 0; i < 16; i++ ) {
        if (mins == 45) {
          active[i] = active[i] | pgm_read_word(&(h2[i]));
        } else {
          active[i] = active[i] | pgm_read_word(&(h1[i]));
        }
      }
    } else
    
    if ( hours == 5 || hours == 17) {
      for( int i = 0; i < 16; i++ ) {
       if (mins == 45) {
          active[i] = active[i] | pgm_read_word(&(h6[i]));
        } else {
          active[i] = active[i] | pgm_read_word(&(h5[i]));
        }
      }
    } else
    
    if ( hours == 6 || hours == 18) {
      for( int i = 0; i < 16; i++ ) {
       if (mins == 45) {
          active[i] = active[i] | pgm_read_word(&(h7[i]));
        } else {
          active[i] = active[i] | pgm_read_word(&(h6[i]));
        }
      }
    } else
    
    if ( hours == 7 || hours == 19) {
      for( int i = 0; i < 16; i++ ) {
        if (mins == 45) {
          active[i] = active[i] | pgm_read_word(&(h8[i]));
        } else {
          active[i] = active[i] | pgm_read_word(&(h7[i]));
        }
      }
    } else
    
    if ( hours == 8 || hours == 20) {
      for( int i = 0; i < 16; i++ ) {
        if (mins == 45) {
          active[i] = active[i] | pgm_read_word(&(h9[i]));
        } else {
          active[i] = active[i] | pgm_read_word(&(h8[i]));
        }
      }
    } else
    
    if ( hours == 9 || hours == 21) {
      for( int i = 0; i < 16; i++ ) {
        if (mins == 45) {
          active[i] = active[i] | pgm_read_word(&(h10[i]));
        } else {
          active[i] = active[i] | pgm_read_word(&(h9[i]));
        }
      }
    } else
    
    if ( hours == 10 || hours == 22) {
      for( int i = 0; i < 16; i++ ) {
        if (mins == 45) {
          active[i] = active[i] | pgm_read_word(&(h11[i]));
        } else {
          active[i] = active[i] | pgm_read_word(&(h10[i]));
        }
      }
    } else
    
    if ( hours == 11) {
      for( int i = 0; i < 16; i++ ) {
        if (mins == 45) {
          active[i] = active[i] | pgm_read_word(&(noon[i]));
        } else {
          active[i] = active[i] | pgm_read_word(&(h11[i]));
        }
      }
    } else
    
    if ( hours == 23) {
      for( int i = 0; i < 16; i++ ) {
        if (mins == 45) {
          active[i] = active[i] | pgm_read_word(&(midnight[i]));
        } else {
          active[i] = active[i] | pgm_read_word(&(h11[i]));
        }
      }
    }  
  
    //Minute handling
    if ( mins == 15) {
      for( int i = 0; i < 16; i++ ) {
        active[i] = active[i] | pgm_read_word(&(m15[i]));
      }
    } else
    
     if ( mins == 45) {
      for( int i = 0; i < 16; i++ ) {
        active[i] = active[i] | pgm_read_word(&(m45[i]));
      }
    } else 
    
     if ( mins == 30) {
      for( int i = 0; i < 16; i++ ) {
        active[i] = active[i] | pgm_read_word(&(half_past[i]));
      }
    } else 
    
     if ( mins == 0  && (hours != 12 && hours != 0)) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(o_clock[i]));
        }
        
    }  else  {
      
      if ( mins < 10 && mins > 0) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(o[i]));
        }
      }  
  
      if ( mins == 1 || mins == 21 || mins == 31 || mins == 41 || mins == 51) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m1[i]));
        }
      } else
      
      if ( mins == 2 || mins == 22 || mins == 32 || mins == 42 || mins == 52) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m2[i]));
        }
      } else
      
      if ( mins == 3 || mins == 23 || mins == 33 || mins == 43 || mins == 53) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m3[i]));
        }
      } else
      
      if ( mins == 4 || mins == 24 || mins == 34 || mins == 44 || mins == 54) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m4[i]));
        }
      } else
      
      if ( mins == 5 || mins == 25 || mins == 35 || mins == 45 || mins == 55) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m5[i]));
        }
      } else
      
      if ( mins == 6 || mins == 26 || mins == 36 || mins == 46 || mins == 56) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m6[i]));
        }
      } else
      
      if ( mins == 7 || mins == 27 || mins == 37 || mins == 47 || mins == 57) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m7[i]));
        }
      } else
      
      if ( mins == 8 || mins == 28 || mins == 38 || mins == 48 || mins == 58) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m8[i]));
        }
      } else
      
      if ( mins == 9 || mins == 29 || mins == 39 || mins == 49 || mins == 59) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m9[i]));
        }
      } else
      
      if ( mins == 10 ) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m10[i]));
        }
      } else
      
      if ( mins == 11 ) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m11[i]));
        }
      } else
      
      if ( mins == 12 ) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m12[i]));
        }
      } else  
      
      if ( mins == 13 ) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m13[i]));
        }
      } else  
      
      if ( mins == 14 ) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m14[i]));
        }
      } else  
      
      if ( mins == 16 ) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m16[i]));
        }
      } else  
      
      if ( mins == 17 ) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m17[i]));
        }
      } else  
      
      if ( mins == 18 ) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m18[i]));
        }
      } else  
      
      if ( mins == 19 ) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m19[i]));
        }
      }   
      
      if ( mins >= 20 && mins < 30 ) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m20[i]));
        }
      }   
      
      if ( mins >= 30 && mins < 40 ) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m30[i]));
        }
      }  
      
      if ( mins >= 40 && mins < 50 ) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m40[i]));
        }
      }    
      
      if ( mins >= 50 && mins < 60 ) {
        for( int i = 0; i < 16; i++ ) {
          active[i] = active[i] | pgm_read_word(&(m50[i]));
        }
      } 
    } //end quarter hour if-else
      
  } //end noon-midnight if-else
}

void wipe () {
    for( int i = 0; i < 16; i++ ) {
      active[i] = 0b0000000000000000;
    }  
}


//Animation functions

//No animation
void drawDisplay() {      
    
    matrix.clear();
    matrix2.clear();
    wipe();
    assemble();
    
     for( int y = 16; y >= 0; y-- ) {
       for( int x = 0; x < 16; x++) {  

         if(bitRead( active[15 - y] , 15 - x)) {
            if( y < 8) {
                matrix.drawPixel(y, x, LED_ON);  
            } else {
                matrix2.drawPixel(y - 8, x, LED_ON);                
            }
          } else {

          }
        } 
     }
      matrix.writeDisplay();  
      matrix2.writeDisplay(); 
}

//Fade out and back in
void fadeOut() {
     for(int b = 16; b >= 0; b--) {
       matrix.setBrightness(b); 
       matrix2.setBrightness(b); 
       delay(50);
    }
    wipe();
    drawDisplay();
    delay(250);
    assemble();
    drawDisplay();
    for(int b = 0; b < 16 ; b++) {
       matrix.setBrightness(b); 
       matrix2.setBrightness(b); 
       delay(50);
    } 
}

//Random dissolve to all on, then fade back to time
void whiteOut () {
  
      int randomizer[256] = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
        33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
        49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
        65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96,
        97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
        113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128,
        129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144,
        145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 
        161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 
        177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 
        193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 
        209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 
        225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 
        241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256
      };  
  

     bubbleUnsort (randomizer, 256);    
  
     for( int i = 0; i < 256; i++) {
        
          int y = randomizer[i ]/16;
          int x = randomizer[i ]%16;
   
         if( y < 8) {
            matrix.drawPixel(y, x, LED_ON);  
          } else {
            matrix2.drawPixel(y - 8, x, LED_ON);                
          }

     
        matrix.writeDisplay();  
        matrix2.writeDisplay();
        
     }
     
     wipe();
     bubbleUnsort (randomizer, 256);    
     assemble();

     for( int i = 0; i < 256; i++) {
        
          int y = randomizer[i]/16;
          int x = randomizer[i]%16;
   
         if(bitRead( active[15 - y] , 15 - x)) {
            if( y < 8) {
                matrix.drawPixel(y, x, LED_ON);  
            } else {
                matrix2.drawPixel(y - 8, x, LED_ON);                
            }
          } else {
            if( y < 8) {
                matrix.drawPixel(y, x, LED_OFF);  
            } else {
                matrix2.drawPixel(y - 8, x, LED_OFF);                
            }            
          }
       
     
        matrix.writeDisplay();  
        matrix2.writeDisplay();
        
     }
}

//Horizontal sweep.
void sweep() {      
      
    for( int x = 0; x < 16; x++) {
      last[x] = active[x]; 
    }
    

     wipe();
     assemble();   
   
     for( int x = 0; x < 16; x++) {  
       
       for( int y = 16; y >= 0; y-- ) {   

         if(!bitRead( active[15 - y] , 15 - x)) {
            if( y < 8) {
                matrix.drawPixel(y, x, LED_OFF);  
            } else {
                matrix2.drawPixel(y - 8, x, LED_OFF);                
            }
          }
          matrix.writeDisplay();  
          matrix2.writeDisplay(); 
       }          
     }
     
     for( int x = 0; x < 16; x++) {  
       
       for( int y = 16; y >= 0; y-- ) {  
         
         if(bitRead( active[15 - y] , 15 - x)) {
            if( y < 8) {
                matrix.drawPixel(y, x, LED_ON);  
            } else {
                matrix2.drawPixel(y - 8, x, LED_ON);                
            }
          }
          
          matrix.writeDisplay();  
          matrix2.writeDisplay(); 
        } 
     }
}

//Matrixy.
void matrixy() {      
    wipe();
    assemble();
 
     int cols[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
     bubbleUnsort( cols, 16);
 
     
     for(int r = 0; r < 2; r++) {
     
     for( int i = 0; i < 16; i++) {
     
     int x = cols[i];
     int x2 = cols[i+1];
     int x3 = cols[i+2];
     int x4 = cols[i+3];
 
     for( int y = 16; y >= 0; y-- ) {
                 
            if( y < 8) {
                matrix.drawPixel(y, x, LED_ON);
                matrix.drawPixel(y, x2, LED_ON);  
                matrix.drawPixel(y, x3, LED_ON);  
                matrix.drawPixel(y, x4, LED_ON);  
            } else {
                matrix2.drawPixel(y - 8, x, LED_ON);                
                matrix2.drawPixel(y - 8, x2, LED_ON);                
                matrix2.drawPixel(y - 8, x3, LED_ON);                
                matrix2.drawPixel(y - 8, x4, LED_ON);                


            }
           matrix.writeDisplay();  
          matrix2.writeDisplay();
         delay(10);
         
         if(!bitRead( active[15 - y] , 15 - x)) {
            if( y < 8) {
                matrix.drawPixel(y, x, LED_OFF);  
            } else {
                matrix2.drawPixel(y - 8, x, LED_OFF);                
            }
          }
         
         if(!bitRead( active[15 - y] , 15 - x2)) {
            if( y < 8) {
                matrix.drawPixel(y, x2, LED_OFF);  
            } else {
                matrix2.drawPixel(y - 8, x2, LED_OFF);                
            }
          }         
         if(!bitRead( active[15 - y] , 15 - x3)) {
            if( y < 8) {
                matrix.drawPixel(y, x3, LED_OFF);  
            } else {
                matrix2.drawPixel(y - 8, x3, LED_OFF);                
            }
          }         
         if(!bitRead( active[15 - y] , 15 - x4)) {
            if( y < 8) {
                matrix.drawPixel(y, x4, LED_OFF);  
            } else {
                matrix2.drawPixel(y - 8, x4, LED_OFF);                
            }
          }
         
          
          matrix.writeDisplay();  
          matrix2.writeDisplay();  
        } 
       
   }
     }

}

void sampleAnimation () {
  
  if( aniChoice > 5) {
     aniChoice = 0; 
  }
  
  
  if (aniChoice == 0 ) {
    matrix.clear();
    matrix2.clear();
    matrix.writeDisplay();  
    matrix2.writeDisplay();         
    delay(250);
    drawDisplay();
    delay(250);
    matrix.clear();
    matrix2.clear();
    matrix.writeDisplay();  
    matrix2.writeDisplay();         
    delay(250);
    drawDisplay();     
  }
  
  if (aniChoice == 1 ) {
    fadeOut();
  }
  
  if (aniChoice == 2 ) {
    whiteOut();
  }
  
  if (aniChoice == 3 ) {
    matrix.clear();
    matrix2.clear();
    matrix.writeDisplay();  
    matrix2.writeDisplay();  
    sweep();
  } 
  
  if (aniChoice == 4 ) {
    matrixy();
  }   
   
  if (aniChoice == 5 ) {
    matrix.clear();
    matrix2.clear();
    matrix.writeDisplay();  
    matrix2.writeDisplay();         
    delay(100);
    drawDisplay(); 
    delay(100);
    matrix.clear();
    matrix2.clear();
    matrix.writeDisplay();  
    matrix2.writeDisplay();         
    delay(100);
    drawDisplay();  
    delay(100);
    matrix.clear();
    matrix2.clear();
    matrix.writeDisplay();  
    matrix2.writeDisplay();         
    delay(100);
    drawDisplay();    
  }
 
}

void doAnimation () {
   bool isRandom = false;
  
  if( aniChoice > 5) {
     aniChoice = 0; 
  }
  
  if(aniChoice == 5) {
    isRandom = true;
    int randAni[4] = {1,2,3,4};
    bubbleUnsort(randAni,4);
    aniChoice = randAni[0];  
  }
  
  if (aniChoice == 0 ) {
    drawDisplay();
  }
  
  if (aniChoice == 1 ) {
    fadeOut();
  }
  
  if (aniChoice == 2 ) {
    whiteOut();
  }
  
  if (aniChoice == 3 ) {
    sweep();
  } 
  
  if (aniChoice == 4 ) {
    matrixy();
  }   
 
  if( isRandom == true) {
    aniChoice = 5;
  }    
}

//Math!
void bubbleUnsort(int *list, int elem)
{
 for (int a=elem-1; a>0; a--)
 {
   //int r = random(a+1);
   int r = rand_range(a+1);
   if (r != a)
   {
     int temp = list[a];
     list[a] = list[r];
     list[r] = temp;
   }
 }
}

// generate a value between 0 <= x < n, thus there are n possible outputs
int rand_range(int n)
{
   int r, ul;
   ul = RAND_MAX - RAND_MAX % n;
   while ((r = random(RAND_MAX+1)) >= ul);
   return r % n;
}



void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
  asm volatile ("  jmp 0");  
} 
