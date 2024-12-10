CFLAGS=-c -g -Wall -Werror
CC=gcc

all: server client

server: server.o
	$(CC) -o server server.o

client: client.o
	$(CC) -o client client.o

clean:
	rm -f *.o server client

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

