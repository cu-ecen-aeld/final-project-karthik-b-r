CROSS_COMPILE=

ifeq ($(CC),)
	CC = $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS = -Wall -Werror -g 
endif

TARGET = temp_sensor_mlx90614
	
all: $(TARGET) 
default: $(TARGET)
	
temp_sensor_mlx90614: temp_sensor_mlx90614.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o temp_sensor_mlx90614.o temp_sensor_mlx90614.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o temp_sensor_mlx90614 temp_sensor_mlx90614.o
	
        

.PHONY: clean

clean:
	rm -f *.o $(TARGET) 