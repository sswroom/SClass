#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/I2CChannelOS.h"
#include "Text/MyString.h"
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#ifndef I2C_SLAVE
#define I2C_SLAVE 0x703
#endif

IO::I2CChannelOS::I2CChannelOS(Int32 busNo, UInt8 slaveAddr)
{
	Char sbuff[32];
	this->hand = 0;
	Text::StrInt32(Text::StrConcat(sbuff, "/dev/i2c-"), busNo);
	Int32 fd = open(sbuff, O_RDWR);
	if (fd < 0)
		return;
	if (ioctl(fd, I2C_SLAVE, slaveAddr) < 0)
	{
		close(fd);
		return;
	}
	this->hand = (void*)(OSInt)fd;
}

IO::I2CChannelOS::~I2CChannelOS()
{
	if (this->hand)
	{
		close((int)(OSInt)this->hand);
	}
}

Bool IO::I2CChannelOS::IsError()
{
	return this->hand == 0;
}

UOSInt IO::I2CChannelOS::I2CRead(UInt8 *buff, UOSInt buffSize)
{
	OSInt ret;
	while (true)
	{
		ret = read((int)(OSInt)this->hand, buff, buffSize);
		if (ret >= 0 || errno != EINTR)
			break;
	}
	if (ret < 0)
		return 0;
	else
		return (UOSInt)ret;
}

UOSInt IO::I2CChannelOS::I2CWrite(const UInt8 *buff, UOSInt buffSize)
{
	OSInt ret;
	while (true)
	{
		ret = write((int)(OSInt)this->hand, buff, buffSize);
		if (ret >= 0 || errno != EINTR)
			break;
	}
	if (ret < 0)
		return 0;
	else
		return (UOSInt)ret;
}
