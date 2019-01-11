# pot_once.py - measure resistance of a potentiometer, once
# (c) BotBook.com - Karvinen, Karvinen, Valtokari

import botbook_mcp3002 as mcp	# <1>

x = mcp.readAnalog(0,0)	# <2>
print(x)	# <3>

