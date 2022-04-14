/*reference: https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c */
/*Code has been modified in order to send sensor data to server over socket - Author - Ananth Deshpande*/

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#define MAX 10000
#define PORT 8080
#define SA struct sockaddr
#define I2C_DEV_PATH   ("/dev/i2c-1")
#define INTERVAL       (3000000)
#define DEVICE_ADDRESS (0x5A)
#define ROOM_TEMP      (0x06)     // To sense room temperature
#define OBJECT_TEMP    (0x07)     // To sense object temperature

#ifndef I2C_SMBUS_READ 
#define I2C_SMBUS_READ 1 
#endif 
#ifndef I2C_SMBUS_WRITE 
#define I2C_SMBUS_WRITE 0 
#endif

typedef union i2c_smbus_data i2c_data;

void func(int sockfd, int fdev)
{
	char strBuf[MAX];
	int n;

	bzero(strBuf, sizeof(strBuf));

        // trying to read something from the device unsing SMBus READ request
        i2c_data data;
	
        char command = ROOM_TEMP; // command 0x06 is reading thermopile sensor, 0x07 is for reading IR sensor	

	struct i2c_smbus_ioctl_data sdat =
        {
	   .read_write = I2C_SMBUS_READ,
	   .command = command,
	   .size = I2C_SMBUS_WORD_DATA,
           .data = &data
        };

        while(1)
        {

            // do actual request
            if (ioctl(fdev, I2C_SMBUS, &sdat) < 0)
            {
               fprintf(stderr, "Failed to perform I2C_SMBUS transaction, error: %s\n", strerror(errno));
            }

            // calculate temperature in Celsius by formula from datasheet
            double temp = (double) data.word;
            temp = (temp * 0.02)-0.01;
            temp = temp - 273.15;

            // print result
            printf("Room Temperature = %04.2f\n", temp);
            int sprintfRetVal = sprintf(strBuf, "Room temperature = %04.2f\n", temp);

            int retVal = write(sockfd, strBuf, sprintfRetVal);

       	    if(retVal < 0)
	    {
	       printf("file write failed\n");
	    }

	    usleep(INTERVAL);
        }

	/*int writeRetVal = write(sockfd, buff, readRetVal);

	if(writeRetVal < 0)
	{
	   printf("write to socket failed\n");
	}*/


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

        
	/*to initialise I2C for temp and humidity sensors*/

	//open i2c bus
	int fdev = open(I2C_DEV_PATH, O_RDWR);

        if(fdev < 0)
	{
		fprintf(stderr, "Failed to open I2C interfcae %s Error: %s\n", I2C_DEV_PATH, strerror(errno));
		return -1;
	}

	unsigned char i2c_addr = DEVICE_ADDRESS;

	//set slave device address, MLX's address is 0x5A
	if(ioctl(fdev, I2C_SLAVE, i2c_addr) < 0)
	{
		fprintf(stderr, "Failed to select I2C slave device, Error: %s\n", strerror(errno));
		return -1;
	}

	//to enable checksums control
	if(ioctl(fdev, I2C_PEC, 1) < 0)
	{
		fprintf(stderr, "Failed to enable SMBus packet error checking, error:%s\n", strerror(errno));
		return -1;
	}

	// function for chat
	func(sockfd, fdev);

	// close the socket
	close(sockfd);
}

