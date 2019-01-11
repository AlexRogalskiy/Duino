# blueNumber.py - Send numbers to bluetooth serial to control robot. 
# (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi
 
import appuifw, e32, key_codes, socket
 
btDeviceAddress='00:07:80:83:ab:6a' # Arduino BT. 'hcitool scan'
 
def quit():
	sock.close()
	appuifw.note(u"Bye! See you at www.sulautetut.fi!")
	appLock.signal()
 
## Main
 
canvas = appuifw.Canvas()
appuifw.app.body = canvas
 
canvas.bind(key_codes.EKey4, lambda: sock.send("4") )
canvas.bind(key_codes.EKey5, lambda: sock.send("5") )
canvas.bind(key_codes.EKey6, lambda: sock.send("6") )
# mobile phone number pad is upside down computer numpad:
canvas.bind(key_codes.EKey8, lambda: sock.send("2") ) # back 8-2
canvas.bind(key_codes.EKey2, lambda: sock.send("8") ) # forward 2-8
 
sock=socket.socket(socket.AF_BT, socket.SOCK_STREAM)
sock.connect((btDeviceAddress, 1)) # extra parenthesis for tuple
appuifw.note(u"Use 2468 to move, 5 to kick. See you at www.sulautetut.fi!")
 
appuifw.app.exit_key_handler = quit
appLock = e32.Ao_lock()
appLock.wait()
