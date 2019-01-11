#!/usr/bin/env python
# readSerial.py - Read serial port and print to screen
# (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi

import serial, sys
 
# File name might differ between Linux and Windows
ser = serial.Serial("/dev/ttyUSB1", 115200)
 
if (ser):
	print("Serial port " + ser.portstr + " opened.")
 
while True:
	sys.stdout.write(ser.read(1))
