CC = gcc

CFLAGS = -Wall -pedantic -g


all: mush
mush: mush.c parseline.c pipe.c
	$(CC) $(CFLAGS) -o mush mush.c
run: mush
	./mush
