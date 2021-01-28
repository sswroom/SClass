#include "Stdafx.h"
#include "IO/EEPROM16.h"

#define EEAR (*(volatile UInt16*)0x41)
#define EECR (*(volatile UInt8*)0x3F)
#define EEDR (*(volatile UInt8*)0x40)

#define EERE 0
#define EEPE 1
#define EEMPE 2
#define EERIE 3

UInt16 IO::EEPROM16::GetSize()
{
	return 1024;
}

UInt8 IO::EEPROM16::ReadByte(UInt16 addr)
{
	while (EECR & (1 << EEPE));
	EEAR = addr;
	EECR |= (1 << EERE);
	return EEDR;
}

Bool IO::EEPROM16::WriteByte(UInt16 addr, UInt8 data)
{
	while (EECR & (1 << EEPE));
	EEAR = addr;
	EEDR = data;
	EECR |= (1 << EEMPE);
	EECR |= (1 << EEPE);
	return true;
}

