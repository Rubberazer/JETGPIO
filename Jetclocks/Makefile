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
	$(eval DTBFILE := $(shell find /boot/dtb -name "*kernel*.dtb" -exec basename {} \; | head -n 1))
	@if [ "$(CHECKER)" = "" ] && [ "$(DTBFILE)" != "" ]; then\
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install;\
		touch /etc/modules-load.d/jetclocks.conf;\
		echo "# Load jetclocks module\njetclocks" > /etc/modules-load.d/jetclocks.conf;\
		cp $(PWD)/jetclocks.h /usr/include;\
		depmod -a;\
		echo "jetclocks.conf created into /etc/modules-load.d/";\
		echo "Finding device tree blob";\
		echo "Creating backup of: /boot/dtb/$(DTBFILE) -> /boot/dtb/$(DTBFILE).jetclocks.backup";\
		cp /boot/dtb/$(DTBFILE) /boot/dtb/$(DTBFILE).jetclocks.backup;\
		echo "Applying overlay blob to: /boot/dtb/$(DTBFILE) -> /boot/dtb/jetclocks.dtb";\
		fdtoverlay -i /boot/dtb/$(DTBFILE) -o /boot/dtb/jetclocks.dtb $(PWD)/overlay_blob/jetclocks.dtbo;\
		echo "Creating backup of: /boot/extlinux/$(EXTFILE) -> /boot/extlinux/$(EXTFILE).jetclocks.backup";\
		cp /boot/extlinux/$(EXTFILE) /boot/extlinux/$(EXTFILE).jetclocks.backup;\
		echo "Modifying: /boot/extlinux/$(EXTFILE)";\
		echo "`awk '/LABEL primary/{f=1} /APPEND/{f=0;print} f' /boot/extlinux/extlinux.conf | sed '/#/d'`" > $(PWD)/extlinux.conf.temp;\
		sed -i 's/LABEL\sprimary/LABEL jetclocks/g' $(PWD)/extlinux.conf.temp;\
		sed -i 's/MENU\sLABEL\sjetclocks/MENU LABEL primary/g' $(PWD)/extlinux.conf.temp;\
		sed -i '/FDT/d' $(PWD)/extlinux.conf.temp;\
		sed -i '/LINUX/a\      FDT /boot/dtb/jetclocks.dtb' $(PWD)/extlinux.conf.temp;\
		echo "`cat $(PWD)/extlinux.conf.temp`" >> /boot/extlinux/$(EXTFILE);\
		sed -i 's/DEFAULT\sprimary/DEFAULT jetclocks/g' /boot/extlinux/$(EXTFILE);\
		rm -f $(PWD)/extlinux.conf.temp;\
		echo "All done, you should reboot your machine now";\
	fi

uninstall:
	$(eval JETFILE := $(shell find /lib/modules/$(uname -r) -name "jetclocks.ko"))
	@rm -f $(JETFILE)
	@rm -f /etc/modules-load.d/jetclocks.conf
	@echo "$(JETFILE) removed"	
	@echo "/etc/modules-load.d/jetclocks.conf removed"
	depmod -a
	$(eval EXTFILE := $(shell find /boot/extlinux -name "*jetclocks.backup" -exec basename {} \;))
	@if [ "$(EXTFILE)" != "" ]; then\
		cp /boot/extlinux/$(EXTFILE) /boot/extlinux/extlinux.conf;\
		echo "/boot/extlinux/$(EXTLIFE) restored to /boot/extlinux/extlinux.conf";\
	fi
	$(eval DTBFILE := $(shell find /boot/dtb -name "jetclocks.dtb"  -exec basename {} \;))
	@if [ "$(EXTFILE)" != "" ]; then\
		rm -f /boot/dtb/$(DTBFILE);\
		echo "/boot/dtb/$(DTBFILE) removed";\
	fi
