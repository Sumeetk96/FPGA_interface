/*
    Simple udp client
*/

#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>

/******TO BE MODIFIED BY USER*****/

#define SERVER "192.168.2.6" //Enter the nodeMCU server address here

/*******************************/


#define BUFLEN 1024  //Max length of buffer
#define PORT 4080  //The port on which to send data
 
    char buffer[1024];
    unsigned char start_Byte1 = 0x19;
    unsigned char start_Byte2 = 0x1e;
    unsigned char No_of_Data = 0x00;
    unsigned char stop_Byte = 0x1c;
    unsigned char Temp_Byte = 0x00;
    int Address = 0x00;

void die(char *s)
{
    perror(s);
    exit(1);
}

/*********** Function To Print Binary value **************/

void bin(unsigned char n)
{
    /* step 1 */
    if (n > 1)
    bin(n/2);
 
    /* step 2 */
    printf("%d", n % 2);
}

int main(int argc, char* argv[])
{
    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    char buf[BUFLEN];
 
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
 
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
     
    if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    while(1)
    {
	printf("\nEnter Read/Write: \n");
	scanf("%c",&Temp_Byte);

/********************** Function To Build Fram ****************************/	
	
	
	
        if(Temp_Byte == 'W' || Temp_Byte == 'w' || Temp_Byte == '1' )
	{
		printf("\nEnter message : \n");
		scanf("%s",buffer);

		do
		{
			printf("\nEnter Address between 0 and 4094: \n");
			scanf("%d",&Address);
			
		}
		while(Address > 4095);

		// 0b01010101<<1 | 0b00000001 i.e 0b10101011
		
		No_of_Data = strlen(buffer);

		char message[No_of_Data + 5];
		
		message[0] = start_Byte1;

		message[1] = start_Byte2;
	
		message[2] = (unsigned char) (Address >> 7) & 0x1F;
                
		message[3] = (unsigned char) (Address << 1) | 1;
		
		for( i = 0; i < No_of_Data ; i++ )
		{	
			//printf("%c",buffer[i]);
			message[4+i] = buffer[i];
		}
		printf("\n");
		message[No_of_Data +4] = stop_Byte;

		for( i = 0; i <= No_of_Data +4 ; i++ )
		{
			//printf("%c \t ",Frame[i]);
			bin(message[i]);
			printf(" \t  %c\n",message[i]);
		
		}
		printf("\n");

	/*****************************************************************************/
		 printf("\n%d\n",No_of_Data +5);
		message[2]++;
		message[3]++;
		char message1[] = {0x45,0x23,0x67,0x00,0x56,0x78,0x00};
		//send the message
		if (sendto(s, message1, 7, 0 , (struct sockaddr *) &si_other, slen)==-1)
		{
		    die("sendto()");
		}
		 memset(buf,'\0', BUFLEN);
		//try to receive some data, this is a blocking call
		if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
		{
		    die("recvfrom()");
		}
		 
		for (i = 0 ; i < 10 ; i++)
		printf("%d",buf[i]);


	}
	else if(Temp_Byte == 'R' || Temp_Byte == 'r' || Temp_Byte == '0' )
	{
		printf("\nEnter No of Bytes to be Read : \n");
		scanf("%d",&No_of_Data);
		
		do
		{
			printf("\nEnter Address between 0 and 4094 : \n");
			scanf("%d",&Address);
			
		}


		while(Address> 4094);


		// 0b01010101<<1 & 0b11111110 i.e 0b10101011
		//Addr_Byte = (Addr_Byte<<1) & 0xFE;

		//No_of_Data = strlen(buffer);

		unsigned char message_Rx[7];
		message_Rx[0] = start_Byte1;
	
		message_Rx[1] = start_Byte2;		

		message_Rx[2] = (unsigned char) (Address>> 7) & 0x1F;
                
		message_Rx[3] = (unsigned char) (Address << 1) & 0xFE;
			
		message_Rx[4] = (unsigned char) (No_of_Data >> 8);
                
		message_Rx[5] = (unsigned char) (No_of_Data);
		
		message_Rx[6] = stop_Byte;
		
		printf("\n");
		for( i = 0; i < 7 ; i++ )
		{
			//printf("%c \t ",Frame[i]);
			bin(message_Rx[i]);
			printf(" \t  %c\n",message_Rx[i]);
		
		}
		printf("\n");

	/*****************************************************************************/
		
		//for(i = 2 ; i <= 5 ; i++)
		//message_Rx[i]++;
		//send the message_Rx
		if (sendto(s, message_Rx, 7 , 0 , (struct sockaddr *) &si_other, slen)==-1)
		{
		    die("sendto()");
		}
		 
		//receive a reply and print it
		//clear the buffer by filling null, it might have previously received data
		memset(buf,'\0', BUFLEN);
		//try to receive some data, this is a blocking call
		if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
		{
		    die("recvfrom()");
		}
		 
		puts(buf);
	}
    }
    close(s);
    return 0;
}
