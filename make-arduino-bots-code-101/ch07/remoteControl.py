#!/usr/bin/env python
# remoteControl.py - Graphical user interface for remote control. 
# (c) Kimmo Karvinen & Tero Karvinen http://BotBook.com
 
import serial 
import gtk
 
ser=None    # global variable 
 
def sendSerial(widget, ch): 
    global ser 
    print("Sending "+ch) 
    ser.write(ch) 
 
def main(): 
    global ser 
    # File name might differ between Linux, Windows, and Mac OS X 
    ser = serial.Serial('/dev/ttyUSB0', 9600)
 
    if (ser): 
        print("Serial port " + ser.portstr + " opened.")
 
    window = gtk.Window(gtk.WINDOW_TOPLEVEL) 
    window.connect("destroy", gtk.main_quit) 
 
    vbox=gtk.VBox() 
    window.add(vbox)
    row1=gtk.HBox() 
    off1 = gtk.Button("1 off") 
    off1.connect("clicked", sendSerial, "2") 
    row1.pack_start(off1) 
    vbox.pack_start(row1)
    on1 = gtk.Button("1 on") 
    on1.connect("clicked", sendSerial, "8")
    row1.pack_start(on1)
 
    row2=gtk.HBox() 
    vbox.pack_start(row2) # vertical box starts at the bottom
    off2 = gtk.Button("2 off")
    off2.connect("clicked", sendSerial, "3")
    row2.pack_start(off2)
    on2 = gtk.Button("2 on")
    on2.connect("clicked", sendSerial, "7")
    row2.pack_start(on2)
 
    row3=gtk.HBox()
    vbox.pack_start(row3)
    off3 = gtk.Button("3 off")
    off3.connect("clicked", sendSerial, "4")
    row3.pack_start(off3)
    on3 = gtk.Button("3 on")
    on3.connect("clicked", sendSerial, "6")
    row3.pack_start(on3)
 
    window.show_all() 
    gtk.main() 
    print ("Thanks for using BotBook.com remote control.")
 
if __name__ == "__main__": 
    main()