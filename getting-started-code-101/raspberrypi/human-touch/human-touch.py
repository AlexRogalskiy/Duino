# human-touch.py - detect human touch with Keyes capacitive touch sensor
# (c) BotBook.com - Karvinen, Karvinen, Valtokari


import time	# <1>
import tgpio	# <2>

while True:	# <3>
	tgpio.mode(25, "in")	# <4>
	touch_detected = tgpio.read(25)	# <5>
	if( touch_detected == 1 ):	# <6>
		print("Touch detected")	# <7>
	else:	# <8>
		print("No touch detected")	# <9>
	time.sleep(0.5)	# <10>

