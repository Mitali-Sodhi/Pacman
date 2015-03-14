all: 
	rm -rf pacman ghost1 
	gcc -o pacman `pkg-config --cflags playerc` pacman.c `pkg-config --libs playerc` 
	gcc -o ghost1 `pkg-config --cflags playerc` ghost1.c `pkg-config --libs playerc`

pacman: pacman.c
	gcc -o pacman `pkg-config --cflags playerc` pacman.c `pkg-config --libs playerc`

ghost1: ghost1.c
	gcc -o ghost1 `pkg-config --cflags playerc` ghost1.c `pkg-config --libs playerc` 


clean:
	rm -rf pacman ghost1
