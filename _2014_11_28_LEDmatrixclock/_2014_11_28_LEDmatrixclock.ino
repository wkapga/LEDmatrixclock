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

/* Hardware

 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to CS/LOAD 
 We have only a single MAX72XX.
 */


// LedControl lc=LedControl(4,3,2,1); pins 2,3,4 - 1 matrix
LedControl lc=LedControl(12,11,10,3); // 3x ledmatrix

// buffer for 8x8 ledmatrix

uint8_t buffer[39] = {0};   // 8x5

/*
 buffer[][0-7] is invisible right to the display
 buffer[][8-31] is the visible part on 3 matrix
 buffer[][32-39] is invisible left to the display
*/

const int scrolldelay = 60;

void setup() {
  // init all matrix
  //nr was set when  creating LedControl
  int devices=lc.getDeviceCount();
  //init  loop
  for(int address=0;address<devices;address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(address,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(address,8);
    /* and clear the display */
    lc.clearDisplay(address);
  }
}



void buffer2led(int LEDid, int buffid) {
  for(int row=0;row<8;row++) {
    lc.setRow(LEDid,row,buffer[row+(8*buffid-1)]); // write buffer to ledmatrix
  }
}
/*
void shiftbuffer() {

  for(int row=0;row<8;row++) {
    buffer[row] = buffer[row]<< 1; //shift buffer one to left
    buffer[row] = buffer[row] |  ( bufferright[row] >> 7 ); // insert leftmost pixel of bufferrright , make sure via unsingned that minus-sign is not carried over
    bufferright[row] <<= 1; //shift rightbuffer to left
  }
}*/

/*
void shiftbufferleft(int bufffrom, int buffto) {
//for each row
  for(int row=0;row<8;row++) {
    //scroll "to" (left) buffer one pixel to the left
    buffer[row][buffto] =  buffer[row][buffto] << 1;
    // copy highest (leftmost) pixel over   
    buffer[row][buffto] =  buffer[row][buffto] | ( buffer[row][bufffrom] >> 7 );
  }
}
*/
/*
void shiftdisplayleft(int nrofbuffer) { //3 matrix = 5 buffer
// shift very left (3) buffer to invisible (4)
/  continue till very right (1) which gets data from invisble (0) 

  for(int i=nrofbuffer-1;i=1;i--) { 
    shiftbufferleft(i-1,i);
  }
}
*/

void clearbuffer( int nrofbuffer) {
  for (int buffid=0;buffid<nrofbuffer;buffid++) {
    for(int row=0;row<8;row++) {
      buffer[row+(8*buffid-1)] = 0;
    }
  }
}
/*
void loadchar2bufferright (int ascii) {
	if  ( (ascii >= 0x20) && (ascii <= 0x7f) ) {
		for(int row=0;row<7;row++) {
			buffer[row] = font5x7[( ascii - 0x20 )*7 + row]; 
			//ascii conversion
			}
		}
	}


void scrollastring(char  message[] ){

//clear buffer and screen
 clearbuffer();
 buffer2led();

for(int i=0;message[i]!=0;i++){
 
	DEBUG_PRINT(message[i]); 
  //load character in right buffer
	loadchar2bufferright(message[i]);
    // shift left and display 5 times (for 5x7font)
	for (int z=1; z<7 ; z++){ 
		shiftbuffer();
		buffer2led();
		delay(scrolldelay);
	}  
}

}*/


void loop() { 
// This is the main loop

// test
/*clearbuffer(5);
buffer[22]= 19; // 1+2+16.. 1st, 2nd, and 4th dot in 3rd line
buffer2led(2,1);
*/

lc.setLed(0,0,0,true);
lc.setLed(1,0,0,true);
lc.setLed(2,0,0,true);
	
    delay(3000);

 // shiftdisplayleft(5);


}
