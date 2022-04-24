CC = gcc
C_FLAGS = -g -Wall

gcc:
	$(CC) $(C_FLAGS) -c linklayer.c aux.c data.c
	$(CC) main.c linklayer.o aux.o data.o -o app

tx:
	./app /dev/ttyS10 tx penguin.gif

rx:
	./app /dev/ttyS11 rx penguin-r.gif

txlab:
	./app /dev/ttyS0 tx penguin.gif

rxlab:
	./app /dev/ttyS1 rx penguin-r.gif

tx_png:
	./app /dev/ttyS10 tx rubiks_cube.png

rx_png:
	./app /dev/ttyS11 rx rubiks_cube_r.png

clear:
	rm -f app
	rm -f *.o
	rm -f penguin-r.gif rubiks_cube_r.png