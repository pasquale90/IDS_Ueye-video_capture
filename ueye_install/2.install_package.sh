#!/bin/bash

#download debian package
wget https://en.ids-imaging.com/files/downloads/ids-software-suite/software/linux-desktop/ids-software-suite-linux-64-4.95.1-debian.zip
unzip ids-software-suite-linux-64-4.95.1-debian.zip
rm ids-software-suite-linux-64-4.95.1-debian.zip

cd ids-software-suite-linux-64-4.95.1-debian

#install packages
sudo dpkg -i ueye-common_4.95.1.1722_amd64.deb
sudo dpkg -i ueye-manual-en_4.95.1.1722_amd64.deb
sudo dpkg -i ueye-manual-de_4.95.1.1722_amd64.deb
sudo dpkg -i ueye-manuals_4.95.1.1722_amd64.deb
sudo dpkg -i ueye-driver-usb_4.95.1.1722_amd64.deb
sudo dpkg -i ueye-driver-eth_4.95.1.1722_amd64.deb
sudo dpkg -i ueye-drivers_4.95.1.1722_amd64.deb
sudo dpkg -i ueye-dev_4.95.1.1722_amd64.deb
sudo dpkg -i ueye-tools-cli_4.95.1.1722_amd64.deb
sudo dpkg -i ueye-demos_4.95.1.1722_amd64.deb
sudo dpkg -i ueye-tools-qt5_4.95.1.1722_amd64.deb
sudo dpkg -i ueye_4.95.1.1722_amd64.deb

rm -r ./*deb
