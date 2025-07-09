CC = gcc
CFLAGS = -Wall
.PHONY: all clean
all: printer user
printer: printer.c
	$(CC) $(CFLAGS) printer.c -o printer
user: user.c
	$(CC) $(CFLAGS) user.c -o user
clean:
	rm -rf printeruser