#!/bin/sh
if [ $# -eq 2 ]
	then
		~/arduino-1.8.8/hardware/tools/avr/bin/avr-objcopy -O ihex -R .eeprom $1 $1.hex
		sudo ~/arduino-1.8.8/hardware/tools/avr/bin/avrdude -V -F -p atmega328p -C /home/sswroom/arduino-1.8.8/hardware/tools/avr/etc/avrdude.conf -P $2 -c stk500v1 -b 57600 -U flash:w:$1.hex
	else
		echo "arguments: [File Name] [Serial Port]"
fi
