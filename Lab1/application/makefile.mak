CC = gcc
C_FLAGS = -g -Wall -pipe

all:
	$(CC) $(C_FLAGS) linklayer.c aux.c -c
	$(CC) main.c linklayer.o aux.o -o app

clean:
	rm -f App a.out
	rm -f *.o