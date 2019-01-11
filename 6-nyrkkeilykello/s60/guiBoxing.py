# guiBoxing.py - Boxing clock with menu, title and proper quit. 
# (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi 
import e32
import audio
import appuifw
 
def breakStart():
	print("bling bling - Break!")
	breakSound.play()
	blingTimer.cancel()
	blingTimer.after(breakLen, fightStart)
 
def fightStart():
	print("bling - Fight!")
	fightSound.play()
	blingTimer.cancel()
	blingTimer.after(roundLen, breakStart)
 
def setRoundLength():
	global roundLen
	roundLen=appuifw.query(u"Round lenght in seconds", "number", roundLen)
 
def setBreakLength():
	global breakLen
	breakLen=appuifw.query(u"Break lenght in seconds", "number", breakLen)
 
def quit():
	blingTimer.cancel()
	print("Bye! Visit http://sulautetut.fi")
	appLock.signal()
 
# main
roundLen=60*2 # seconds
breakLen=60
 
fightSound=audio.Sound.open("e:\\python\\boxingClock\\fight.mp3")
breakSound=audio.Sound.open("e:\\python\\boxingClock\\break.mp3")
 
blingTimer = e32.Ao_timer()
 
appuifw.app.title = u"Boxing Clock - Sulautetut.fi"
appuifw.app.menu = [
  (u"Round length", setRoundLength), 
  (u"Break length", setBreakLength)
]
appuifw.app.exit_key_handler = quit
 
breakStart() 
 
appLock = e32.Ao_lock()
appLock.wait() # main loop
