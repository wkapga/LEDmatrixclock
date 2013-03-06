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
}


}



void buffer2led() {
for(int row=0;row<8;row++) {
  lc.setRow(0,row,buffer[row]);
  }

}


void loop() { 

drawtimer2buffer(7,14);
buffer2led();
}
