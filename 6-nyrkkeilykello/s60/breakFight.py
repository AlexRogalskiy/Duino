# breakFight.py - Indicate 1 min break and 2 min round with sound. 
# (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi
import e32
import audio
 
def breakStart():
	print("bling bling - Break!")
	breakSound.play()
	blingTimer.cancel()
	blingTimer.after(60, fightStart)
 
def fightStart():
	print("bling - Fight!")
	fightSound.play()
	blingTimer.cancel()
	blingTimer.after(60*2, breakStart)
 
# main
fightSound=audio.Sound.open("e:\\python\\boxingClock\\fight.mp3")
breakSound=audio.Sound.open("e:\\python\\boxingClock\\break.mp3")
 
blingTimer = e32.Ao_timer()
breakStart()

