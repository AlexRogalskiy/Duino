# hc-sr04.py - measure distance with ultrasound
# (c) BotBook.com - Karvinen, Karvinen, Valtokari


import time	# <1>
import botbook_gpio as gpio	# <2>

def readDistanceCm():	# <3>
	triggerPin = 22	# <4>
	echoPin = 27	# <5>

	v=(331.5+0.6*20) # m/s	# <6>

	gpio.mode(triggerPin,"out")	# <7>

	gpio.mode(echoPin,"in")	# <8>
	gpio.interruptMode(echoPin, "both")	# <9>

	gpio.write(triggerPin, 0)	# <10>
	time.sleep(0.5)	# <11>

	gpio.write(triggerPin, 1)	# <12>
	time.sleep(1/1000.0/1000.0)	# <13>
	gpio.write(triggerPin, 0)	# <14>

	t = gpio.pulseInHigh(echoPin) # s	# <15>

	d = t*v	# <16>
	d = d/2	# <17>
	return d*100 # cm	# <18>

dist = readDistanceCm()	# <19>
print("Distance is %i cm" % dist)	# <20>


