/*reference: https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c */
/*Code has been modified in order to send sensor data to server over socket - Author - Ananth Deshpande*/

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define MAX 10000
#define PORT 8080
#define SA struct sockaddr
void func(int sockfd)
{
	char buff[MAX];
	int n;
 
        int fd = open("/var/tmp/sensorLog.txt", O_RDONLY, 0644);

	bzero(buff, sizeof(buff));

	int readRetVal = read(fd, buff, MAX);

        if(readRetVal < 0)
	{
	   printf("sensorlog file read error\n");
	}

	int writeRetVal = write(sockfd, buff, readRetVal);

	if(writeRetVal < 0)
	{
	   printf("write to socket failed\n");
	}


	//for (;;) 
	/*{
		bzero(buff, sizeof(buff));
		printf("Enter the string : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;
		write(sockfd, buff, sizeof(buff));
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		printf("From Server : %s", buff);
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}
	}*/
}

int main(int argc, char* argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;
	char *ipStr = "127.0.0.1";

	if(argc == 2)
	{
	   /*implies we have an input ip address*/
	   ipStr = argv[1];
	}

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
	servaddr.sin_addr.s_addr = inet_addr(ipStr);
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	func(sockfd);

	// close the socket
	close(sockfd);
}

