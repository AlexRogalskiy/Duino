# lm35.py - print temperature in Celcius
# (c) BotBook.com - Karvinen, Karvinen, Valtokari


import time	# <1>
import botbook_mcp3002 as mcp	# <2>

def readTemperature():	# <3>
	data = mcp.readAnalog(0,0)	# <4>
	percent = data / 1023.0	# <5>
	volts = percent * 3.3	# <6>
	celcius = 100.0 * volts	# <7>
	return celcius	# <8>

while True:	# <9>
	t = readTemperature()	# <10>
	print("Current temperature is %i C " % t)	# <11>
	time.sleep(0.5) # seconds	# <12>

