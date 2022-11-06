CC=gcc
CFLAGS=-c -Wall -Werror -fpic
LDFLAGS=-shared

CFILES=jetgpio.c
OBJ=jetgpio.o
LIB=libjetgpio.so
LIBS=-lpthread -lrt

all:
	$(CC) $(CFLAGS) $(CFILES)
	$(CC) $(LDFLAGS) -o $(LIB) $(OBJ) $(LIBS)

clean:
	rm -f *.o $(LIB)

install:
	install -m 0755 $(LIB) /usr/lib
	install -m 0644 jetgpio.h /usr/include
	ldconfig
	ldconfig -p | grep libjetgpio.so

uninstall:
	rm -rf /usr/lib/$(LIB)
	rm -rf /usr/include/jetgpio.h
	ldconfig	



