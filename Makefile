#
#
CC=gcc
CFLAGS=-g -pthread

# comment line below for Linux machines
#LIB= -lsocket -lnsl

all: master player

master:	
	$(CC) $(CFLAGS) -o master master.c socketlib.c potato_protocol.c

player:	speak.o socketlib.o 
	$(CC) $(CFLAGS) -o player player.c socketlib.c potato_protocol.c


clean:
	\rm -f master player
	\rm *.o

squeaky:
	make clean
	\rm -f master.o player.o

tar:
	#cd ..; tar czvf socket.tar.gz socket/Makefile socket/listen.c socket/speak.c socket/README; cd socket; mv ../socket.tar.gz .

