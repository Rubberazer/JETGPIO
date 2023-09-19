SHELL:=/bin/bash
CC=gcc
MODELFILE=get_chip_id.c
MODELTARGET=get_chip_id
C_EXTENSION=.c
OBJ_EXTENSION=.o
CFLAGS=-c -Wall -Werror -fpic
LDFLAGS=-shared
LIB=libjetgpio.so
LIBS=-lpthread -lrt

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
	@if [ $(MODEL) == orin ]; then\
		cp ./scripts/pwm_enabler.sh /etc/systemd/system;\
		cp ./scripts/pwm_enable.service /etc/systemd/system;\
		chmod +x /etc/systemd/system/pwm_enabler.sh;\
		chown root /etc/systemd/system/pwm_enabler.sh;\
		systemctl enable pwm_enable.service;\
		systemctl start pwm_enable.service;\
	fi
step5:
	rm -f /usr/lib/$(LIB)
	rm -f /usr/include/jetgpio.h
	ldconfig
	@if [ $(MODEL) == orin ]; then\
		systemctl disable pwm_enable.service;\
		rm -f /etc/systemd/system/pwm_enabler.sh;\
		rm -f /etc/systemd/system/pwm_enable.service;\
	fi
nano:
	$(CC) $(CFLAGS) nano.c
	$(CC) $(LDFLAGS) -o $(LIB) nano.o $(LIBS)
	@echo nano >  ./hardware
orin:
	$(CC) $(CFLAGS) orin.c
	$(CC) $(LDFLAGS) -o $(LIB) orin.o $(LIBS)
	@echo orin >  ./hardware
clean:
	rm -f *.o $(LIB) get_chip_id


install: step2 step4

uninstall: step2 step5

