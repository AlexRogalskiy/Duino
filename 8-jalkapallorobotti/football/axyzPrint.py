# axyzPrint.py - Print acceleration from N95 accelerometer
# (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi
 
import appuifw, e32, axyz
 
def readAcc(x, y, z):
	print x, y, z
 
def quit():
	print("Bye... visit sulautetut.fi")
	axyz.disconnect()
	app_lock.signal()
 
axyz.connect(readAcc)
 
appuifw.app.title = u"Print Acceleration - Sulautetut.fi"
appuifw.app.exit_key_handler = quit
app_lock = e32.Ao_lock()
app_lock.wait()

