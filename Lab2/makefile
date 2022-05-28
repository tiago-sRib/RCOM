CC = gcc
C_FLAGS = -g -Wall

gcc:
	$(CC) $(C_FLAGS) -c arg.c
	$(CC) download.c arg.o -o app


clear:
	rm -f app
	rm -f *.o