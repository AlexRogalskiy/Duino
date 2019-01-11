#!/usr/bin/env python
# boxInsideBox.py - Combine boxes to lay out elements 
# in rows and columns. 
# (c) Kimmo Karvinen & Tero Karvinen http://sulautetut.fi
 
import gtk
 
window=gtk.Window()
window.connect("destroy", gtk.main_quit)
 
vbox=gtk.VBox()
window.add(vbox)
 
b1=gtk.Button("Button 1")
vbox.pack_start(b1)
 
hbox=gtk.HBox()
vbox.pack_start(hbox)
 
b2=gtk.Button("Button 2")
hbox.pack_start(b2)
 
b3=gtk.Button("Button 3")
hbox.pack_start(b3)
 
window.show_all()
gtk.main()
