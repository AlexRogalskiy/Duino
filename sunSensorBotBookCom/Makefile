# Makefile for Prototype of Sun Sensor for Finland's first satellite. 
# Copyright 2013 http://botbook.com/satellite Tero Karvinen & Kimmo Karvinen

all: sunsensor/.build/uno/firmware.hex obc/.build/uno/firmware.hex monitor

monitor:
	./sermultimon.py

sunsensor/.build/uno/firmware.hex: sunsensor/src/sunsensor.ino
	@echo -e "\n\n### sunsensor (i2c slave) ###\n"
	cd sunsensor && ino build && ino upload -p /dev/ttyACM1

obc/.build/uno/firmware.hex: obc/src/obc.ino
	@echo -e "\n\n### Master ###\n"
	cd obc && ino build && ino upload -p /dev/ttyACM0

clean: 
	cd obc && ino clean
	cd sunsensor && ino clean
	cd blinky && ino clean
	cd e910.86-arduino && ino clean

blink:
	@echo "Uploading blink to erase Arduinos (obc and sunsensor)"
	cd blinky && ino build
	@echo -e "\n\n### Master blink ###\n"
	cd blinky && ino upload -p /dev/ttyACM0
	@echo -e "\n\n### Slave blink ###\n"
	cd blinky && ino upload -p /dev/ttyACM1

release: clean
	# create a tarball with just release contents
	tar -xf --exclude-backups --exclude-caches-all .

checkupdates:
	@echo "Checking for updates..."
	wget -qo - 'http://botbook.com/updates?n=sunsensor&v=0.4.0'

e910: blink
	@echo "### Hello e910.86 Sun Sensor on Arduino ###"
	cd e910.86-arduino && ino build && ino upload -p /dev/ttyACM1
	./sermultimon.py


