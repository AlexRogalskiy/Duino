#!/bin/bash
# Copyright 2013 Tero Karvinen & Kimmo Karvinen http://botbook.com/satellite

echo "About to install command line development tools for xUbuntu Linux. "
echo "Installer will automatically check for updates and news over internet. "
echo "More information in http://botbook.com/satellite"
echo "Press enter to continue or ctrl-C to quit. "
echo -n "[enter]"
read

sudo apt-get update
sudo apt-get -y install arduino gedit python-serial gdebi make git rsync screen tree
wget -qo - 'http://botbook.com/updates?n=sunsensor-cli-devtools&v=0.4.0'
wget http://botbook.com/satellite/download/python-ino_0.3.5-1_all.deb
sudo gdebi -n python-ino_*.deb
find -iname '*.ino' -exec touch "{}" \;
sudo adduser $(whoami) dialout

echo 
echo "****************************************************************"
echo "* Development environment install: done.                       *"
echo "* Build the physical device. See http://botbook.com/satellite  *"
echo "* Then run 'make' in this shell.                               *"
echo "****************************************************************"
echo 

newgrp dialout

