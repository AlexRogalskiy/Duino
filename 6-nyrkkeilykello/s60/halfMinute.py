# halfMinute.py - Print "Bling" every half minute
# (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi
import e32
 
def bling():
	print("Bling!")
	blingTimer.cancel()
	blingTimer.after(30, bling)
 
# main
blingTimer = e32.Ao_timer()
bling()
