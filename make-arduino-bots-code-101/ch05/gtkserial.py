#!/usr/bin/env python
# gtkserial.py - Read serial port with GTK.
# (c) Kimmo Karvinen & Tero Karvinen http://BotBook.com
import serial, gtk, gobject, sys
 
# File name will be different if you are on Windows or Mac OS X
ser = serial.Serial('/dev/ttyUSB1', 9600)
 
def pollSerial():
    sys.stdout.write(ser.read(1))
    sys.stdout.flush()
    return True
 
if (ser):
    print("Serial port " + ser.portstr + " opened.")
    gobject.timeout_add(100, pollSerial)
 
gtk.main()