#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

///////// TO BE EDITED BY USER ///////////

#define SERVER "192.168.43.100"

/////////////////////////////////////////

#define PORT 4080
#define StartByte1 0x19
#define StartByte2 0x1E

int frame_transfer(char ,unsigned int ,unsigned int ,unsigned char* , unsigned char* );
int sock_init(void);
int data_integrity(unsigned int ,unsigned int);

    // user sets timeout
long set_sec = 0;
long set_usec = 0;

int sockfd;
struct sockaddr_in esp;
int slen = sizeof(esp);

// for select()
int ready;
struct timeval timeout;
fd_set esp_readset;

// for timing
struct timeval tv_start_time, tv_end_time, tv_duration;
int _send = 0;
int sent = 0;
int job_count = 1;
int jobs_completed = 0;
int i = 0;
int Sentflag = 0;
int transmits = 0;
int retransmits = 0;



int main(void)
{
	
	int status = 0;
	int choice = 0;
	unsigned int write_address = 0;
	unsigned int read_address = 0;
	unsigned int length = 0;
	unsigned int data_int_packetsize = 0;
	unsigned int data_int_instances = 0;
	char temp[1400] = {0} ;
	unsigned char *write_message = (unsigned char*) malloc(255);
	unsigned char *read_message = (unsigned char*) malloc(255);
	char buf[10];	
	status = sock_init();
	if(!status)		
		return 0;

    	printf("Enter timeout for select (seconds): ");
    	scanf("%ld", &set_sec);
    	printf("Enter timeout for select (microseconds): ");
    	scanf("%ld", &set_usec);
       	while(1)
	{	
		printf("\n1. Write data	2.Read data	3.Data Integrity	4.Quit\n");
		scanf("%d",&choice);

		switch(choice)
		{
			case 1:
			{
				printf("Enter address to write data (0 to 4095): ");
				scanf("%d" ,&write_address);
				getchar();
				printf("Enter Data you want to write: ");
				fgets(write_message, 255, stdin);
				printf("length is %d address is %d , message is %s ",strlen(write_message),write_address,write_message);
				frame_transfer('w' , strlen(write_message), write_address , write_message , NULL);
				break;
				
			}

			case 2:
			{
				printf("Enter address to read data from(0 to 4095): ");
				scanf("%d" ,&read_address);	
				printf("Enter the number of Bytes you want to read: ");
				scanf("%d",&length);
				printf("No of bytes: %d 	Address: %d",length ,read_address);
				frame_transfer('r' , length , read_address , NULL , read_message);
				printf("\nReceived data is: \n"); 
				for(i = 0 ; i < length ; i ++)
					printf("%c" ,read_message[i]);
				break;

				
			}

			case 3:
			{
				printf("Enter the packet size for data integrity test: ");
				scanf("%d", &data_int_packetsize);
				printf("Enter the number of instances to run: ");
				scanf("%d", &data_int_instances);
				data_integrity(data_int_packetsize , data_int_instances);
				break;
			}

			case 4:
			{
				return 0;
			}

			default:
			{
				break;
			}
		}	
	}

//	printf("packets sent: %d 	packets retansmitted: %d\n",transmits , retransmits);
    return 0;
}


int frame_transfer(char readwrite , unsigned int length , unsigned int address ,unsigned  char *write_data ,unsigned char *read_data )
{

	//Frame Structure:
	//StartByte1 | StartByte2 | ByteCountMSB | ByteCountLSB | AddressMSB | AddressLSB + r/w bit (1 for write, 0 for read) |	Data(if write) |
	unsigned int i,framelen,checksum = 0;
	unsigned char checksum_MSB, checksum_LSB;
	unsigned char *frame = (unsigned char*) malloc(length+6);
	unsigned char *receiveframe = (unsigned char*) malloc(length);	
	frame[0] = 0;
	frame[1] = 0;
	frame[2] = StartByte1;
	frame[3] = StartByte2;	
	frame[4] = (length >> 8);            
	frame[5] = length;
	frame[6] = (address >> 7);
    
    	if(readwrite == 'r')
	{
			
		frame[7] = (address << 1) & 0xFE;
		for( i  = 0 ; i < 8 ; i++)
		{
			checksum = checksum + frame[i];
		}	
		frame[0] = checksum >> 8;
		frame[1] = checksum;
	
	}	
	else if (readwrite == 'w')
	{
		frame[7] = (address << 1) | 0x01;
		
		for(i = 8; i < length + 8 ; i++)
			frame[i] = *write_data++;
		checksum = 0;			
		for( i  = 2 ; i < length + 8 ; i++)
		{
			checksum = checksum + frame[i];
		}
		frame[0] = checksum >> 8;
		frame[1] = checksum;
	}
	else 
		return -1;
		
	Sentflag = 0;
	while(!Sentflag)
	{
	//Send Write or Read Frame*******************
		if(readwrite == 'r')
    			framelen = 8;
    		else
    			framelen = length + 8;
    //		printf("Constructed frame is : ");
//		for(i = 0 ; i < 8 ; i++)
//			printf("%x ", frame[i]);	
    		sendto(sockfd, frame, framelen, 0, (struct sockaddr*) &esp, slen);
    		checksum = 0;
    	//	printf("Sent frame(HEX) is: ");
    	//	for(i = 0 ; i < framelen ; i++)
	//			printf("%x",frame[i]);
	//		printf("\n");
		transmits += 1;

		// resetting the timeout for select()
		timeout.tv_sec = set_sec;
		timeout.tv_usec = set_usec;
		// resetting the FD set for select()
		FD_ZERO(&esp_readset);
		FD_SET(sockfd, &esp_readset);

		ready = select((sockfd+1), &esp_readset, NULL, NULL, &timeout);

		if(ready < 0) 
		{
			perror("select");
		}
	       	else if(ready == 1) 
		{
			//Receive requested data/Ack**************
			if(readwrite == 'w')
				framelen = 1;
			else
				framelen = length;
			recvfrom(sockfd, receiveframe, framelen, 0, (struct sockaddr*) &esp, &slen);
			if(readwrite =='w')
			{
	//			printf("%c\n",receiveframe[0]);
				sent += 1;				
				if(receiveframe[0] == 'E')
				{
					Sentflag = 1;
					return length;
				}
				else if(receiveframe[0] == 'C')
				{
					Sentflag = 0;
					printf("Checksum Error");
					break;
				}
				else
				{
					return -1;
				}
			}

			else
			{
				for(i = 0 ; i < length ; i++)
				{
					*read_data++ = receiveframe[i];
	//				printf("%c" , receiveframe[i]);
				}	
	//			printf("\n");
				sent += 1;
				Sentflag = 1;
				return length;
			}	
			
			
		 } 
		else
	       	{ 
			// in this case, we would simply retransmit the packet
			// for which we haven't received an acknowledgement or data
			retransmits += 1;
			printf("\nTIMEOUT\n");
		}
	
		printf("\nloop over\n");
	}
}

int sock_init()
{
			
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
       	{
  		perror("socket");
        	return 0;
   	 }

	esp.sin_family = AF_INET;
	esp.sin_port = htons(PORT);
	if(inet_aton(SERVER, &esp.sin_addr) == 0) {
        fprintf(stderr, "inet_aton() error\n");
		return 0;
    	}

}

int data_integrity(unsigned int packetsize,unsigned  int iterations)
{
	int i = 0;	
	int current_iteration = 0;
	int success_count = 0;
	int fail_count = 0;
	int mismatch;
	unsigned char *randomdata = (unsigned char*) malloc(packetsize);
	unsigned char *received_data = (unsigned char*) malloc(packetsize);
	srand(time(NULL));
   	 // populate with random data
    	i = 0;
	while(current_iteration < iterations)
	{
    
		while(i < packetsize)
	       	{
			unsigned char num = (rand() %  (255 - 30 + 1)) + 30;             		
 			randomdata[i] = num;
			//printf("%d ", randomdata[i]); 
        		i += 1;				
		}
		i = 0;
		frame_transfer('w' , packetsize , 1 , randomdata , NULL);
		frame_transfer('r' , packetsize , 1 , NULL , received_data);
		
		mismatch = 0;
		while( i < packetsize)
		{
			if(*received_data++ != *randomdata++)
			       mismatch += 1;
	   		i += 1;
		}
		i = 0;	
		if(mismatch)
			fail_count += 1;
		else 
		{
			success_count += 1;
			printf("%dS\n",success_count);
		}
		current_iteration += 1; 
	}
	printf("Transmits: %d	Retransmits: %d",transmits ,retransmits);
	printf("\nPacket size: %d bytes \n Packets Sent: %d \nSuccess Count: %d\nFail Count: %d\n",packetsize, iterations ,success_count, fail_count); 
	transmits = 0; 
	retransmits = 0;
	return 0;	
}




