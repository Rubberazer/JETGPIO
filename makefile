SHELL:=/bin/bash
CC=gcc
MODELFILE=get_chip_id.c
MODELTARGET=get_chip_id
C_EXTENSION=.c
OBJ_EXTENSION=.o
CFLAGS=-c -Wall -Werror -fpic
LDFLAGS=-shared
LIB=libjetgpio.so
LIBS=-lpthread -lrt -lm

all: step1 step2 step3

step1:
	$(CC) -Wall -Werror -o $(MODELTARGET) $(MODELFILE)
	./$(MODELTARGET)

step2:
	$(eval MODEL := $(shell cat ./hardware))

step3:
	$(CC) $(CFLAGS) $(MODEL)$(C_EXTENSION)
	$(CC) $(LDFLAGS) -o $(LIB) $(MODEL)$(OBJ_EXTENSION) $(LIBS)

step4:
	install -m 0755 $(LIB) /usr/lib
	install -m 0644 jetgpio.h /usr/include
	ldconfig
	ldconfig -p | grep libjetgpio.so
	@if  [ $(MODEL) = "orin" ] || [ $(MODEL) = "orinagx" ]; then\
		cd ./Jetclocks && make;\
		make install;\
	fi

step5:
	$(eval MODEL := $(shell find /lib/modules/$(uname -r) -name "jetclocks.ko" -exec basename {} \;))

step6:
	rm -f /usr/lib/$(LIB)
	rm -f /usr/include/jetgpio.h
	ldconfig
	@if [ "$(MODEL)" == "jetclocks.ko" ]; then\
		cd ./Jetclocks && make uninstall;\
	fi

nano:
	$(CC) $(CFLAGS) nano.c
	$(CC) $(LDFLAGS) -o $(LIB) nano.o $(LIBS)
	@echo nano >  ./hardware

orin:
	$(CC) $(CFLAGS) orin.c
	$(CC) $(LDFLAGS) -o $(LIB) orin.o $(LIBS)
	@echo orin >  ./hardware

orinagx:
	$(CC) $(CFLAGS) orinagx.c
	$(CC) $(LDFLAGS) -o $(LIB) orinagx.o $(LIBS)
	@echo orinagx >  ./hardware

clean:
	rm -f *.o $(LIB) get_chip_id hardware

install: step2 step4

uninstall: step5 step6

