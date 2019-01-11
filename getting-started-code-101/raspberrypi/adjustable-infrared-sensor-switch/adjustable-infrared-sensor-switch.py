# adjustable-infrared-sensor-switch.py - is object within predefined distance?
# (c) BotBook.com - Karvinen, Karvinen, Valtokari


import botbook_gpio as gpio	# <1>

gpio.mode(27, "in")	# <2>
x = gpio.read(27)	# <3>
if( x == 0 ):	# <4>
	print "Something is inside detection range"	# <5>
else:	# <6>
	print "There is nothing inside detection range"	# <7>


