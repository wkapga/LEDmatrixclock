// #define DEBUG //(un)comment this line for debug

#ifdef DEBUG
	#define DEBUG_PRINT(x) Serial.println(x)
#else
	#define DEBUG_PRINT(x) 
#endif

//We always have to include the library
#include "LedControl.h"
#include "LEDmatrixclockfont.h"
#include <SPI.h>  
#include <Time.h>
#include "OneButton.h"

/* Hardware

 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to CS/LOAD 
 We have only a single MAX72XX.
 */


// LedControl lc=LedControl(4,3,2,1); pins 2,3,4 - 1 matrix
LedControl lc=LedControl(12,11,10,3); // 3x ledmatrix

// BUtton
OneButton button(A3, true);

// buffer for 8x8 ledmatrix

uint8_t buffer[39] = {0};   // 8x5

/*
 buffer[0-7] is invisible right to the display
 buffer[8-31] is the visible part on 3 matrix
 buffer[32-39] is invisible left to the display
*/

const int scrolldelay = 60;

const unsigned long seventy_years = 2208988800UL;
byte myhour = 0;
byte myminute = 0;


void setup() {
  
   pinMode(2, INPUT_PULLUP);
 //  pinMode(3, INPUT_PULLUP);
   
   button.attachDoubleClick(doubleclick);

  
  // init all matrix
  //nr was set when  creating LedControl
  int devices=lc.getDeviceCount();
  //init  loop
  for(int address=0;address<devices;address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(address,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(address,3);
    /* and clear the display */
    lc.clearDisplay(address);
  }
}



void buffer2led(int LEDid, int buffid) {
  for(int row=0;row<8;row++) {
    lc.setRow(LEDid,row,buffer[row+(8*buffid-1)]); // write buffer to ledmatrix
  }
}


void buffer2led2() {
  for(int row=0;row<8;row++) {
/*    lc.setColumn(0,row,mirror(buffer[row+ 8])); // write buffer to ledmatrix
    lc.setColumn(1,row,mirror(buffer[row+16])); // write buffer to ledmatrix
    lc.setColumn(2,row,mirror(buffer[row+24])); // write buffer to ledmatrix */
  lc.setColumn(0,row,(buffer[15-row]));
  lc.setColumn(1,row,(buffer[23-row]));
  lc.setColumn(2,row,(buffer[31-row]));
}
}

unsigned char mirror( unsigned char a )
{
  a = ((a >> 4) & 0x0F) | ((a << 4) & 0xF0);
  a = ((a >> 2) & 0x33) | ((a << 2) & 0xCC);
  a = ((a >> 1) & 0x55) | ((a << 1) & 0xAA);

  return a;
}




void clearbuffer( int buffid) {
  for(int row=0;row<8;row++) {
    buffer[row+(8*buffid)] = 0;
  }
}

void clearbufferall() {
  for (int i=0;i<5;i++){
    clearbuffer(i);
  }
}  



void ascii2buffer(int ascii, int buffid){
    if  ( (ascii >= 0x20) && (ascii <= 0x7f) ) {
        for(int row=0;row<7;row++) {
            buffer[row+(buffid* 8)] = font5x7[( ascii - 0x20 )*7 + row];
        }
    }
}

void ascii2bufferand(int ascii, int buffid){
    if  ( (ascii >= 0x20) && (ascii <= 0x7f) ) {
        for(int row=0;row<7;row++) {
            buffer[row+(buffid* 8)] = buffer[row+(buffid* 8)] | font5x7[( ascii - 0x20 )*7 + row];
        }
    }
}


void time2buffer(int h, int m){
    clearbufferall();
    for(int row=0;row<7;row++) {
        buffer[row+8]= font5x7[( (m % 10) + 0x10 )*7 + row] >> 3;
        buffer[row+8]= buffer[row+8] | font5x7[( m/10 + 0x10 )*7 + row] << 3;
        buffer[row+16]=  font5x7[( m/10 + 0x10 )*7 + row] >> 5;
        buffer[row+16]=  buffer[row+16] | font5x7[( (h % 10) + 0x10 )*7 + row] << 2 ;
        buffer[row+24]= font5x7[( (h % 10) + 0x10 )*7 + row] >> 6;
        if (h > 9) { 
            buffer[row+24]=  buffer[row+24] | font5x7[( (h / 10) + 0x10 )*7 + row] ;
        }
  }
    
    
    
}


void doubleclick() {
 buffer[23] = 0b10000001; 

}  



void loop() { 
  button.tick();
  
  time2buffer(16,39);
  
  
  int schalter = digitalRead(2);
  if (schalter == LOW) {
    buffer[23] = 0b00011000;
  }

    buffer2led2(); 

  
  


}
