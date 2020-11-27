CC = gcc

CFLAGS = -Wall -pedantic -g


all: mush
mush: mush.c exec.c parseline.c pipe.c share.c define.h
	$(CC) $(CFLAGS) -o mush mush.c
run: mush
	./mush
