#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/Device/DS18B20.h"
#include "Sync/ThreadUtil.h"

UInt8 IO::Device::DS18B20::CalcCRC(const UInt8 *buff, OSInt size)
{
	UInt8 v = 0;
	OSInt i;
	while (size-- > 0)
	{
		v = v ^ (*buff++);
		i = 8;
		while (i-- > 0)
		{
			if (v & 1)
			{
				v = (UInt8)((v >> 1) ^ 0x8c);
			}
			else
			{
				v = (UInt8)(v >> 1);
			}
		}
	}
	return v;
}

IO::Device::DS18B20::DS18B20(IO::OneWireGPIO *oneWire)
{
	this->oneWire = oneWire;
}

IO::Device::DS18B20::~DS18B20()
{
}

Bool IO::Device::DS18B20::ReadSensorID(UInt8 *buff)
{
	UInt8 rbuff[8];
	if (!this->oneWire->Init())
	{
		return false;
	}
	rbuff[0] = 0x33;
	oneWire->SendBits(rbuff, 8);
	oneWire->ReadBits(rbuff, 64);

	if (this->CalcCRC(rbuff, 7) != rbuff[7])
	{
		return false;
	}
	buff[0] = rbuff[0];
	buff[1] = rbuff[1];
	buff[2] = rbuff[2];
	buff[3] = rbuff[3];
	buff[4] = rbuff[4];
	buff[5] = rbuff[5];
	buff[6] = rbuff[6];
	return true;
}

Bool IO::Device::DS18B20::ConvTemp()
{
	UInt8 buff[2];
	if (!this->oneWire->Init())
	{
		return false;
	}
	buff[0] = 0xCC;
	buff[1] = 0x44;
	this->oneWire->SendBits(buff, 16);
	while (true)
	{
		this->oneWire->ReadBits(buff, 1);
		if (buff[0] != 0)
			break;
	}
	return true;
}

Bool IO::Device::DS18B20::ReadTemp(Double *temp)
{
	UInt8 buff[9];
	if (!oneWire->Init())
	{
		return false;
	}
	buff[0] = 0xCC;
	buff[1] = 0xBE;
	oneWire->SendBits(buff, 16);
	oneWire->ReadBits(buff, 72);
	if (this->CalcCRC(buff, 8) != buff[8])
	{
		return false;
	}
	if ((buff[4] & 0x60) == 0)
	{
		*temp = ReadInt16(buff) / 2.0;
	}
	else if ((buff[4] & 0x60) == 0x20) 
	{
		*temp = ReadInt16(buff) / 4.0;
	}
	else if ((buff[4] & 0x60) == 0x40) 
	{
		*temp = ReadInt16(buff) / 8.0;
	}
	else if ((buff[4] & 0x60) == 0x60) 
	{
		*temp = ReadInt16(buff) / 16.0;
	}
	return true;
}

