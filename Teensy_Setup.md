# How To Install Teensy Installer

## [Teensy First Use](https://www.pjrc.com/teensy/first_use.html)

## [Teensy Loader and Toolchain](https://www.pjrc.com/teensy/loader_linux.html)

### For Linux (Ubuntu 18)

1. Download the Teensy Program (64 bit) to Downloads
1. Unzip the archive
	````sh
	`tar -xvzf teensy_linux64.tar.gz`
	````
1. Install dependencies
	````sh
	sudo apt-get install gcc-avr binutils-avr avr-libc
	sudo apt-get install gdb-avr
	sudo apt-get install avrdude
	sudo apt-get install libcanberra-gtk-module
	````
1. Create `teensy.rules` as `sudo`
	````sh
	cd /etc/udev/rules.d/
	````

	Copy the contents of [`49-teensy.rules`](refs/49-teensy.rules)

### For Windows 10

- Download the Teensy executable program
- Install [WinAVR](https://sourceforge.net/projects/winavr/files/)
