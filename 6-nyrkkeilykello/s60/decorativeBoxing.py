# decorativeBoxing.py - Boxing clock with decorations. 
# (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi 
import e32, audio, appuifw
import graphics, time, math
 
def breakStart():
	global round, startTime
	round=False
	startTime=time.clock()
	drawUI()
	blitCenterAt(img, clockBreak, h/2)
	breakSound.play()
	blingTimer.cancel()
	blingTimer.after(breakLen, fightStart)
 
def fightStart():
	global round, startTime, roundsStarted
	roundsStarted+=1
	round=True
	startTime=time.clock()
	drawUI()
	blitCenterAt(img, clockFight, h/2)
	fightSound.play()
	blingTimer.cancel()
	blingTimer.after(roundLen, breakStart)
 
def setRoundLength():
	global roundLen
	roundLen=appuifw.query(u"Round lenght in seconds", "number", 
		roundLen)
	breakStart()
 
def setBreakLength():
	global breakLen
	breakLen=appuifw.query(u"Break lenght in seconds", "number", 
		breakLen)
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
		roundOrBreak="round"
	else: 
		t=breakLen
		roundOrBreak="break"
	percent=(time.clock()-startTime)/t
	#print "passed", time.clock()-startTime, "t", t, "percent", percent
	x=w/2+r*math.cos( (percent-0.25) *2*math.pi )
	y=h/2+r*math.sin( (percent-0.25) *2*math.pi )
	img.line((w/2, h/2, x, y), outline=pieColor, width=10)
	canvas.blit(img)
	e32.reset_inactivity() # disable screensaver
 
	elapsed=time.clock()-trainingStarted # seconds, floating point
	min = int(elapsed/float(60))
	sec = int(elapsed-min*60)
	msg=u"%d:%02d elapsed, %s %d" % (min, sec, roundOrBreak, roundsStarted)
	img.rectangle( (w/2-120,msgY-20, w/2+120,msgY+5), fill=black)
	img.text( (msgX,msgY), msg, bcRed )
 
	pieTimer.cancel()
	pieTimer.after(0.5, updatePie)
 
def blitTopCenter(background, thing):
	"Center thing on the top of the background. "
	y=0
	(objW, objH)=thing.size
	(bacW, bacH)=background.size
	x=bacW/2-objW/2
	background.blit(thing, target=(x,y))
 
def blitBottomCenter(background, thing):
	"Put image 'thing' on the bottom (y) and center (x) of image 'background'. "
	(objW, objH)=thing.size
	(bacW, bacH)=background.size
	# Calculate top left corner of "thing" when centered. 
	x=bacW/2-objW/2
	y=bacH-objH
	background.blit(thing, target=(x,y))
 
def blitCenterAt(background, thing, y):
	"Place center of thing to given y coordinate. "
	(objW, objH)=thing.size
	(bacW, bacH)=background.size
	x=bacW/2-objW/2
	y=y-objW/2 # change y from thing center to thing top border
	background.blit(thing, target=(x,y))	
 
def drawUI():
	img.clear(black)
	blitTopCenter(img, title)
	blitBottomCenter(img, softkeys)
 
# main
roundLen=60*2 # 60*2 # seconds
breakLen=60
 
fightSound=audio.Sound.open("e:\\python\\boxingClock\\fight.mp3")
breakSound=audio.Sound.open("e:\\python\\boxingClock\\break.mp3")
title=graphics.Image.open("e:\\python\\boxingClock\\title.png")
clockFight=graphics.Image.open("e:\\python\\boxingClock\\clockFight.jpg")
clockBreak=graphics.Image.open("e:\\python\\boxingClock\\clockBreak.jpg")
softkeys=graphics.Image.open("e:\\python\\boxingClock\\softkeys.png")
 
blingTimer = e32.Ao_timer()
 
appuifw.app.title = u"Boxing Clock  -  Sulautetut.fi"
appuifw.app.menu = [
  (u"Round length", setRoundLength), 
  (u"Break length", setBreakLength)
]
appuifw.app.exit_key_handler = quit
appuifw.app.screen='full'
 
black = 0x000000
white = 0xffFFff
red   = 0xff0000
green = 0x00ff00
blue  = 0x0000ff
bcRed = 0xff3900
pieColor=white
 
img=graphics.Image.new((1, 1))
canvas=appuifw.Canvas(redraw_callback=redraw)
appuifw.app.body=canvas
w, h = canvas.size
img=graphics.Image.new((w, h))
img.clear(green)
round=False
 
r=50	# ray of pie, pixels
 
msgY=h-softkeys.size[1]-5	# bottom of text
msgX=w/2-80
 
pieTimer = e32.Ao_timer()
startTime=0
trainingStarted=time.clock()
roundsStarted=0
 
breakStart() 
updatePie()
 
appLock = e32.Ao_lock()
appLock.wait() # main loop
