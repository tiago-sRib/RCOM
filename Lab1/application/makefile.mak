CC = gcc
C_FLAGS = -g -Wall -pipe

gcc:
	$(CC) $(C_FLAGS) linklayer.c aux.c -c
	$(CC) main.c linklayer.o aux.o -o app

tx:
	./app /dev/ttyS10 tx penguin.gif

rx:
	./app /dev/ttyS11 rx penguin-recieved.gif

clear:
	rm -f app a.out
	rm -f *.o
	rm -f penguin-recieved.gif