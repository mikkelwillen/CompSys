GCC=gcc -O3 -g -Wall -Wextra -pedantic -std=gnu11
LD_FLAGS= -lpthread

all: peer name_server cascade

rebuild: clean all

csapp.o: csapp.c csapp.h
	$(GCC) -c $< -o $@

common.o: common.c common.h
	$(GCC) -c $< -o $@

cascade: cascade.c cascade.h common.o csapp.o sha256.o
	$(GCC) $< *.o -o $@ $(LD_FLAGS)

name_server: name_server.c name_server.h common.o csapp.o
	$(CC) $(CFLAGS) $< *.o -o $@ $(LD_FLAGS)

sha256.o : sha256.c sha256.h
	$(CC) $(CFLAGS) -c $< -o $@

zip: ../src.zip

../src.zip: clean
	cd .. && zip -r src.zip src/Makefile src/*.c src/*.h src/tests/*

clean:
	rm -rf *.o peer name_server cascade sha256 vgcore*
