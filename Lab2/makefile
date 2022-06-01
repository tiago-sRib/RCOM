CC = gcc
C_FLAGS = -g -Wall

gcc:
	$(CC) $(C_FLAGS) -c arg.c
	$(CC) $(C_FLAGS) download.c arg.o -o app

test1:
	./app ftp://ftp.up.pt/pub/gnu/GNUinfo/Audio/index.txt

test2: 
	./app ftp://netlab1.fe.up.pt/pub.txt

test3:
	./app ftp://rcom:rcom@netlab1.fe.up.pt/pipe.txt

test4:
	./app ftp://rcom:rcom@netlab1.fe.up.pt/files/pic1.jpg

test5:
	./app ftp://rcom:rcom@netlab1.fe.up.pt/files/pic2.jpg

clear:
	rm -f app
	rm -f *.o