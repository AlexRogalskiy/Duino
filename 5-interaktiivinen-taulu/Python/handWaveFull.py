#!/usr/bin/env python
# handWaveFull.py - Choose full screen image by waving hand. 
# (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi
 
import gtk, os, serial, gobject
 
# Global variables
 
dir="data"
pixbufs=[]
image=None
bg=None
pos=0
ser=None
 
# Pixbuf manipulation
 
def fitRect(thing, box):
	# scale
	scaleY=float(box.height)/thing.height
	scaleX=float(box.width)/thing.width
	scale=min(scaleY, scaleX)
	thing.width=scale*thing.width
	thing.height=scale*thing.height
	# center
	thing.x=box.width/2-thing.width/2
	thing.y=box.height/2-thing.height/2
	return thing
 
def scaleToBg(pix, bg):
	fit=fitRect(
		gtk.gdk.Rectangle(0,0, pix.get_width(), pix.get_height()),
		gtk.gdk.Rectangle(0,0, bg.get_width(), bg.get_height())
	)
	scaled=pix.scale_simple(fit.width, fit.height, gtk.gdk.INTERP_BILINEAR)
	ret=bg.copy()
	scaled.copy_area(
		src_x=0, src_y=0,
		width=fit.width, height=fit.height, 
		dest_pixbuf=ret, 
		dest_x=fit.x, dest_y=fit.y
	)
	return ret
 
def newPix(width, height, color=0x000000ff):
	pix=gtk.gdk.Pixbuf(gtk.gdk.COLORSPACE_RGB, True, 8, width , height)
	pix.fill(color)
	return pix
 
# File reading
 
def loadImages():
	global pixbufs
	for file in os.listdir(dir):
		filePath=os.path.join(dir, file)
		pix=gtk.gdk.pixbuf_new_from_file(filePath)
		pix=scaleToBg(pix, bg)
		pixbufs.append(pix)
		print("Loaded image "+filePath)
 
# Controls
 
def go(relativePos):
	global pos
	pos+=relativePos
 
	last=len(pixbufs)-1
	if pos<0:
		pos=last
	elif pos>last:
		pos=0
 
	image.set_from_pixbuf(pixbufs[pos])
 
def keyEvent(widget, event):
	global pos, image
	key = gtk.gdk.keyval_name(event.keyval)
	if key=="space" or key=="Page_Down":
		go(1)
	elif key=="b" or key=="Page_Up":
		go(-1)
	elif key=="q" or key=="F5":
		gtk.main_quit()
	else:
		print("Key "+key+" was pressed")
 
def pollSerial():
	cmd=ser.read(size=1)
	print("Serial port read: \"%s\"" % cmd)
	if cmd=="F":
		go(1)
	elif cmd=="B":
		go(-1)
	return True
 
# Main 
 
def main():
	global bg, image, ser
	bg=newPix(gtk.gdk.screen_width(), gtk.gdk.screen_height())
	loadImages()
	image=gtk.image_new_from_pixbuf(pixbufs[pos])
 
	ser = serial.Serial('/dev/ttyUSB1', 115200)
	gobject.timeout_add(100, pollSerial)
 
	window = gtk.Window()
	window.connect("destroy", gtk.main_quit)
	window.connect("key-press-event", keyEvent)
	window.fullscreen()
	window.add(image)
	window.show_all()
	gtk.main()
 
if __name__ == "__main__":
	main()
