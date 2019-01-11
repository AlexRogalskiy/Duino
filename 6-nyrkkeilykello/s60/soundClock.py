# soundClock.py - Play a sound every half minute
# (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi
import e32
import audio
 
def bling():
	print("Bling!")
	fightSound.play()
	blingTimer.cancel()
	blingTimer.after(30, bling)
 
# main
fightSound=audio.Sound.open("e:\\python\\boxingClock\\fight.mp3")
blingTimer = e32.Ao_timer()
bling()
