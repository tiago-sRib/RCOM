CC = gcc
C_FLAGS = -g -Wall

gcc:
	$(CC) $(C_FLAGS) -c arg.c
	$(CC) download.c arg.o -o app

test1:
	./app ftp://ftp.up.pt/pub/gnu/GNUinfo/Audio/index.txt

clear:
	rm -f app
	rm -f *.o