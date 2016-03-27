CC=gcc
CFLAGS=-g -pthread
OBJ_MASTER=master.o socketlib.o potato_protocol.o
OBJ_PLAYER=player.o socketlib.o potato_protocol.o

# comment line below for Linux machines
#LIB= -lsocket -lnsl

all: master player

master:	$(OBJ_MASTER)
	$(CC) $(CFLAGS) -o $@ $(OBJ_MASTER)

player:	$(OBJ_PLAYER)
	$(CC) $(CFLAGS) -o $@ $(OBJ_PLAYER)


clean:
	\rm -f master player
	\rm *.o

squeaky:
	make clean
	\rm -f master.o player.o

tar:
	#cd ..; tar czvf socket.tar.gz socket/Makefile socket/listen.c socket/speak.c socket/README; cd socket; mv ../socket.tar.gz .
	tar czvf potato.tar.gz debug.h Makefile master.h player.c README socketlib.h player.h potato_protocol.c master.c playerinfo.h  potato_protocol.h  socketlib.c 


