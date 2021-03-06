
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
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Time.h>

// #include "font.h"
/*
 Now we need a LedControl to work with.

 pin 4 is connected to the DataIn 
 pin 3 is connected to the CLK 
 pin 2 is connected to CS/LOAD 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(6,5,4,1);

// buffer for 8x8 ledmatrix
byte buffer[8] = {0};

byte bufferright[8] = {0};

const int scrolldelay = 60;

// use your own mac
byte mac[] = { 0x00, 0x11, 0x95, 0x29, 0x0E, 0x92 };

byte ip[] = { 10, 0, 0, 40 };                  // ip address
byte gateway[] = { 10, 0, 0, 138 };                  // internet access via router
byte subnet[] = { 255, 0, 0, 0 };                   //subnet mask

unsigned int localPort = 8888;      // local port to listen for UDP packets

IPAddress timeServer(91,118,20,250); // 0.at.pool.ntp.org
// IPAddress timeServer(83,137,41,12); // 0.at.pool.ntp.org

EthernetClient client;

const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

const unsigned long seventy_years = 2208988800UL;
byte myhour = 0;
byte myminute = 0;


void setup() {

#ifdef DEBUG
 Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  } 
#endif
  
  //Ethernet.begin(mac, ip, gateway, subnet);
  // delay(1000);
  
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
  

  // start Ethernet and UDP
  if (Ethernet.begin(mac) == 0) {
    DEBUG_PRINT("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  
  Udp.begin(localPort);
  
  getNTPtime();
  
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

void fetchwebdata() {

byte server[] = {10,0,0,21};
	if (client.connect(server,80)) {
	client.println("GET /GRAFIK.htm");
	client.println();
	} else {
	DEBUG_PRINT( "web connecction failed");
	}


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

buffer[7] = 0; // make sure puffer is clean in last row

if ( m > 14) buffer[0] |= 1; //for 1.quarter hour set last led in 1st row
if ( m > 29) buffer[1] |= 1;
if ( m > 44) buffer[1] |= 2;

m = m % 15;

if ( m > 4) buffer[3] |= 1;  // 5mins in 4th row
if ( m > 9) buffer[3] |= 2;

m = m % 5;

if ( m > 0) buffer[5] |= 1; // mins 1to4 in 6th and 7th row
if ( m > 1) buffer[6] |= 1;
if ( m > 2) buffer[6] |= 2;
if ( m > 3) buffer[5] |= 2;

buffer2led();
}

void buffer2led() {
  for(int row=0;row<8;row++) {
    lc.setRow(0,row,buffer[row]); // write buffer to ledmatrix
  }
}

void shiftbuffer() {

  for(int row=0;row<8;row++) {
    buffer[row] = buffer[row]<< 1; //shift buffer one to left
    buffer[row] = buffer[row] |  ( bufferright[row] >> 7 ); // insert leftmost pixel of bufferrright , make sure via unsingned that minus-sign is not carried over
    bufferright[row] <<= 1; //shift rightbuffer to left
  }
}

void clearbuffer() {
  for(int row=0;row<8;row++) {
    buffer[row] = 0;
  }
}

void loadchar2bufferright (int ascii) {
	if  ( (ascii >= 0x20) && (ascii <= 0x7f) ) {
		for(int row=0;row<7;row++) {
			bufferright[row] = font5x7[( ascii - 0x20 )*7 + row]; //ascii conversion
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

}

void loopthedayfast() {
  for(int h=0;h<24;h++) { //for testing loop through all minutes
    for(int m=0;m<60;m++) {
 
    drawtimer2buffer(h,m);
    buffer2led();
    delay(400);
    }
  } 
}

int adjusttimetoCET(time_t t) {

int hours;

int m = month(t);
int y = year(t);
int d = day(t);

// EU summertime rules
int marchday = 31 - ((5*y/4 +4) % 7);
int octoberday = 31 - ((5*y/4 +1) % 7);

if ( (m < 3) || (m > 10) ||   // nov-feb is winter
	( (m ==3) && ( d < marchday) ) ||  // so is before last sunday in march
	( (m == 10) && (d >= octoberday) ) ) { // and also from last sunday in october
		hours = 1; // hardcoded for CE(S)T
	} else {
		hours = 2; // summertime!
}

return hours;
}


void scrolltime() {
	char msg[8];
	
	time_t t = now();
	t = t + 3600 * adjusttimetoCET(t);
	
	sprintf(msg,"%d:%02d  ",hour(t),minute(t));
	scrollastring( msg  );
}


void scrolldate() {
	char msg[8];
	const char dayname[7][3]={"SO","MO","DI","MI","DO","FR","SA"};
	time_t t = now();
	t = t + 3600 * adjusttimetoCET(t);
	
	sprintf(msg,"%s.%d.%d. ",dayname[weekday(t)-1],day(t),month(t));
	scrollastring( msg  );
}



void loop() { 
	time_t t = now();
	if  (((minute(t) % 5) == 1 ) && (second(t) == 42 ) )  { //every 5 minutes
		fetchwebdata();
	}
	if ((hour(t) == 1) && (minute(t) == 21 ) && (second(t) == 42 ) ) { 
		getNTPtime(); // daily 01:21:42
	} 
	
	scrolltime();
	scrolldate();

 //   loopthedayfast();
	t = now();
	t = t + 3600 * adjusttimetoCET(t);
    drawtimer2buffer(hour(t),minute(t));
    delay(3000);
}
