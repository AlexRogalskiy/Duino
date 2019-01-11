#!/usr/bin/env python
# gtkserial.py - Read serial port with GTK.
# (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi
import serial, gtk, gobject, sys
 
# File name might differ between Linux and Windows
ser = serial.Serial("/dev/ttyUSB1", 115200)
 
def pollSerial():
	sys.stdout.write(ser.read(1))
	return True
 
if (ser):
	print("Serial port " + ser.portstr + " opened.")
	gobject.timeout_add(100, pollSerial)
 
gtk.main()
