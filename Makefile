CC=gcc
CFLAGS=-std=c11 -Wall -Werror -Wextra -pedantic -g

file: file.c
	$(CC) -o file file.c $(CFLAGS)