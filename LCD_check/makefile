CROSS_COMPILE=

ifeq ($(CC),)
        CC = $(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
        CFLAGS = -Wall -Werror -g
endif

TARGET = lcd
OBJFILES = wiringPi.o lcd.o

all: $(TARGET)
default: $(TARGET)

$(TARGET): $(OBJFILES)
		$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJFILES)

.PHONY: clean

clean:
		rm -f *.o $(TARGET)
