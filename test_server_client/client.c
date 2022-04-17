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
uint8_t buf[1];

void initHumidity(int fdev)
{
   buf[0] = 0xFE;
   int retval=write(fdev, buf, 1);
   if (retval < 0)
    {
      printf ("\n\rError in writing (Soft reset).");
    }
}   

void readHumidity(int fdev, int sockfd)
{
    //Hold master mode for measuring humidity
    char strBuf[MAX];
      buf[0] = 0xE5;
   int retval=write(fdev, buf, 1);
   if (retval < 0)
    {
      printf ("\n\rError in writing (Soft reset).");
    }

  // 2 sec delay before performing read operation
      sleep (2);
// device response, 14-bit ADC value:
//  first 8 bit part ACK  second 8 bit part        CRC
// [0 1 2 3 4 5 6 7] [8] [9 10 11 12 13 14 15 16] [17 18 19 20 21 22 23 24]
// bit 15 - measurement type (‘0’: temperature, ‘1’: humidity)
// bit 16 - currently not assigned

   uint8_t buf2[3] = { 0 };

   retval=read(fdev, buf2, 3);
   if (retval < 0)
    {
      printf ("\n\rError in reading.");
    }
    
    else if (retval == 0)
	{
	  printf ("\n\rNo data to read.");
	}
      else
	{
	  printf ("\n\rData successfully read.");
	}

      usleep (4000);

   //uint16_t sensor_data = (buf2 [0] << 8 | buf2 [1]) & 0xFFFC;
      uint16_t sensor_data = 0;
      sensor_data = buf2[0] << 8;
      sensor_data += buf[1];
      sensor_data &= ~0x003;

     
// temperature
//double sensor_tmp = sensor_data / 65536.0;
//double result = -46.85 + (175.72 * sensor_tmp);
//printf("Temperature: %.2f C\n", result);
// humidity
double result = (-6.0 + 125.0 / 65536 * (double) sensor_data);

      printf ("Humidity: %.2f %%\n", result);

      int sprintfRetVal = sprintf(strBuf, "Humidity = %.2f\n", result);

      int retVal = write(sockfd, strBuf, sprintfRetVal);

      if(retVal < 0)
      {
	 printf("file write failed\n");
      }      

      sleep (1);
}

void func(int sockfd, int tempfdev, int humidityfdev)
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

	initHumidity(humidityfdev);

        while(1)
        {

            // do actual request
            if (ioctl(tempfdev, I2C_SMBUS, &sdat) < 0)
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

	    readHumidity(humidityfdev, sockfd);
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
	int tempfdev = open(I2C_DEV_PATH, O_RDWR);

        if(tempfdev < 0)
	{
		fprintf(stderr, "Failed to open I2C interfcae %s Error: %s\n", I2C_DEV_PATH, strerror(errno));
		return -1;
	}

	unsigned char i2c_addr = DEVICE_ADDRESS;

	//set slave device address, MLX's address is 0x5A
	if(ioctl(tempfdev, I2C_SLAVE, i2c_addr) < 0)
	{
		fprintf(stderr, "Failed to select I2C slave device, Error: %s\n", strerror(errno));
		return -1;
	}

	//to enable checksums control
	if(ioctl(tempfdev, I2C_PEC, 1) < 0)
	{
		fprintf(stderr, "Failed to enable SMBus packet error checking, error:%s\n", strerror(errno));
		return -1;
	}

        //set slave device address to 0x40
	
	int humidityfdev = open(I2C_DEV_PATH, O_RDWR);

        if(humidityfdev < 0)
	{
		fprintf(stderr, "failed to open humidityI2C interface %s Error: %s\n", I2C_DEV_PATH, strerror(errno));
	}

	unsigned char humidity_i2c_addr = 0x40;
        if(ioctl(humidityfdev, I2C_SLAVE, humidity_i2c_addr) < 0)
	{
		fprintf(stderr, "Failed to select I2c slave device ! Error: %s\n", strerror(errno));
		return -1;
	}

	// function for sending the sensor data over socket
	func(sockfd, tempfdev, humidityfdev);
}
#if 0

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/types.h>


#define I2C_DEV_PATH    ("/dev/i2c-1")

int main()
{
 int fdev = open("/dev/i2c-1", O_RDWR); // open i2c bus

  if (fdev < 0)
  {
    fprintf(stderr, "Failed to open I2C interface %s Error: %s\n", I2C_DEV_PATH, strerror(errno));
    return -1;
  }

  unsigned char i2c_addr = 0x40;
// set slave device address 0x40
  if (ioctl(fdev, I2C_SLAVE, i2c_addr) < 0)
 {
    fprintf(stderr, "Failed to select I2C slave device! Error: %s\n", strerror(errno));
    return -1;
 }
   uint8_t buf[1];
   buf[0] = 0xFE;
   int retval=write(fdev, buf, 1);
   if (retval < 0)
    {
      printf ("\n\rError in writing (Soft reset).");
    }

  //17 ms delay after soft-reset
  usleep (17000);

   while (1)
   {

    //Hold master mode for measuring humidity
      buf[0] = 0xE5;
     retval=write(fdev, buf, 1);
   if (retval < 0)
    {
      printf ("\n\rError in writing (Soft reset).");
    }

  // 2 sec delay before performing read operation
      sleep (2);


// device response, 14-bit ADC value:
//  first 8 bit part ACK  second 8 bit part        CRC
// [0 1 2 3 4 5 6 7] [8] [9 10 11 12 13 14 15 16] [17 18 19 20 21 22 23 24]
// bit 15 - measurement type (‘0’: temperature, ‘1’: humidity)
// bit 16 - currently not assigned

   uint8_t buf2[3] = { 0 };

   retval=read(fdev, buf2, 3);
   if (retval < 0)
    {
      printf ("\n\rError in reading.");
    }

    else if (retval == 0)
	{
	  printf ("\n\rNo data to read.");
	}
      else
	{
	  printf ("\n\rData successfully read.");
	}

      usleep (4000);

   //uint16_t sensor_data = (buf2 [0] << 8 | buf2 [1]) & 0xFFFC;
      uint16_t sensor_data = 0;
      sensor_data = buf2[0] << 8;
      sensor_data += buf[1];
      sensor_data &= ~0x003;


// temperature
//double sensor_tmp = sensor_data / 65536.0;
//double result = -46.85 + (175.72 * sensor_tmp);
//printf("Temperature: %.2f C\n", result);
// humidity
double result = (-6.0 + 125.0 / 65536 * (double) sensor_data);

      printf ("Humidity: %.2f %%\n", result);
      sleep (1);

  }
}
#endif
