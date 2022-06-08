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
	./app ftp://rcom:rcom@netlab1.fe.up.pt/files/pic2.png

test6:
	./app ftp://rcom:rcom@netlab1.fe.up.pt/files/crab.mp4

clear:
	rm -f app
	rm -f *.o
	rm -f index.txt
	rm -f pub.txt
	rm -f pipe.txt
	rm -f pic1.jpg
	rm -f pic2.png
	rm -f crab.mp4
	clear
