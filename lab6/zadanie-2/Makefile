CC=gcc
CFLAGS=-Wall
.PHONY: all clean
all: main integral_calc
integral_calc: integral_calc.c
	$(CC) $(CFLAGS) integral_calc.c -o integral_calc
main: main.c 
	$(CC) $(CFLAGS) main.c -o main
clean:
	rm -f main integral_calc input_fifo result_fifo
