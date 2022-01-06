#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOPin.h"
#include "IO/PhysicalMem.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

// MTK MT7628
#define IO_BASE_ADDR 0x10000000
#define BLOCKSIZE 4096

typedef struct
{
	IO::PhysicalMem *mem;
	volatile UInt32 *memPtr;
} ClassData;

IO::GPIOPin::GPIOPin(OSInt pinNum)
{
	this->pinNum = pinNum;
	ClassData *clsData = MemAlloc(ClassData, 1);
	NEW_CLASS(clsData->mem, IO::PhysicalMem(IO_BASE_ADDR + 0x600, BLOCKSIZE));
	clsData->memPtr = (volatile UInt32 *)clsData->mem->GetPointer();
	this->clsData = clsData;
}

IO::GPIOPin::~GPIOPin()
{
	ClassData *clsData = (ClassData*)this->clsData;
	DEL_CLASS(clsData->mem);
	MemFree(clsData);
}

Bool IO::GPIOPin::IsError()
{
	return ((ClassData*)this->clsData)->mem->IsError();
}

Bool IO::GPIOPin::IsPinHigh()
{
	if (this->pinNum < 32)
	{
		return (((ClassData*)this->clsData)->memPtr[8] & (1 << this->pinNum)) != 0;
	}
	else if (this->pinNum < 64)
	{
		return (((ClassData*)this->clsData)->memPtr[9] & (1 << (this->pinNum - 32))) != 0;
	}
	else if (this->pinNum < 96)
	{
		return (((ClassData*)this->clsData)->memPtr[10] & (1 << (this->pinNum - 64))) != 0;
	}
	else
	{
		return false;
	}
	
}

Bool IO::GPIOPin::IsPinOutput()
{
	if (this->pinNum < 32)
	{
		return (((ClassData*)this->clsData)->memPtr[0] & (1 << this->pinNum)) != 0;
	}
	else if (this->pinNum < 64)
	{
		return (((ClassData*)this->clsData)->memPtr[1] & (1 << (this->pinNum - 32))) != 0;
	}
	else if (this->pinNum < 96)
	{
		return (((ClassData*)this->clsData)->memPtr[2] & (1 << (this->pinNum - 64))) != 0;
	}
	else
	{
		return false;
	}
}

void IO::GPIOPin::SetPinOutput(Bool isOutput)
{
	if (isOutput)
	{
		if (this->pinNum < 32)
		{
			((ClassData*)this->clsData)->memPtr[0] |= (1 << this->pinNum);
		}
		else if (this->pinNum < 64)
		{
			((ClassData*)this->clsData)->memPtr[1] |= (1 << (this->pinNum - 32));
		}
		else if (this->pinNum < 96)
		{
			((ClassData*)this->clsData)->memPtr[2] |= (1 << (this->pinNum - 64));
		}
		else
		{
		}
	}
	else
	{
		if (this->pinNum < 32)
		{
			((ClassData*)this->clsData)->memPtr[0] &= ~(1 << this->pinNum);
		}
		else if (this->pinNum < 64)
		{
			((ClassData*)this->clsData)->memPtr[1] &= ~(1 << (this->pinNum - 32));
		}
		else if (this->pinNum < 96)
		{
			((ClassData*)this->clsData)->memPtr[2] &= ~(1 << (this->pinNum - 64));
		}
		else
		{
		}
	}
}

void IO::GPIOPin::SetPinState(Bool isHigh)
{
	if (isHigh)
	{
		if (this->pinNum < 32)
		{
			((ClassData*)this->clsData)->memPtr[12] |= (1 << this->pinNum);
		}
		else if (this->pinNum < 64)
		{
			((ClassData*)this->clsData)->memPtr[13] |= (1 << (this->pinNum - 32));
		}
		else if (this->pinNum < 96)
		{
			((ClassData*)this->clsData)->memPtr[14] |= (1 << (this->pinNum - 64));
		}
		else
		{
		}
	}
	else
	{
		if (this->pinNum < 32)
		{
			((ClassData*)this->clsData)->memPtr[16] |= (1 << this->pinNum);
		}
		else if (this->pinNum < 64)
		{
			((ClassData*)this->clsData)->memPtr[17] |= (1 << (this->pinNum - 32));
		}
		else if (this->pinNum < 96)
		{
			((ClassData*)this->clsData)->memPtr[18] |= (1 << (this->pinNum - 64));
		}
		else
		{
		}
	}
}

Bool IO::GPIOPin::SetPullType(PullType pt)
{
	return false;
}

UTF8Char *IO::GPIOPin::GetName(UTF8Char *buff)
{
	return Text::StrInt32(Text::StrConcatC(buff, UTF8STRC("GPIO")), this->pinNum);
}

void IO::GPIOPin::SetEventOnHigh(Bool enable)
{
}

void IO::GPIOPin::SetEventOnLow(Bool enable)
{
}

void IO::GPIOPin::SetEventOnRaise(Bool enable)
{
}

void IO::GPIOPin::SetEventOnFall(Bool enable)
{
}

Bool IO::GPIOPin::HasEvent()
{
}

void IO::GPIOPin::ClearEvent()
{
}

OSInt IO::GPIOPin::GetAvailablePins(Data::ArrayList<Int32> *pinList)
{
	OSInt i = 0;
	OSInt j = 96;
	while (i < j)
	{
		pinList->Add((Int32)i);
		i++;
	}
	return j;
}
