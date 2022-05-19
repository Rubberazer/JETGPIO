CC=gcc
CFLAGS=-c -Wall -Werror -fpic
LDFLAGS=-shared

CFILES=jetgpio.c
OBJ=jetgpio.o
LIB=libjetgpio.so

all:
	$(CC) $(CFLAGS) $(CFILES)
	$(CC) $(LDFLAGS) -o $(LIB) $(OBJ)

clean:
	rm -f *.o $(LIB)

install:
	install -m 0755 $(LIB) /usr/lib
	install -m 0644 jetgpio.h /usr/include
	ldconfig
	ldconfig -p | grep libjetgpio.so

	



