#!/usr/bin/env python
# windowHello.py - Create a window with a button.  
# (c) Kimmo Karvinen & Tero Karvinen http://BotBook.com
import gtk
 
window = gtk.Window()
window.connect("destroy", gtk.main_quit)
 
button = gtk.Button("Hello PyGTK - BotBook.com")
window.add(button)
 
window.show_all()
 
gtk.main()
