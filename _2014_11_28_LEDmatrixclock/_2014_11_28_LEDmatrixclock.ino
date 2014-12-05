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

 pin 4 is connected to the DataIn 
 pin 3 is connected to the CLK 
 pin 2 is connected to CS/LOAD 
 We have only a single MAX72XX.
 */


// LedControl lc=LedControl(4,3,2,1); pins 2,3,4 - 1 matrix
LedControl lc=LedControl(4,3,2,3); // 3x ledmatrix

// 5 buffers for 3 pcs 8x8 ledmatrix
// 8 bytes for each line
uint8_t buffer[8][5] = {0};

/*
 buffer[][0] is invisible right to the display
 buffer[][1:3] is the visible part on 3 matrix
 buffer[][4] is invisible left to the display
LED:    x 2 1 0 x
Buffer: 4 3 2 1 0
*/

const int scrolldelay = 60;

void setup() {
  // init all matrix
  // nr was set when  creating LedControl
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

void loop() { 
// This is the main loop

// test
	clearbuffer(5);
	buffer[2][1]= 19; // 1+2+16.. 1st, 2nd, and 4th dot in 3rd line
	buffer2led(2,1);

    delay(3000);

	shiftdisplayleft(5);
}

void buffer2led(int LEDid, int buffid) {
// write buffer[][buffid] to ledmatrix
// LEDid is 0(right) to 2(left)
  for(int row=0;row<8;row++) {
    lc.setRow(LEDid,row,buffer[row][buffid]); 
  }
}

void shiftdisplayleft(int nrofbuffer) { //3 matrix = 5 buffer
/* shift very left (3) buffer to invisible (4)
  continue till very right (1) which gets data from invisble (0) 
*/
  for(int i=nrofbuffer-1;i=1;i--) { 
    shiftbufferleft(i-1,i);
  }
}

void shiftbufferleft(int bufffrom, int buffto) {
/* scroll by 1 pixel: shift "to" 1 pixel left, 
copy leftmost pixel of "from" over as rightmost */
  for(int row=0;row<8;row++) { //for each row
    //scroll "to" (left) buffer one pixel to the left
    buffer[row][buffto] =  buffer[row][buffto] << 1;
    // copy highest (leftmost) pixel over   
    buffer[row][buffto] =  buffer[row][buffto] | ( buffer[row][bufffrom] >> 7 );
  }
}

void clearbuffer( int nrofbuffer) {
// write zeros to all rows of all buffers 
  for (int buffid=0;buffid<nrofbuffer;buffid++) {
    for(int row=0;row<8;row++) {
      buffer[row][buffid] = 0;
    }
  }
}

void loadchar2buffer (int ascii, int buffid) {
// copy row by row of select char to buffer (normally buffer 0)
	if  ( (ascii >= 0x20) && (ascii <= 0x7f) ) {
		for(int row=0;row<7;row++) {
			buffer[row][buffid] = font5x7[( ascii - 0x20 )*7 + row]; 
			//ascii conversion
		}
	}
}

/*
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


