#!/usr/bin/env python
# multipleImages.py - Display image in a window.
# (c) Kimmo Karvinen & Tero Karvinen http://BotBook.com
 
import gtk, os
 
dir="data"
pixbufs=[]
image=None
pos=0
 
def loadImages():
    for file in os.listdir(dir):
        filePath=os.path.join(dir, file)
        pix=gtk.gdk.pixbuf_new_from_file(filePath)
        pixbufs.append(pix)
        print("Loaded image "+filePath)
 
def keyEvent(widget, event):
    global pos
    key = gtk.gdk.keyval_name(event.keyval)
    if key=="space" or key=="Page_Down":
        pos+=1
        image.set_from_pixbuf(pixbufs[pos])
    elif key=="b" or key=="Page_Up":
        pos-=1
        image.set_from_pixbuf(pixbufs[pos])
    else:
        print("Key "+key+" was pressed")
 
def main():
    global image
    window = gtk.Window()
    window.connect("destroy", gtk.main_quit)
    window.connect("key-press-event", keyEvent)
    image=gtk.Image()
    window.add(image)
    loadImages()
    image.set_from_pixbuf(pixbufs[pos])
 
    window.show_all()
    gtk.main()
 
if __name__=="__main__":
    main()
