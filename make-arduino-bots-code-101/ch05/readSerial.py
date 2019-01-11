#!/usr/bin/env python
# readSerial.py - Read serial port and print to screen
# (c) Kimmo Karvinen & Tero Karvinen http://BotBook.com

import serial, sys
 
# File name will be different if you are on Windows or Mac OS X
ser = serial.Serial("/dev/ttyUSB1", 9600)
 
if (ser):
    print("Serial port " + ser.portstr + " opened.")
 
while True:
    sys.stdout.write (ser.read(1))
    sys.stdout.flush()
