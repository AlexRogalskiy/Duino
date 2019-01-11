#!/usr/bin/env python
# fullScreenScale.py - Show image scaled to full screen
# (c) Kimmo Karvinen & Tero Karvinen http://BotBook.com
 
import gtk, os
 
def fitRect(thing, box):
    # scale
    scaleX=float(box.width)/thing.width
    scaleY=float(box.height)/thing.height
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
 
def main(): bs
    pix=gtk.gdk.pixbuf_new_from_file(os.path.join("data", "image1.jpg"))
    window = gtk.Window()
    window.connect("destroy", gtk.main_quit)
    window.fullscreen()
 
    bg=newPix(gtk.gdk.screen_width(), gtk.gdk.screen_height())
    pixFitted=scaleToBg(pix, bg)

    image=gtk.image_new_from_pixbuf(pixFitted)
    window.add(image)
    window.show_all()
    gtk.main()
 
if __name__ == "__main__":
    main()
