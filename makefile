
CFLAGS=--std=c99 -Weverything -g

.PHONY: clean all

all: main
vector.c: vector.h
main.c: vector.h
main: main.c vector.c
clean:
	$(RM) main
