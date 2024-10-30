SHELL:=/bin/bash
obj-m += jetclocks.o

PWD := $(CURDIR)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	$(eval EXTFILE := $(shell find /boot/extlinux -name "extlinux.conf" -exec basename {} \;))
	$(eval CHECKER := $(shell grep /boot/extlinux/$(EXTFILE) -e "DEFAULT jetclocks")) 
	$(eval LABEL := $(shell grep -i "DEFAULT" /boot/extlinux/$(EXTFILE) | sed 's/DEFAULT //g'))
	$(eval DTBFILE := $(shell find /boot/dtb -name "*kernel*.dtb" -exec basename {} \; | head -n 1))
	@if [ "$(CHECKER)" = "" ] && [ "$(DTBFILE)" != "" ]; then\
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install;\
		touch /etc/modules-load.d/jetclocks.conf;\
		echo "# Load jetclocks module\njetclocks" > /etc/modules-load.d/jetclocks.conf;\
		cp $(PWD)/jetclocks.h /usr/include;\
		depmod -a;\
		echo "jetclocks.conf created into /etc/modules-load.d/";\
		echo "Finding device tree blob";\
		echo "Creating backup of: /boot/dtb/$(DTBFILE) -> /boot/$(DTBFILE).jetclocks.backup";\
		cp /boot/dtb/$(DTBFILE) /boot/$(DTBFILE).jetclocks.backup;\
		echo "Applying overlay blob to: /boot/dtb/$(DTBFILE)";\
		fdtoverlay -i /boot/dtb/$(DTBFILE) -o /boot/dtb/temp.dtb $(PWD)/overlay_blob/jetclocks.dtbo;\
		mv /boot/dtb/temp.dtb /boot/dtb/$(DTBFILE);\
		echo "Creating backup of: /boot/extlinux/$(EXTFILE) -> /boot/extlinux/$(EXTFILE).jetclocks.backup";\
		cp /boot/extlinux/$(EXTFILE) /boot/extlinux/$(EXTFILE).jetclocks.backup;\
		echo "Modifying: /boot/extlinux/$(EXTFILE)";\
		echo -e "\n" > $(PWD)/extlinux.conf.temp;\
		echo "`awk '/LABEL $(LABEL)/{flag=1} flag; /APPEND;next/{flag=0}' /boot/extlinux/$(EXTFILE) | sed '/^#/d'`" > $(PWD)/extlinux.conf.temp;\
		sed -i '0,/LABEL $(LABEL)/s//LABEL jetclocks/' $(PWD)/extlinux.conf.temp;\
		sed -i 's/MENU\sLABEL\s/MENU LABEL $(LABEL)/g' $(PWD)/extlinux.conf.temp;\
		sed -i '/FDT/d' $(PWD)/extlinux.conf.temp;\
		sed -i '/LINUX/a\      FDT /boot/dtb/$(DTBFILE)' $(PWD)/extlinux.conf.temp;\
		echo "`cat $(PWD)/extlinux.conf.temp`" >> /boot/extlinux/$(EXTFILE);\
		sed -i '/DEFAULT/c\DEFAULT jetclocks' /boot/extlinux/$(EXTFILE);\
		echo "All done, you should reboot your machine now";\
	fi

uninstall:
	$(eval JETFILE := $(shell find /lib/modules/$(uname -r) -name "jetclocks.ko"))
	@rm -f $(JETFILE)
	@rm -f /etc/modules-load.d/jetclocks.conf
	@rm -f /usr/include/jetclocks.h
	@echo "$(JETFILE) removed"	
	@echo "/etc/modules-load.d/jetclocks.conf removed"
	@echo "/usr/include/jetclocks.h removed"
	depmod -a
	$(eval CHECKER := $(shell grep /boot/extlinux/extlinux.conf -e "DEFAULT jetclocks"))
	$(eval EXTFILE := $(shell find /boot/extlinux -name "*jetclocks.backup" -exec basename {} \;))
	@if [ "$(CHECKER)" != "" ]; then\
		cp /boot/extlinux/$(EXTFILE) /boot/extlinux/extlinux.conf;\
		echo "/boot/extlinux/$(EXTFILE) restored to /boot/extlinux/extlinux.conf";\
		rm -f /boot/extlinux/$(EXTFILE);\
	fi
