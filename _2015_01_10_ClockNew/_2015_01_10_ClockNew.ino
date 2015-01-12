// #define DEBUG //(un)comment this line for debug
 #define NETPLUGGED 

#ifdef DEBUG
	#define DEBUG_PRINT(x) Serial.println(x)
#else
	#define DEBUG_PRINT(x) 
#endif

//We always have to include the library
#include "LedControl.h"
#include "LEDmatrixclockfont.h"
#include <SPI.h>  
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "Time.h"
#include "OneButton.h"

/* Hardware

 pin 6 is connected to the DataIn 
 pin 5 is connected to the CLK 
 pin 4 is connected to CS/LOAD 
 We have only a single MAX72XX.
 */


// LedControl lc=LedControl(4,3,2,1); pins 2,3,4 - 1 matrix
LedControl lc=LedControl(6,5,4,3); // 3x ledmatrix

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

time_t t = 0;
int lastsec = 0;

const unsigned long seventy_years = 2208988800UL;
byte myhour = 0;
byte myminute = 0;

byte mac[] = { 0x00, 0x11, 0x95, 0x29, 0x0E, 0x92 };

byte ip[] = { 10, 0, 0, 26 };                  // ip address
byte gateway[] = { 10, 0, 0, 138 };                  // internet access via router
byte subnet[] = { 255, 0, 0, 0 };                   //subnet mask

unsigned int localPort = 8888;      // local port to listen for UDP packets

IPAddress timeServer(80,92,126,650); // 0.at.pool.ntp.org
// IPAddress timeServer(83,137,41,12); // 0.at.pool.ntp.org

EthernetClient client;

const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;


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
  
  // start Ethernet and UDP
#ifdef NETPLUGGED
  if (Ethernet.begin(mac) == 0) {
    DEBUG_PRINT("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  
  Udp.begin(localPort);
  
  getNTPtime();
#endif
}


void getNTPtime() {

  sendNTPpacket(timeServer); // send an NTP packet to a time server

  DEBUG_PRINT("sending"); 
  
  // wait to see if a reply is available
  delay(2000);  
  if ( Udp.parsePacket() ) {  
    // We've received a packet, read the data from it
    Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;    
    unsigned long epoch = secsSince1900 - 2208988800UL;
    DEBUG_PRINT(epoch); 
    
    // myhour =  (epoch  % 86400L) / 3600 ;
    // myminute =  (epoch  % 3600) / 60;

   setTime(epoch);

    DEBUG_PRINT(myhour); 
    DEBUG_PRINT(myminute); 
    
  } else {
    DEBUG_PRINT("no answer"); 
  }
}



unsigned long sendNTPpacket(IPAddress& address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp: 		   
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket(); 
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
    int schalter = digitalRead(2);
    
    if (schalter == LOW) {
        time2buffer(hour(t),minute(t));
    } else {
        time2buffer(day(t),month(t));
    }
    
    t = now();
    int currentsec = second(t);  
  
    if (lastsec !=  currentsec ) {
        if ( ((2 * lastsec) % 2) == 0 ) {
            buffer[23] = 0b00011000;
        }
        else {
             buffer[23] = 0b00000000;
        }
        lastsec = currentsec;
        
    }
  
  
  
  
  /*
  
 
    buffer[23] = 0b00011000;
  }
  */
    buffer2led2(); 

  
  


}
