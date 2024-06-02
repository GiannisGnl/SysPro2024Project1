CC=gcc
CFLAGS=-Wall -g

all: jobCommander jobExecutorServer

jobCommander: jobCommander.c 
	$(CC) $(CFLAGS) jobCommander.c -o jobCommander

jobExecutorServer: jobExecutorServer.c 
	$(CC) $(CFLAGS) jobExecutorServer.c -o jobExecutorServer

clean:
	rm -f jobCommander jobExecutorServer