#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/I2CChannelOS.h"
#include "Text/MyString.h"

IO::I2CChannelOS::I2CChannelOS(Int32 busNo, UInt8 slaveAddr)
{
	this->hand = 0;
}

IO::I2CChannelOS::~I2CChannelOS()
{
}

Bool IO::I2CChannelOS::IsError()
{
	return this->hand == 0;
}

UOSInt IO::I2CChannelOS::I2CRead(UnsafeArray<UInt8> buff, UOSInt buffSize)
{
	return 0;
}

UOSInt IO::I2CChannelOS::I2CWrite(UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	return 0;
}
