/*Reference: https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c*/
/*code has been modified to read sensor data over the socket*/
// 24th April

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "wiringPi.h"
#include "lcd.h"

#define MAX 10000
#define PORT 8080
#define SA struct sockaddr


static int lcdAddr[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88,0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
                       0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8 ,0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	               0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0X97, 0x98, 0x99, 0x9A, 0x9B, 0x9C ,0x9D, 0x9E, 0x9F,
	               0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC ,0xDD, 0xDE, 0xDF};



// Function designed for chat between client and server.
void readSensorDataFromClient(int connfd)
{
  char buff[MAX];
  lcd_init();
	// infinite loop for chat
  for (;;) 
  {
    bzero(buff, MAX);
    // read the message from client and copy it in buffer
    read(connfd, buff, sizeof(buff));
 
    //Displays the read buffer on LCD 
    char *p = buff;
    int i = 0;
    //reads reach char and writes it to the specific location on LCD
    while (*p != '\0')
    {
      printChar(*p, lcdAddr[i]);
      delay(3);
      i++;
      p++;
      if(63 == i)
        i = 0;
    }
    delay(3);
    SetCmdMode();
    delay(3);
    lcd_byte(0x02);   //returns to home posotion for next message ftom client
    delay(3);
  
    // print buffer which contains the client contents
    printf(" %s\t ", buff);
    bzero(buff, MAX);
  }
}

// Driver function
int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accept the client...\n");

	// Function for chatting between client and server
	readSensorDataFromClient(connfd);

	// After chatting close the socket
	close(sockfd);
}

