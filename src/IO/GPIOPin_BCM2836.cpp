#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOPin.h"
#include "IO/PhysicalMem.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

// BCM2709 (BCM2836 BCM2837)
#define IO_BASE_ADDR 0x3F000000
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
	NEW_CLASS(clsData->mem, IO::PhysicalMem(IO_BASE_ADDR + 0x200000, BLOCKSIZE));
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
		return (((ClassData*)this->clsData)->memPtr[13] & (1 << this->pinNum)) != 0;
	}
	else
	{
		return (((ClassData*)this->clsData)->memPtr[14] & (1 << (this->pinNum - 32))) != 0;
	}
}

Bool IO::GPIOPin::IsPinOutput()
{
	switch (this->pinNum)
	{
	case 0:
		return ((ClassData*)this->clsData)->memPtr[0] & 1;
	case 1:
		return (((ClassData*)this->clsData)->memPtr[0] >> 3) & 1;
	case 2:
		return (((ClassData*)this->clsData)->memPtr[0] >> 6) & 1;
	case 3:
		return (((ClassData*)this->clsData)->memPtr[0] >> 9) & 1;
	case 4:
		return (((ClassData*)this->clsData)->memPtr[0] >> 12) & 1;
	case 5:
		return (((ClassData*)this->clsData)->memPtr[0] >> 15) & 1;
	case 6:
		return (((ClassData*)this->clsData)->memPtr[0] >> 18) & 1;
	case 7:
		return (((ClassData*)this->clsData)->memPtr[0] >> 21) & 1;
	case 8:
		return (((ClassData*)this->clsData)->memPtr[0] >> 24) & 1;
	case 9:
		return (((ClassData*)this->clsData)->memPtr[0] >> 27) & 1;

	case 10:
		return ((ClassData*)this->clsData)->memPtr[1] & 1;
	case 11:
		return (((ClassData*)this->clsData)->memPtr[1] >> 3) & 1;
	case 12:
		return (((ClassData*)this->clsData)->memPtr[1] >> 6) & 1;
	case 13:
		return (((ClassData*)this->clsData)->memPtr[1] >> 9) & 1;
	case 14:
		return (((ClassData*)this->clsData)->memPtr[1] >> 12) & 1;
	case 15:
		return (((ClassData*)this->clsData)->memPtr[1] >> 15) & 1;
	case 16:
		return (((ClassData*)this->clsData)->memPtr[1] >> 18) & 1;
	case 17:
		return (((ClassData*)this->clsData)->memPtr[1] >> 21) & 1;
	case 18:
		return (((ClassData*)this->clsData)->memPtr[1] >> 24) & 1;
	case 19:
		return (((ClassData*)this->clsData)->memPtr[1] >> 27) & 1;

	case 20:
		return ((ClassData*)this->clsData)->memPtr[2] & 1;
	case 21:
		return (((ClassData*)this->clsData)->memPtr[2] >> 3) & 1;
	case 22:
		return (((ClassData*)this->clsData)->memPtr[2] >> 6) & 1;
	case 23:
		return (((ClassData*)this->clsData)->memPtr[2] >> 9) & 1;
	case 24:
		return (((ClassData*)this->clsData)->memPtr[2] >> 12) & 1;
	case 25:
		return (((ClassData*)this->clsData)->memPtr[2] >> 15) & 1;
	case 26:
		return (((ClassData*)this->clsData)->memPtr[2] >> 18) & 1;
	case 27:
		return (((ClassData*)this->clsData)->memPtr[2] >> 21) & 1;
	case 28:
		return (((ClassData*)this->clsData)->memPtr[2] >> 24) & 1;
	case 29:
		return (((ClassData*)this->clsData)->memPtr[2] >> 27) & 1;

	case 30:
		return ((ClassData*)this->clsData)->memPtr[3] & 1;
	case 31:
		return (((ClassData*)this->clsData)->memPtr[3] >> 3) & 1;
	case 32:
		return (((ClassData*)this->clsData)->memPtr[3] >> 6) & 1;
	case 33:
		return (((ClassData*)this->clsData)->memPtr[3] >> 9) & 1;
	case 34:
		return (((ClassData*)this->clsData)->memPtr[3] >> 12) & 1;
	case 35:
		return (((ClassData*)this->clsData)->memPtr[3] >> 15) & 1;
	case 36:
		return (((ClassData*)this->clsData)->memPtr[3] >> 18) & 1;
	case 37:
		return (((ClassData*)this->clsData)->memPtr[3] >> 21) & 1;
	case 38:
		return (((ClassData*)this->clsData)->memPtr[3] >> 24) & 1;
	case 39:
		return (((ClassData*)this->clsData)->memPtr[3] >> 27) & 1;

	case 40:
		return ((ClassData*)this->clsData)->memPtr[4] & 1;
	case 41:
		return (((ClassData*)this->clsData)->memPtr[4] >> 3) & 1;
	case 42:
		return (((ClassData*)this->clsData)->memPtr[4] >> 6) & 1;
	case 43:
		return (((ClassData*)this->clsData)->memPtr[4] >> 9) & 1;
	case 44:
		return (((ClassData*)this->clsData)->memPtr[4] >> 12) & 1;
	case 45:
		return (((ClassData*)this->clsData)->memPtr[4] >> 15) & 1;
	case 46:
		return (((ClassData*)this->clsData)->memPtr[4] >> 18) & 1;
	case 47:
		return (((ClassData*)this->clsData)->memPtr[4] >> 21) & 1;
	case 48:
		return (((ClassData*)this->clsData)->memPtr[4] >> 24) & 1;
	case 49:
		return (((ClassData*)this->clsData)->memPtr[4] >> 27) & 1;

	case 50:
		return ((ClassData*)this->clsData)->memPtr[5] & 1;
	case 51:
		return (((ClassData*)this->clsData)->memPtr[5] >> 3) & 1;
	case 52:
		return (((ClassData*)this->clsData)->memPtr[5] >> 6) & 1;
	case 53:
		return (((ClassData*)this->clsData)->memPtr[5] >> 9) & 1;
	}
	return false;
}

void IO::GPIOPin::SetPinOutput(Bool isOutput)
{
	UInt32 val = isOutput?1:0;
	switch (this->pinNum)
	{
	case 0:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~7) | val;
		break;
	case 1:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 3)) | (val << 3);
		break;
	case 2:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 6)) | (val << 6);
		break;
	case 3:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 9)) | (val << 9);
		break;
	case 4:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 12)) | (val << 12);
		break;
	case 5:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 15)) | (val << 15);
		break;
	case 6:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 18)) | (val << 18);
		break;
	case 7:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 21)) | (val << 21);
		break;
	case 8:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 24)) | (val << 24);
		break;
	case 9:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 27)) | (val << 27);
		break;

	case 10:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~7) | val;
		break;
	case 11:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 3)) | (val << 3);
		break;
	case 12:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 6)) | (val << 6);
		break;
	case 13:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 9)) | (val << 9);
		break;
	case 14:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 12)) | (val << 12);
		break;
	case 15:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 15)) | (val << 15);
		break;
	case 16:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 18)) | (val << 18);
		break;
	case 17:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 21)) | (val << 21);
		break;
	case 18:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 24)) | (val << 24);
		break;
	case 19:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 27)) | (val << 27);
		break;

	case 20:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~7) | val;
		break;
	case 21:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 3)) | (val << 3);
		break;
	case 22:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 6)) | (val << 6);
		break;
	case 23:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 9)) | (val << 9);
		break;
	case 24:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 12)) | (val << 12);
		break;
	case 25:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 15)) | (val << 15);
		break;
	case 26:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 18)) | (val << 18);
		break;
	case 27:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 21)) | (val << 21);
		break;
	case 28:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 24)) | (val << 24);
		break;
	case 29:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 27)) | (val << 27);
		break;

	case 30:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~7) | val;
		break;
	case 31:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 3)) | (val << 3);
		break;
	case 32:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 6)) | (val << 6);
		break;
	case 33:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 9)) | (val << 9);
		break;
	case 34:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 12)) | (val << 12);
		break;
	case 35:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 15)) | (val << 15);
		break;
	case 36:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 18)) | (val << 18);
		break;
	case 37:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 21)) | (val << 21);
		break;
	case 38:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 24)) | (val << 24);
		break;
	case 39:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 27)) | (val << 27);
		break;

	case 40:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~7) | val;
		break;
	case 41:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 3)) | (val << 3);
		break;
	case 42:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 6)) | (val << 6);
		break;
	case 43:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 9)) | (val << 9);
		break;
	case 44:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 12)) | (val << 12);
		break;
	case 45:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 15)) | (val << 15);
		break;
	case 46:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 18)) | (val << 18);
		break;
	case 47:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 21)) | (val << 21);
		break;
	case 48:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 24)) | (val << 24);
		break;
	case 49:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 27)) | (val << 27);
		break;

	case 50:
		((ClassData*)this->clsData)->memPtr[5] = (((ClassData*)this->clsData)->memPtr[5] & ~7) | val;
		break;
	case 51:
		((ClassData*)this->clsData)->memPtr[5] = (((ClassData*)this->clsData)->memPtr[5] & ~(7 << 3)) | (val << 3);
		break;
	case 52:
		((ClassData*)this->clsData)->memPtr[5] = (((ClassData*)this->clsData)->memPtr[5] & ~(7 << 6)) | (val << 6);
		break;
	case 53:
		((ClassData*)this->clsData)->memPtr[5] = (((ClassData*)this->clsData)->memPtr[5] & ~(7 << 9)) | (val << 9);
		break;
	}
}

void IO::GPIOPin::SetPinState(Bool isHigh)
{
	if (isHigh)
	{
		if (this->pinNum < 32)
		{
			((ClassData*)this->clsData)->memPtr[7] = 1 << this->pinNum;
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[8] = 1 << (this->pinNum - 32);
		}
	}
	else
	{
		if (this->pinNum < 32)
		{
			((ClassData*)this->clsData)->memPtr[10] = 1 << this->pinNum;
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[11] = 1 << (this->pinNum - 32);
		}
	}
}

Bool IO::GPIOPin::SetPullType(PullType pt)
{
	if (pt == PT_DISABLE)
	{
		((ClassData*)this->clsData)->memPtr[37] = 0;
	}
	else if (pt == PT_DOWN)
	{
		((ClassData*)this->clsData)->memPtr[37] = 1;
	}
	else if (pt == PT_UP)
	{
		((ClassData*)this->clsData)->memPtr[37] = 2;
	}
	else
	{
		return false;
	}
	Sync::Thread::Sleepus(1);
	if (this->pinNum < 32)
	{
		((ClassData*)this->clsData)->memPtr[38] = (1 << this->pinNum);
	}
	else
	{
		((ClassData*)this->clsData)->memPtr[39] = (1 << (this->pinNum - 32));
	}
	Sync::Thread::Sleepus(1);
	if (this->pinNum < 32)
	{
		((ClassData*)this->clsData)->memPtr[38] = 0;
	}
	else
	{
		((ClassData*)this->clsData)->memPtr[39] = 0;
	}
	return true;
}

UTF8Char *IO::GPIOPin::GetName(UTF8Char *buff)
{
	return Text::StrInt32(Text::StrConcatC(buff, UTF8STRC("GPIO")), this->pinNum);
}

void IO::GPIOPin::SetEventOnHigh(Bool enable)
{
	if (this->pinNum < 32)
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[25] |= (1 << this->pinNum);
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[25] &= ~(1 << this->pinNum);
		}
	}
	else
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[26] |= (1 << (this->pinNum - 32));
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[26] &= ~(1 << (this->pinNum - 32));
		}
	}
}

void IO::GPIOPin::SetEventOnLow(Bool enable)
{
	if (this->pinNum < 32)
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[28] |= (1 << this->pinNum);
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[28] &= ~(1 << this->pinNum);
		}
	}
	else
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[29] |= (1 << (this->pinNum - 32));
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[29] &= ~(1 << (this->pinNum - 32));
		}
	}
}

void IO::GPIOPin::SetEventOnRaise(Bool enable)
{
	if (this->pinNum < 32)
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[19] |= (1 << this->pinNum);
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[19] &= ~(1 << this->pinNum);
		}
	}
	else
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[20] |= (1 << (this->pinNum - 32));
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[20] &= ~(1 << (this->pinNum - 32));
		}
	}
}

void IO::GPIOPin::SetEventOnFall(Bool enable)
{
	if (this->pinNum < 32)
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[22] |= (1 << this->pinNum);
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[22] &= ~(1 << this->pinNum);
		}
	}
	else
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[23] |= (1 << (this->pinNum - 32));
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[23] &= ~(1 << (this->pinNum - 32));
		}
	}
}

Bool IO::GPIOPin::HasEvent()
{
	if (this->pinNum < 32)
	{
		return (((ClassData*)this->clsData)->memPtr[16] & (1 << this->pinNum)) != 0;
	}
	else
	{
		return (((ClassData*)this->clsData)->memPtr[17] & (1 << (this->pinNum - 32))) != 0;
	}
}

void IO::GPIOPin::ClearEvent()
{
	if (this->pinNum < 32)
	{
		((ClassData*)this->clsData)->memPtr[16] &= ~(1 << this->pinNum);
	}
	else
	{
		((ClassData*)this->clsData)->memPtr[17] &= ~(1 << (this->pinNum - 32));
	}
}

OSInt IO::GPIOPin::GetAvailablePins(Data::ArrayList<Int32> *pinList)
{
	OSInt i = 0;
	OSInt j = 54;
	while (i < j)
	{
		pinList->Add((Int32)i);
		i++;
	}
	return j;
}

