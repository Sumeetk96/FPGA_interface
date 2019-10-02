#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include<SPI.h>
/******TO BE MODIFIED BY USER******/

const char* ssid = "hotspot";      //Enter the ssid of your WIFI Network
const char* password = "passwerd";  //Enter the password of your network

/*********************************/
#define packetlen 250
long Start_Time = 0;
long Stop_Time = 0;
long Response_Time = 0;
char Receive_Slave[1500];
char j = 0 , Num =0;
int i;
char flag;
int lendata;
int spi_rw_test();
WiFiUDP Udp;
unsigned int localUdpPort = 4080;  // local port to listen on
char incomingPacket[1500];  // buffer for incoming packets
char  replyPacketSuccess[] = "E";  // a reply string to send back
char  replyPacket_checksum_error[] = "Checksum mismatch. Please resend"; 


void setup()
{ 

	Serial.begin(115200); 
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
	j=0;
	Num=0;
	int received_checksum = 0 , local_checksum = 0;   
	int packetSize = Udp.parsePacket();
	if (packetSize)
	{
		// receive incoming UDP packets
		//Serial.printf("\nReceived %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());

		int len = Udp.read(incomingPacket,1472);
		if (len > 0)
			incomingPacket[len] = 0;
		
		//for(int j = 0; j < len ;j++)
		//Serial.print(incomingPacket[j]);
    //Serial.print(" ");
		received_checksum = incomingPacket[0];
		received_checksum = (received_checksum<<8) | incomingPacket[1];

		for( i = 2 ; i < len ; i++)
		local_checksum = local_checksum + incomingPacket[i];
		local_checksum &= 0xFFFF;

		//Serial.printf("\nlocal checksum is %d and received checksum is %d",local_checksum ,received_checksum);


		if(local_checksum == received_checksum)
		{
			local_checksum = 0;
			if( packetSize == 8)
			{

				lendata = (incomingPacket[4]<<8) | incomingPacket[5];
				//Serial.println("Read Mode");
				//Serial.println(lendata);
				
				digitalWrite(D8,LOW);
				for( int i=2 ; i<8; i++)
				{
					SPI.transfer(incomingPacket[i]);
					//Serial.print(incomingPacket[i],HEX);
				}
				SPI.transfer('\0');
				//Serial.println("\nSent data");
				for( int i=0 ; i<lendata; i++)
				{ 
					//Serial.println(incomingPacket2[i],HEX);
					j = (SPI.transfer('d'));
					Receive_Slave[i] = j;
					//Serial.print(Receive_Slave[i],HEX);
				}

				SPI.transfer('\n');
				digitalWrite(D8,HIGH);
				// send back a reply, to the IP address and port we got the packet from
				Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
				Udp.write(Receive_Slave,lendata);
				Udp.endPacket();
				for(i = 0; i < lendata ; i++)
					Receive_Slave[i] = 0;
			}
			else
			{
				digitalWrite(D8,LOW);
				for( int i=2 ; i< len; i++)
				{
					SPI.transfer(incomingPacket[i]);
				}
				SPI.transfer('\n');
				flag=1;
				digitalWrite(D8,HIGH);

				// send back a reply, to the IP address and port we got the packet from
				Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
				Udp.write(replyPacketSuccess);
				Udp.endPacket();
			}
		}
		else
		{
			Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
			Udp.write(replyPacket_checksum_error);
			Udp.endPacket();
		}


	}

}


int spi_rw_test()
{
	unsigned char testPacket2[7] = {0x19,0x1e,0x00,0xFA,0x00,0x06};
	unsigned char Receive_Slave[250] = {0};
	unsigned char temp[packetlen] = {0};
	unsigned char testPacket[256] = {0};
	char randomnumber;
	int ecount=0;
	int scount=0;
	int flag=0;
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
		testPacket[3] = 0xFA;
		testPacket[4] = 0x00;
		testPacket[5] = 0x07;
		for(i = 0 ; i < 250 ; i++)
		{
			randomnumber = random(48,57);
			testPacket[i+6] = randomnumber;
		}    
		for(i=0 ; i < 250 ; i++)
			temp[i] = testPacket[i+6];

		digitalWrite(D8,LOW);
		startTime = micros();

		for( i = 0 ; i < 256 ; i++)
			SPI.transfer(testPacket[i]);
		SPI.transfer('\n');
		midTime = micros();
		for( i = 0 ; i < 7 ; i++)
			SPI.transfer(testPacket2[i]);
		for( i = 0 ; i < 250 ; i++)
			Receive_Slave[i] = (SPI.transfer('d'));
		SPI.transfer('\n');
		stopTime=micros();
		digitalWrite(D8,HIGH);
		for(i = 0 ; i < 250 ; i++)
		{
			if(Receive_Slave[i] != temp[i])
				flag = 1;
			else
				flag = 0;
		}
		if(flag)
		{
			ecount++;
			flag=0;
		}
		else
			scount++;
		writeTime[j] = midTime-startTime;
		j++;
		midTime = 0;
		startTime  = 0;
	}


	Serial.print("\nSuccess count: ");
	Serial.println(scount);
	Serial.print("Error Count: ");
	Serial.println(ecount);
	Serial.print("Time for write: ");
	for(j = 0; j < 100 ; j ++)
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
