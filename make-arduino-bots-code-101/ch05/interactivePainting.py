#!/usr/bin/env python
# interactivePainting.py - Choose full screen image by waving hand. 
# (c) Kimmo Karvinen & Tero Karvinen http://BotBook.com

import gtk, os, serial, gobject

# Global variables

dir="data"
pixbufs=[]
image=None
bg=None
pos=0
ser=None
reel=None
x=0
w=0
speed=0

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

def catenate(left, right): 
    "Return a Pixbuf with 'right' catenated on the right side of 'left'. "
    assert left.get_width()==right.get_width() 
    assert left.get_height()==right.get_height()
    reel=newPix(left.get_width()+right.get_width(), left.get_height()) 
    left.copy_area( 
        src_x=0, src_y=0,
        width=left.get_width(), height=right.get_height(), 
        dest_pixbuf=reel, 
        dest_x=0, dest_y=0
    )
    right.copy_area(
        src_x=0, src_y=0,
        width=right.get_width(), height=right.get_height(), 
        dest_pixbuf=reel, 
        dest_x=left.get_width(), dest_y=0
    )
    return reel 

def getBox(pix, x, width): 
    "Return Pixbuf, a slice of pix, starting at x, given width. "
    buf=newPix(width, pix.get_height())
    pix.copy_area(
        src_x=x, src_y=0, 
        width=width, height=pix.get_height(), 
        dest_pixbuf=buf, dest_x=0, dest_y=0)
    return buf

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
    global pos, reel, x, speed
    last=len(pixbufs)-1
    if pos<0:
        pos=last
    elif pos>last:
        pos=0
    
    if 0<relativePos:
        print("Next")
        if pos==last: 
            right=0
        else:
            right=pos+1 
        reel=catenate(pixbufs[pos], pixbufs[right])
        x=0 
        speed=60
    if relativePos<0:
        print("prev")
        if pos==0:
            left=last
        else:
            left=pos-1
        reel=catenate(pixbufs[left], pixbufs[pos])
        x=w
        speed=-60
    print("pos == "+str(pos))
    pos+=relativePos 

def animateSlide():
    global reel, x, speed
    if speed!=0: 
        x+=speed
        if x>=w or x<=0: 
            speed=0
        print x, reel
        pix=getBox(reel, x, w) 
        image.set_from_pixbuf(pix) 
    return True

def keyEvent(widget, event):
    global pos, image
    key = gtk.gdk.keyval_name(event.keyval)
    if key=="space" or key=="Page_Down":
        go(1)
    elif key=="b" or key=="Page_Up":
        go(-1)
    elif key=="q" or key=="F5" or key=="ESC":
        gtk.main_quit()
    else:
        print("Key "+key+" was pressed")

def pollSerial():
    if ser.inWaiting()<=0:
        #print("No data waiting in serial buffer.")
        return True # call again later
    cmd=ser.read(size=1)
    print("Serial port read: \"%s\"" % cmd)
    if cmd=="F":
        go(1)
    elif cmd=="B":
        go(-1)
    return True

# Main 

def main():
    global bg, image, ser, w
    
    w=gtk.gdk.screen_width()
    h=gtk.gdk.screen_height()
    window = gtk.Window()
    window.connect("destroy", gtk.main_quit)
    window.connect("key-press-event", keyEvent)
    window.fullscreen()
    bg=newPix(w, h)
    loadImages()
    image=gtk.image_new_from_pixbuf(pixbufs[pos])
    
    ser = serial.Serial('/dev/ttyUSB0’, 9600, timeout=0)
    gobject.timeout_add(100, pollSerial)
    
    gobject.timeout_add(30, animateSlide) 
    
    window.add(image)
    window.show_all()
    gtk.main()

if __name__ == "__main__":
    main()