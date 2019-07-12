#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include<SPI.h>
/******TO BE MODIFIED BY USER******/

const char* ssid = "belkin.32af";      //Enter the ssid of your WIFI Network
const char* password = "4eb9c6e4";  //Enter the password of your network

/*********************************/
#define packetlen 250
    long Start_Time = 0;
long Stop_Time = 0;
long Response_Time = 0;
char Receive_Slave[4100];
char j = 0 , Num =0;
int i;
char flag;
  int lendata;
int spi_rw_test();
WiFiUDP Udp;
unsigned int localUdpPort = 4080;  // local port to listen on
char incomingPacket[512];  // buffer for incoming packets
char  replyPacket[] = "E";  // a reply string to send back

char buff[]="Hello Slave\n";

void setup()
{                                    //configure the line as output
  Serial.begin(115200);              //Set UART baud rate                                 //Initialize the CS line to HIGH
  
  Serial.println();
  pinMode(D8,OUTPUT);
  
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
 
    SPI.begin();  /* begin SPI */
    SPI.setFrequency(20000000);
   digitalWrite(D8,LOW);
   digitalWrite(D4,LOW);
    delay(10);
    digitalWrite(D4,HIGH);
    digitalWrite(D3,LOW);
    SPI.transfer(0x1c);
    SPI.transfer(0x1c);
    SPI.transfer(0x1c);
    digitalWrite(D3,HIGH);
   digitalWrite(D8,HIGH);
   if(spi_rw_test())
   Serial.println("Test Successful");
   else
   Serial.println("Test failed");

}


void loop()
{


  digitalWrite(D8,LOW);
  j=0;
  Num=0;
       
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    //delay(100);
   Serial.printf("\nReceived %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());

    
    int len = Udp.read(incomingPacket, 512);
   if (len > 0)
   {
     incomingPacket[len] = 0;
   }
  for(int j = 0; j < len ;j++)
  Serial.print(incomingPacket[j],HEX);
    for(i = 0; i < 4000 ; i++)
      Receive_Slave[i] = 0;
    
    
    if( strlen(incomingPacket) == 7)
     {
     
     for(i = 2 ; i<=5 ; i++)
        incomingPacket[i]--;
      lendata = (incomingPacket[4]<<8) | incomingPacket[5];
      Serial.println("Read Mode");
     Serial.println(lendata);
     //digitalWrite(D8,LOW);
      for( int i=0 ; i<7; i++)
      {
        SPI.transfer(incomingPacket[i]);
        Serial.print(incomingPacket[i],HEX);
      }
      //Serial.println("\nSent data");
      for( int i=0 ; i<lendata; i++)
      { 
        //Serial.println(incomingPacket2[i],HEX);
        j = (SPI.transfer('d'));
        Receive_Slave[i] = j;
        //Serial.print(Receive_Slave[i],HEX);
       
      }

      SPI.transfer('\n');
        //digitalWrite(D8,HIGH);
      
       
      // send back a reply, to the IP address and port we got the packet from
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write(Receive_Slave);
      Udp.endPacket();
      for(i = 0; i<strlen(Receive_Slave) ; i++)
      Receive_Slave[i] = 0;
    }
    else
    {
      //digitalWrite(D8,LOW);
     // Serial.println("\nReceived data");
     // digitalWrite(D8,LOW);
      incomingPacket[2]--;
      incomingPacket[3]--;
      Start_Time = micros();
      for( int i=0 ; i< len; i++)
      {
      Serial.print('\n');
        SPI.transfer(incomingPacket[i]);
        //for( int i=4 ; i< 255; i++)
       Serial.print(incomingPacket[i],HEX);
      }
      SPI.transfer('\n');
      Stop_Time = micros();
      flag=1;
     // digitalWrite(D8,HIGH);
     // Response_Time = Start_Time - Stop_Time;
      // send back a reply, to the IP address and port we got the packet from
       // send back a reply, to the IP address and port we got the packet from*/
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write(replyPacket);
      Udp.endPacket();
    }
    if(flag)
    {
  Response_Time = Stop_Time - Start_Time;
 // Serial.print("Write Time is: ");
  //Serial.println(Response_Time);
   flag = 0;
    }
    
   
  }
 
}


int spi_rw_test()
{
  unsigned char testPacket[packetlen+5] = {0};
 unsigned char testPacket2[7] = {0x19,0x1e,0x00,0x00,0x00,0xFA,0x1c};
 unsigned char Receive_Slave[packetlen] = {0};
 unsigned char temp[packetlen] = {0};
char randomnumber;
int ecount=0;
int scount=0;
int flag=0;
int Num;
int j = 0;
int count;
int i;
long startTime = 0;
long stopTime = 0;
long midTime = 0;
long writeTime[100] = {0};
long avgTime = 0;
  for(count = 0 ; count < 100 ; count++)
    { 
 
testPacket[0] = 0x19;
testPacket[1] = 0x1e;
testPacket[2] = 0x00;
testPacket[3] = 0x01;
for(i = 0 ; i < packetlen ; i++)
{
    randomnumber = random(48,57);
  
  testPacket[i+4] = randomnumber;
}    
    testPacket[packetlen+4] = 0x1c;
for(i=0;i<packetlen;i++)
   temp[i] = testPacket[i+4];
  
  //  Serial.println("test packet");
//for( i = 0 ; i < 1005; i++)


    //Serial.print(testPacket[i],HEX);
    //Serial.println("");
    //delay(5000);
    

      digitalWrite(D8,LOW);
      startTime = micros();
      for( int i=0 ; i<packetlen+5; i++)
      {
       //Serial.print("Sent data: ");
       //Serial.print(testPacket[i],HEX);
       //Serial.print("Received data: ");
       SPI.transfer(testPacket[i]);
       //delay(1000);
      }
      SPI.transfer('\n');
      midTime = micros();
      for( int i=0 ; i<7; i++)
      { 
       //Serial.print("Sent data: ");
       //Serial.print(testPacket2[i],HEX);
      // Serial.print("Received data: ");
       SPI.transfer(testPacket2[i]);
     //  delay(1000);
       
      }

      for( int i=0 ; i<packetlen; i++)
      { 
        //Serial.println(testPacket2[i],HEX);
        Receive_Slave[i] = (SPI.transfer('d'));
       // Serial.println(Receive_Slave[i],HEX);
       // delay(1000);
       
      }
      SPI.transfer('\n');
      stopTime=micros();
      digitalWrite(D8,HIGH);
      //Serial.println("Received data");
      //for(int i=0;i<1000;i++)
      //Serial.print(Receive_Slave[i],HEX);
      //Serial.println('\n');
      //delay(5000);
      for(i = 0 ;i<packetlen ; i++)
      {
        if(Receive_Slave[i] != temp[i])
        {
        flag = 1;
      //  Serial.println("");
     //   Serial.println(Receive_Slave[i]);
        }
        else
        flag = 0;
      }
      if(flag)
      {
      Serial.println("error");
      ecount++;
      flag=0;
      }
      else
      {
      //Serial.print("Success    ");
      //Serial.println("");
      scount++;
      }
      writeTime[j] = midTime-startTime;
      j++;
      midTime = 0;
      startTime  = 0;
    }
       

    Serial.print("\nSuccess count: ");
    Serial.println(scount);
    Serial.print("Time for write: ");
    for(j = 0; j<100 ; j++)
    avgTime=avgTime+writeTime[j];
    avgTime = avgTime/100;
    Serial.print(avgTime);
    Serial.println("uS");
    for(j = 0; j<100 ; j++)
    writeTime[j] = 0;
    j=0;
    if(scount==100)
    return 1;
    else
    return 0;
}
