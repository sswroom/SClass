#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/I2CChannelOS.h"
#include "IO/I2CMODBUS.h"
#include "IO/Device/AM2315.h"
//#define _DEBUG
#if defined(_DEBUG)
#include <stdio.h>
#endif

IO::Device::AM2315::AM2315(IO::I2CChannel *channel, Bool toRelease)
{
	this->channel = channel;
	this->toRelease = toRelease;
	NEW_CLASS(this->i2c, IO::I2CMODBUS(this->channel, 20));

	UInt8 buff[3];
	buff[0] = 3;
	buff[1] = 8;
	buff[2] = 7;
	this->Wakeup();
	if (this->channel->I2CWrite(buff, 3) != 3)
	{
		SDEL_CLASS(this->i2c);
#if defined(_DEBUG)
		printf("Error, AM2315 not found\r\n");
#endif
		return;
	}
	this->i2c->Wait();
	if (this->channel->I2CRead(buff, 3) != 3)
	{
		SDEL_CLASS(this->i2c);
#if defined(_DEBUG)
		printf("Error, AM2315 not valid\r\n");
#endif
		return;
	}
	if (buff[0] != 3 || buff[1] != 7)
	{
		SDEL_CLASS(this->i2c);
#if defined(_DEBUG)
		printf("Error, AM2315 wrong reply\r\n");
#endif
		return;
	}
}

IO::Device::AM2315::~AM2315()
{
	SDEL_CLASS(this->i2c);
	if (this->toRelease)
	{
		DEL_CLASS(this->channel);
	}
}

Bool IO::Device::AM2315::IsError()
{
	return this->i2c == 0;
}

void IO::Device::AM2315::Wakeup()
{
	UInt8 byte = 0;
	if (this->i2c)
	{
		this->channel->I2CWrite(&byte, 1);
		this->channel->I2CWrite(&byte, 1);
		this->i2c->Wait();
		this->channel->I2CWrite(&byte, 1);
		this->i2c->Wait();
	}
}

Bool IO::Device::AM2315::ReadTemperature(Single *temp)
{
	UInt8 buff[2];
	if (this->i2c == 0)
		return false;
	this->Wakeup();
	if (this->i2c->ReadBuff(2, 2, buff))
	{
		*temp = (Single)(ReadMInt16(buff) * 0.1);
		return true;
	}
	else
	{
		return false;
	}
}

Bool IO::Device::AM2315::ReadRH(Single *rh)
{
	UInt8 buff[2];
	if (this->i2c == 0)
		return false;
	this->Wakeup();
	if (this->i2c->ReadBuff(0, 2, buff))
	{
		*rh = (Single)(ReadMUInt16(buff) * 0.1);
		return true;
	}
	else
	{
		return false;
	}
}

IO::I2CChannel *IO::Device::AM2315::CreateDefChannel(Int32 i2cBusNum)
{
	IO::I2CChannel *channel;
	NEW_CLASS(channel, IO::I2CChannelOS(i2cBusNum, 0x5c));
	if (channel->IsError())
	{
#if defined(_DEBUG)
		printf("Error in opening i2c device");
#endif
		DEL_CLASS(channel);
		return 0;
	}
	return channel;
}
