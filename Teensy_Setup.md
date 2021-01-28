# How To Install Teensy Installer on Linux (Ubuntu 18)

## [Teensy First Use](https://www.pjrc.com/teensy/first_use.html)

## [Teensy Loader](https://www.pjrc.com/teensy/loader_linux.html)

1. Download the Teensy Program (64 bit) to Downloads
1. Unzip the archive
	````sh
	`tar -xvzf teensy_linux64.tar.gz`
	````
1. Install dependencies
	````sh
	sudo apt-get install libcanberra-gtk-module
	````
1. Create `teensy.rules` as `sudo`


````sh
cd /etc/udev/rules.d/
````

Contents of `teensy.rules`

````rules
UBSYSTEM="usb_device",
SYSFS{idVendor}=="16c0",
SYSFS{idProduct}=="0478",
MODE="0666"
````	
