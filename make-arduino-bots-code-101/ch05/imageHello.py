# imageHello.py - Display image in a window.
# (c) Kimmo Karvinen & Tero Karvinen http://BotBook.com
 
import gtk, os
 
window = gtk.Window()
window.connect("destroy", gtk.main_quit)
 
image=gtk.Image()
window.add(image)
image.set_from_file(os.path.join("data", "image1.jpg"))
 
window.show_all()
gtk.main()
