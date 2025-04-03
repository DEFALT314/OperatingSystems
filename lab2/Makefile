CC = gcc
CFLAGS = -Wall
.PHONY: all clean 

all: libcollatz.a libcollatz.so client-static client-shared client-dynamic

libcollatz.a: libcollatz.o
	$(CC) -c libcollatz.c
	ar rcs libcollatz.a libcollatz.o
	rm libcollatz.o

libcollatz.so: libcollatz.c libcollatz.h
	$(CC) -c -fPIC libcollatz.c
	gcc -shared libcollatz.o -o libcollatz.so
	rm libcollatz.o

clean:
	rm -f libcollatz.a libcollatz.so libcollatz.o clinet-static client-shared client-dynamic

client-static: client.c libcollatz.a
	$(CC) $(CFLAGS) -static client.c -L. -lcollatz -o client-static

client-shared: client.c libcollatz.so
	$(CC) $(CFLAGS) client.c -L. -lcollatz -Wl,-rpath,. -o client-shared

client-dynamic: client.c libcollatz.so
	$(CC) $(CFLAGS) client.c -D DYNAMIC -o client-dynamic
