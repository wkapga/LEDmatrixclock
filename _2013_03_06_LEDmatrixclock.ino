//We always have to include the library
#include "LedControl.h"
#include "LEDmatrixclockfont.h"

// #include "font.h"
/*
 Now we need a LedControl to work with.

 pin 4 is connected to the DataIn 
 pin 3 is connected to the CLK 
 pin 2 is connected to CS (=LOAD?) 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(4,3,2,1);

byte buffer[8] = {0};

void setup() {
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
}




void drawtimer2buffer(byte h, byte m ) {

if (h > 12) h -= 12;

if (h < 10) {
  for(int row=0;row<7;row++) {
    buffer[row] = font5x7[( 16 + h )*7 + row];
  }
} else {
  for(int row=0;row<7;row++) {
    buffer[row] = font5x7special[( -10 + h )*7 + row];
  }
}

buffer[7] = 0;

if ( m > 15) buffer[0] |= 1;
if ( m > 30) buffer[1] |= 1;
if ( m > 45) buffer[1] |= 2;

m = m % 15;

if ( m > 5) buffer[3] |= 1;
if ( m > 10) buffer[3] |= 2;

m = m % 5;

if ( m > 0) buffer[5] |= 1;
if ( m > 1) buffer[6] |= 1;
if ( m > 2) buffer[6] |= 2;
if ( m > 3) buffer[5] |= 2;
}



void buffer2led() {
for(int row=0;row<8;row++) {
  lc.setRow(0,row,buffer[row]);
  }

}


void loop() { 

for(int h=0;h<24;h++) {
  for(int m=0;m<60;m++) {
 
    drawtimer2buffer(h,m);
    buffer2led();
    delay(400);
  }

}


}
