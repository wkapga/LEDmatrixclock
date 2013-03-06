//We always have to include the library
#include "LedControl.h"
#include "LEDmatrixclockfont.h"
#include <SPI.h>         
#include <Ethernet.h>
#include <EthernetUdp.h>


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

byte mac[] = { 0x00, 0x11, 0x95, 0x29, 0x0E, 0x92 };

byte ip[] = { 10, 0, 0, 40 };                  // ip address
byte gateway[] = { 10, 0, 0, 138 };                  // internet access via router
byte subnet[] = { 255, 0, 0, 0 };                   //subnet mask

unsigned int localPort = 8888;      // local port to listen for UDP packets

IPAddress timeServer(91,118,20,250); // 0.at.pool.ntp.org



// IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov NTP server
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov NTP server
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov NTP server

const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets 

// A UDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

const unsigned long seventy_years = 2208988800UL;
byte myhour = 0;
byte myminute = 0;


void setup() {
  
 Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  } 
  
  
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
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  

  Udp.begin(localPort);
  
  

}



void getNTPtime() {

  sendNTPpacket(timeServer); // send an NTP packet to a time server
 Serial.println("sending"); 
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
    Serial.println(epoch); 
    
    myhour =  (epoch  % 86400L) / 3600 ;
    myminute =  (epoch  % 3600) / 60;

   Serial.println(myhour); 
    Serial.println(myminute); 
    

    /*drawtimer2buffer(5,27);
    buffer2led();*/

  } else {
    Serial.println("no answer"); 
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

if ( m > 14) buffer[0] |= 1;
if ( m > 29) buffer[1] |= 1;
if ( m > 44) buffer[1] |= 2;

m = m % 15;

if ( m > 4) buffer[3] |= 1;
if ( m > 9) buffer[3] |= 2;

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

/* for(int h=0;h<24;h++) {
  for(int m=0;m<60;m++) {
 
    drawtimer2buffer(h,m);
    buffer2led();
    delay(400);
  }

} */

  getNTPtime();


    drawtimer2buffer(myhour,myminute);
    buffer2led();

delay(10000);
  myminute++;
  Serial.println("toc.");

}
