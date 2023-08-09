#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/I2CMODBUS.h"
#include "Text/MyString.h"
#include <stdio.h>

IO::I2CMODBUS::I2CMODBUS(NotNullPtr<IO::I2CChannel> channel, UOSInt delayMS) : I2C(channel, delayMS)
{
}

IO::I2CMODBUS::~I2CMODBUS()
{
}

Bool IO::I2CMODBUS::ReadBuff(UInt8 regAddr, UInt8 len, UInt8 *data)
{
	UInt8 buff[64];
	buff[0] = 3;
	buff[1] = regAddr;
	buff[2] = len;
	if (this->channel->I2CWrite(buff, 3) != 3)
	{
		printf("ReadBuff: Write Error\r\n");
		return false;
	}
	Wait();
	if (this->channel->I2CRead(buff, 4 + (UOSInt)len) != (4 + (UOSInt)len))
	{
		printf("ReadBuff: Read Error\r\n");
		return false;
	}
	UInt16 dataCRC = ReadUInt16(&buff[2 + len]);
	if (dataCRC == 0)
	{
		MemCopyNO(data, &buff[2], len);
		return true;
	}
	UInt8 crcVal[2];
	this->crc.Clear();
	this->crc.Calc(buff, (UOSInt)len + 2);
	this->crc.GetValue(crcVal);
	if (((UInt16)~ReadMUInt16(crcVal)) == dataCRC)
	{
		MemCopyNO(data, &buff[2], len);
		return true;
	}
	else
	{
		printf("ReadBuff: CRC Error %X != %X\r\n", ((UInt16)~ReadMUInt16(crcVal)), dataCRC);
		return false;
	}
}

Bool IO::I2CMODBUS::WriteBuff(UInt8 regAddr, UInt8 len, UInt8 *data)
{
	UInt8 buff[64];
	buff[0] = 0x10;
	buff[1] = regAddr;
	buff[2] = len;
	MemCopyNO(&buff[3], data, len);
	UInt8 crcVal[2];
	this->crc.Clear();
	this->crc.Calc(buff, (UOSInt)len + 3);
	this->crc.GetValue(crcVal);
	WriteInt16(&buff[len + 3], ~ReadMUInt16(crcVal));
	if (this->channel->I2CWrite(buff, (UOSInt)len + 5) != (UOSInt)(len + 5))
	{
		printf("WriteBuff: Write Error\r\n");
		return false;
	}
	Wait();
	if (this->channel->I2CRead(buff, 5) != 5)
	{
		printf("WriteBuff: Read Error\r\n");
		return false;
	}
	if (buff[0] != 0x10 || buff[1] != regAddr || buff[2] != len)
	{
		printf("WriteBuff: Reply Error\r\n");
		return false;
	}
	UInt16 dataCRC = ReadUInt16(&buff[3]);
	this->crc.Clear();
	this->crc.Calc(buff, 3);
	this->crc.GetValue(crcVal);
	if (((UInt16)~ReadMUInt16(crcVal)) == dataCRC)
	{
		return true;
	}
	else
	{
		printf("WriteBuff: CRC Error %X != %X\r\n", ((UInt16)~ReadMUInt16(crcVal)), dataCRC);
		return false;
	}
}
