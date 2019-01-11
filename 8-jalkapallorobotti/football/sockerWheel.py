# sockerWheel.py - control soccerbot with n95 acceleration
# (c) Karvinen

import socket, e32, time, axyz, appuifw, graphics

btAddress='00:07:80:83:ab:6a'
gui=True
bg=graphics.Image.open("e:\\python\\blueServo\\bgGreen.jpg")

canvas=None
sock=False
counter=0
prevSpeed=0
appLock = e32.Ao_lock()

def suSend(left, right, kick="-"):
	print("left: "+str(left)+"	right:"+str(right))
	cmd="S"+chr(left+60)+chr(right+60)+kick+"U"
	sock.send(cmd)

def scale(x, old, new):
	(oldMin, oldMax) = old
	(newMin, newMax) = new
	p=float(x-oldMin)/(oldMax-oldMin) # x percentage of old scale
	y=(newMax-newMin)*p+newMin
	y=constrain(y, -1, 1)
	return y

def constrain(x, min, max):
	if x<min: return min
	elif max<x: return max
	else: return x

def readAcc(x, y, z):
	global counter, prevSpeed
	if counter>=10:
		left=scale(y, (-50, 50), (-1, 1)) 
		right=-left
		speed=scale(-z, (-50, 50), (-1, 1))
		left+=speed
		right+=speed
		left=constrain(left, -1, 1)
		right=constrain(right, -1, 1)
		if left+right<0:
			tmp=left
			left=right
			right=tmp
		if (abs(left)+abs(right))/2<0.15:
			left=0
			right=0
		#if prevSpeed<-0.1 and speed>0.8:
		if abs(prevSpeed-speed)>0.7:
			kick="k"
			print("Kick!")
			counter=-3000
		else:
			kick="-"
		suSend(left*60, right*60, kick)
		prevSpeed=speed
		counter=0
	counter+=1

def quit():
	suSend(0, 0)
	sock.close()
	axyz.disconnect()
	appLock.signal()
	print("Closed socket. Bye bye, visit sulautetut.fi!")

def redraw(dummy=None):
	canvas.blit(bg)

def main():
	global sock, canvas
	print("Started.")
	appuifw.app.exit_key_handler = quit

	sock=socket.socket(socket.AF_BT, socket.SOCK_STREAM)
	sock.connect((btAddress, 1))
	print("Connected")
	
	axyz.connect(readAcc)
	print("Axyz connected.")

	appuifw.app.screen='full'
	canvas = appuifw.Canvas(redraw_callback=redraw)
	appuifw.app.body=canvas
	redraw()

	print("Entering main loop")
	appLock.wait()

if __name__== '__main__' :
	main()
	print("Program finnished")
