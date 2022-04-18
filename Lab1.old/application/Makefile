CC = gcc
C_FLAGS = -g -Wall

gcc:
	$(CC) $(C_FLAGS) -c linklayer.c aux.c data.c
	$(CC) main.c linklayer.o aux.o data.o -o app

tx:
	./app /dev/ttyS10 tx penguin.gif

rx:
	./app /dev/ttyS11 rx penguin-recieved.gif

txlab:
	./app /dev/ttyS0 tx penguin.gif

rxlab:
	./app /dev/ttyS1 rx penguin-recieved.gif

clear:
	rm -f app a.out
	rm -f *.o
	rm -f penguin-recieved.gif