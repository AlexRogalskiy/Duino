#!/usr/bin/env python
# packingVBox.py - Pack many widgets on top of each other.  
# (c) Kimmo Karvinen & Tero Karvinen http://BotBook.com
 
import gtk
 
window=gtk.Window()
window.connect("destroy", gtk.main_quit)
 
vbox=gtk.VBox() 
window.add(vbox) 
 
b1=gtk.Button("Button 1") 
vbox.pack_start(b1) 
 
b2=gtk.Button("Button 2") 
vbox.pack_start(b2)
 
window.show_all()
gtk.main()