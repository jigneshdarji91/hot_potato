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
	tar czvf potato.tar.gz Makefile master.c master.h player.c player.h socketlib.c socketlib.h potato_protocol.c potato_protocol.h playerinfo.h debug.h README REFERENCES

