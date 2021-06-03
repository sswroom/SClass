#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/I2C.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include <stdio.h>

IO::I2C::I2C(IO::I2CChannel *channel, UOSInt delayMS)
{
	this->channel = channel;
	this->delayMS = delayMS;
}

IO::I2C::~I2C()
{
}

void IO::I2C::Wait()
{
	if (this->delayMS > 0)
	{
		Sync::Thread::Sleep(this->delayMS);
	}
}

Bool IO::I2C::ReadBuff(UInt8 regAddr, UInt8 len, UInt8 *data)
{
	if (this->channel->I2CWrite(&regAddr, 1) != 1)
	{
		printf("ReadBuff: Write Error\r\n");
		return false;
	}
	this->Wait();
	if (this->channel->I2CRead(data, len) != len)
	{
		printf("ReadBuff: Read Error\r\n");
		return false;
	}
	return true;
}

Bool IO::I2C::WriteBuff(UInt8 regAddr, UInt8 len, UInt8 *data)
{
	UInt8 buff[64];
	buff[0] = regAddr;
	MemCopyNO(&buff[1], data, len);
	return this->channel->I2CWrite(buff, (UOSInt)len + 1) == (UOSInt)(len + 1);
}
