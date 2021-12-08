CC = gcc
CFLAGS = -Wall -std=c11 -g -ggdb

all: sim

../src.zip: *.c *.h
	cd .. && zip src.zip src/*


sim: *.c *.h
	$(CC) $(CFLAGS) *.c -o sim

clean:
	rm -rf *.o sim test_runs
