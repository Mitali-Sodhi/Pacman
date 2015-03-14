INC_PATH = -I/usr/local/include/player-3.0
LIB_PATH = -L/usr/local/lib64 -lplayerc -lm -lz -lplayerinterface -lplayerwkb -lplayercommon

all:
	rm -rf pacman
	gcc -o pacman $(INC_PATH) pacman.c $(LIB_PATH)

pacman:	pacman.c
	gcc -o pacman $(INC_PATH) pacman.c $(LIB_PATH)


clean:
	rm -rf pacman
	
	gcc file.c -o file -lm -lz -lplayerint -lplayerc -I.
	
	#include <stdio.h>
	#include <util.h>
