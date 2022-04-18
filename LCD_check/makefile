CROSS_COMPILE=

ifeq ($(CC),)
        CC = $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
        CFLAGS = -Wall -Werror -g
endif

ifeq ($(LDFLAGS),)
	LDFLAGS = -pthread -lrt
endif

TARGET = server_display
OBJFILES = wiringPi lcd server_display

all: $(TARGET)
default: $(TARGET)

$(TARGET): $(OBJFILES).o
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJFILES).o
		
$(OBJFILES).o: $(OBJFILES).c 		
		$(CC) $(CFLAGS) -c $(OBJFILES)

.PHONY: clean

clean:
		rm -f *.o $(TARGET)
