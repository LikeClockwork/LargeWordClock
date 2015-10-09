/*************************************************** 
  This is a library for our I2C LED Backpacks

  Designed specifically to work with the Adafruit 16x8 LED Matrix backpacks 
  ----> http://www.adafruit.com/products/2035
  ----> http://www.adafruit.com/products/2036
  ----> http://www.adafruit.com/products/2037
  ----> http://www.adafruit.com/products/2038
  ----> http://www.adafruit.com/products/2039
  ----> http://www.adafruit.com/products/2040
  ----> http://www.adafruit.com/products/2041
  ----> http://www.adafruit.com/products/2042
  ----> http://www.adafruit.com/products/2043
  ----> http://www.adafruit.com/products/2044
  ----> http://www.adafruit.com/products/2052

  These displays use I2C to communicate, 2 pins are required to 
  interface. There are multiple selectable I2C addresses. For backpacks
  with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
  with 3 Address Select pins: 0x70 thru 0x77

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include "RTClib.h"

#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

RTC_DS1307 RTC;
Adafruit_8x16matrix matrix = Adafruit_8x16matrix();
Adafruit_8x16matrix matrix2 = Adafruit_8x16matrix();

void setup() {
  Serial.begin(9600);
  Serial.println("16x8 LED Matrix Test");

#include "RTClib.h"  
  matrix.begin(0x70);  // pass in the address
  matrix2.begin(0x71);  // pass in the address
  

  
  matrix.clear();      // clear display

       matrix.setBrightness(15); 
        matrix.drawPixel(7, 0, LED_ON);  //C0, A0 
        matrix.drawPixel(6, 0, LED_ON);  //C0, A0 

        matrix.writeDisplay(); 


}

static const uint8_t PROGMEM
  smile_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
  neutral_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10111101,
    B10000001,
    B01000010,
    B00111100 },
  frown_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B01000010,
    B00111100 };

void loop() {
  

 
  // write the changes we just made to the display




  for( int j = 7; j > -1; j--) {
  for(int i = 16; i > -1; i--) {
      Serial.print(j);
      Serial.print(i);
      matrix.drawPixel(j, i, LED_ON);  //C0, A0 
      matrix2.drawPixel(j, i, LED_ON);  //C0, A0 
     matrix2.writeDisplay(); 

        matrix.writeDisplay();
      //delay(10);
      //matrix.clear();      
    }
  }  
  
     matrix2.writeDisplay(); 

        matrix.writeDisplay();
}


  
 


