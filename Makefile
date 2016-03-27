#
#
CC=gcc
CFLAGS=-g -pthread

# comment line below for Linux machines
#LIB= -lsocket -lnsl

all: master player

master:	master.o socketlib.o potato_protocol.o
	$(CC) $(CFLAGS) -o master master.c socketlib.c potato_protocol.c

player:	player.o socketlib.o potato_protocol.o
	$(CC) $(CFLAGS) -o player player.c socketlib.c potato_protocol.c


clean:
	\rm -f master player
	\rm *.o

squeaky:
	make clean
	\rm -f master.o player.o

tar:
	#cd ..; tar czvf socket.tar.gz socket/Makefile socket/listen.c socket/speak.c socket/README; cd socket; mv ../socket.tar.gz .
	tar czvf potato.tar.gz debug.h Makefile master.h player.c README socketlib.h player.h potato_protocol.c master.c playerinfo.h  potato_protocol.h  socketlib.c 


