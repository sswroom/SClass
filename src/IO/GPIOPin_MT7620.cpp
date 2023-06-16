#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOPin.h"
#include "IO/PhysicalMem.h"
#include "Sync/Interlocked.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

// MTK MT7620
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
	if (this->pinNum < 24)
	{
		return (((ClassData*)this->clsData)->memPtr[8] & (1 << this->pinNum)) != 0;
	}
	else if (this->pinNum < 40)
	{
		return (((ClassData*)this->clsData)->memPtr[18] & (1 << (this->pinNum - 24))) != 0;
	}
	else if (this->pinNum < 72)
	{
		return (((ClassData*)this->clsData)->memPtr[28] & (1 << (this->pinNum - 40))) != 0;
	}
	else if (this->pinNum == 72)
	{
		return (((ClassData*)this->clsData)->memPtr[38] & 1) != 0;
	}
	else
	{
		return false;
	}
	
}

Bool IO::GPIOPin::IsPinOutput()
{
	if (this->pinNum < 24)
	{
		return (((ClassData*)this->clsData)->memPtr[9] & (1 << this->pinNum)) != 0;
	}
	else if (this->pinNum < 40)
	{
		return (((ClassData*)this->clsData)->memPtr[19] & (1 << (this->pinNum - 24))) != 0;
	}
	else if (this->pinNum < 72)
	{
		return (((ClassData*)this->clsData)->memPtr[29] & (1 << (this->pinNum - 40))) != 0;
	}
	else if (this->pinNum == 72)
	{
		return (((ClassData*)this->clsData)->memPtr[39] & 1) != 0;
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
		if (this->pinNum < 24)
		{
			((ClassData*)this->clsData)->memPtr[9] |= (1 << this->pinNum);
		}
		else if (this->pinNum < 40)
		{
			((ClassData*)this->clsData)->memPtr[19] |= (1 << (this->pinNum - 24));
		}
		else if (this->pinNum < 72)
		{
			((ClassData*)this->clsData)->memPtr[29] |= (1 << (this->pinNum - 40));
		}
		else if (this->pinNum == 72)
		{
			((ClassData*)this->clsData)->memPtr[39] |= 1;
		}
		else
		{
		}
	}
	else
	{
		if (this->pinNum < 24)
		{
			((ClassData*)this->clsData)->memPtr[9] &= ~(1 << this->pinNum);
		}
		else if (this->pinNum < 40)
		{
			((ClassData*)this->clsData)->memPtr[19] &= ~(1 << (this->pinNum - 24));
		}
		else if (this->pinNum < 72)
		{
			((ClassData*)this->clsData)->memPtr[29] &= ~(1 << (this->pinNum - 40));
		}
		else if (this->pinNum == 72)
		{
			((ClassData*)this->clsData)->memPtr[39] &= ~1;
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
		if (this->pinNum < 24)
		{
			((ClassData*)this->clsData)->memPtr[11] |= (1 << this->pinNum);
		}
		else if (this->pinNum < 40)
		{
			((ClassData*)this->clsData)->memPtr[21] |= (1 << (this->pinNum - 24));
		}
		else if (this->pinNum < 72)
		{
			((ClassData*)this->clsData)->memPtr[31] |= (1 << (this->pinNum - 40));
		}
		else if (this->pinNum == 72)
		{
			((ClassData*)this->clsData)->memPtr[41] |= 1;
		}
		else
		{
		}
	}
	else
	{
		if (this->pinNum < 24)
		{
			((ClassData*)this->clsData)->memPtr[12] |= (1 << this->pinNum);
		}
		else if (this->pinNum < 40)
		{
			((ClassData*)this->clsData)->memPtr[22] |= (1 << (this->pinNum - 24));
		}
		else if (this->pinNum < 72)
		{
			((ClassData*)this->clsData)->memPtr[32] |= (1 << (this->pinNum - 40));
		}
		else if (this->pinNum == 72)
		{
			((ClassData*)this->clsData)->memPtr[42] |= 1;
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
	OSInt j = 73;
	while (i < j)
	{
		pinList->Add((Int32)i);
		i++;
	}
	return j;
}
