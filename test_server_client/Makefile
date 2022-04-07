#generic makefile to build for various compilers
ifeq ($(CC),)
        CC = $(CROSS_COMPILE)gcc
endif
ifeq ($(CFLAGS),)
        CFLAGS = -g
endif
ifeq ($(LDFLAGS),)
        LDFLAGS = -pthread -lrt
endif

all: client server
default: all

client: client.c
	$(CC) $(CFLAGS) -o client client.c $(LDFLAGS)

server: server.c
	$(CC) $(CFLAGS) -o server server.c $(LDFLAGS)


.PHONY: all

clean:
	rm client server
	$(RM) $(TARGET)
