CC = gcc
CFLAGS = -Wall
.PHONY: all clean debug
all: countdown

countdown: countdown.c
	$(CC) $(CFLAGS) countdown.c -o countdown

debug: countdown.c
	$(CC) $(CFLAGS) -g countdown.c -o countdown_debug

clean:
	rm -f countdown countdown_debug