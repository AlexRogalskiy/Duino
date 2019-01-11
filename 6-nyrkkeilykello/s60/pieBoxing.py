# pieBoxing.py - Boxing clock with time pie. 
# (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi 
import e32, audio, appuifw
import graphics, time, math
 
def breakStart():
	global round, startTime
	round=False
	startTime=time.clock()
	img.clear(green)
	breakSound.play()
	blingTimer.cancel()
	blingTimer.after(breakLen, fightStart)
 
def fightStart():
	global round, startTime
	round=True
	startTime=time.clock()
	img.clear(red)
	fightSound.play()
	blingTimer.cancel()
	blingTimer.after(roundLen, breakStart)
 
def setRoundLength():
	global roundLen
	roundLen=appuifw.query(u"Round lenght in seconds", "number", roundLen)
	breakStart()
 
def setBreakLength():
	global breakLen
	breakLen=appuifw.query(u"Break lenght in seconds", "number", breakLen)
	breakStart()
 
def quit():
	blingTimer.cancel()
	#updateTimer.cancel()
	print("Bye! Visit http://sulautetut.fi")
	appLock.signal()
 
def redraw(dummy=None):
	canvas.blit(img)
 
def updatePie():
	if round: 
		t=roundLen
	else: 
		t=breakLen
	percent=(time.clock()-startTime)/t
	#print "passed", time.clock()-startTime, "t", t, "percent", percent
	x=w/2+r*math.cos( (percent-0.25) *2*math.pi )
	y=h/2+r*math.sin( (percent-0.25) *2*math.pi )
	img.line((w/2, h/2, x, y), outline=pieColor, width=10)
	canvas.blit(img)
	e32.reset_inactivity() # disable screensaver
	pieTimer.cancel()
	pieTimer.after(0.5, updatePie)
 
# main
roundLen=60*2 # 60*2 # seconds
breakLen=60
 
fightSound=audio.Sound.open("e:\\python\\boxingClock\\fight.mp3")
breakSound=audio.Sound.open("e:\\python\\boxingClock\\break.mp3")
 
blingTimer = e32.Ao_timer()
 
appuifw.app.title = u"Boxing Clock  -  Sulautetut.fi"
appuifw.app.menu = [
  (u"Round length", setRoundLength), 
  (u"Break length", setBreakLength)
]
appuifw.app.exit_key_handler = quit
 
black = 0x000000
white = 0xffFFff
red   = 0xff0000
green = 0x00ff00
blue  = 0x0000ff
pieColor=white
 
img=graphics.Image.new((1, 1))
canvas=appuifw.Canvas(redraw_callback=redraw)
appuifw.app.body=canvas
w, h = canvas.size
img=graphics.Image.new((w, h))
img.clear(green)
round=False
 
r=50	# ray of pie, pixels
 
pieTimer = e32.Ao_timer()
startTime=0
 
breakStart() 
updatePie()
 
appLock = e32.Ao_lock()
appLock.wait() # main loop
