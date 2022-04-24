CROSS_COMPILE=

ifeq ($(CC),)
        CC = $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
#        CFLAGS = -Wall -Werror -g
         CFLAGS = -g
endif

ifeq ($(LDFLAGS),)
	LDFLAGS = -pthread -lrt
endif

TARGET = server_display 
#TARGET = lcd
OBJFILES = wiringPi.o lcd.o server_display.o
#OBJFILES = wiringPi.o lcd.o 


all: $(TARGET)
default: $(TARGET)

$(TARGET): $(OBJFILES)
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJFILES)
		

.PHONY: clean

clean:
		rm -f *.o $(TARGET)
