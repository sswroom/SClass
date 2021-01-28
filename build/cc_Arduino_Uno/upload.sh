#!/bin/sh
if [ $# -eq 2 ]
	then
		avr-objcopy -O ihex -R .eeprom $1 $1.hex
		sudo avrdude -v -p atmega328p -C /usr/share/arduino/hardware/tools/avrdude.conf -P $2 -carduino -b 115200 -U flash:w:$1.hex
	else
		echo "arguments: [File Name] [Serial Port]"
fi
