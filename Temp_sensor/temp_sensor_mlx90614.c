/*Reference : https://olegkutkov.me/2017/08/10/mlx90614-raspberry/
*
*
*/

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <linux/i2c-dev.h>

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
int main()
{
    int fdev = open(I2C_DEV_PATH, O_RDWR); // open i2c bus

    if (fdev < 0) 
	{
        fprintf(stderr, "Failed to open I2C interface %s Error: %s\n", I2C_DEV_PATH, strerror(errno));
        return -1;
    }

    unsigned char i2c_addr = DEVICE_ADDRESS;

    // set slave device address, default MLX is 0x5A
    if (ioctl(fdev, I2C_SLAVE, i2c_addr) < 0) 
	{
        fprintf(stderr, "Failed to select I2C slave device! Error: %s\n", strerror(errno));
        return -1;
    }

    // enable checksums control
    if (ioctl(fdev, I2C_PEC, 1) < 0) 
	{
        fprintf(stderr, "Failed to enable SMBus packet error checking, error: %s\n", strerror(errno));
        return -1;
    }


    // trying to read something from the device unsing SMBus READ request
    i2c_data data;
	
    char command = ROOM_TEMP; // command 0x06 is reading thermopile sensor, 0x07 is for reading IR sensor

    // build request structure
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
            return -1;
        }
        
        // calculate temperature in Celsius by formula from datasheet
        double temp = (double) data.word;
        temp = (temp * 0.02)-0.01;
        temp = temp - 273.15;
        
        // print result
        printf("Room Temperature = %04.2f\n", temp);
		usleep(INTERVAL);
	
	}

    return 0;
}