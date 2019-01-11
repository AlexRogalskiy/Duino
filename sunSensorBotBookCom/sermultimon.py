#!/usr/bin/env python
# sermultimon - monitor multiple serial ports
# Copyright 2013 http://botbook.com/satellite Tero Karvinen & Kimmo Karvinen

import serial
from time import sleep

def green(s):
	return "\033[92m" + s + "\033[0m"

def blue(s):
	return "\033[94m" + s + "\033[0m"

def serReadLine(ser):
	s = ser.readline(255)
	s = s.replace('\n', '');
	if len(s) > 0:
		return s
	else:
		return None

def main():
	ser0 = serial.Serial("/dev/ttyACM0", 115200, timeout=0)
	ser1 = serial.Serial("/dev/ttyACM1", 115200, timeout=0)

	while True:
		s = serReadLine(ser0)
		if s: print green("0: "+s)
		
		s = serReadLine(ser1)
		if s: print blue("1: "+s)
		
		sleep(0.1) # s

if __name__ == '__main__':
	main()


