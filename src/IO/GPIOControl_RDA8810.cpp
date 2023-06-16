#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOControl.h"
#include "IO/PhysicalMem.h"
#include "Sync/Interlocked.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

// RDA8810
#define GPIOA_BASE 0x20930000
#define GPIOB_BASE 0x20931000
#define GPIOC_BASE 0x11A08000
#define GPIOD_BASE 0x20932000

struct IO::GPIOControl::ClassData
{
	IO::PhysicalMem *mem;
	IO::PhysicalMem *memC;
	volatile UInt32 *gpioPtr;
	volatile UInt32 *gpiocPtr;
};

IO::GPIOControl::GPIOControl()
{
	ClassData *clsData = MemAlloc(ClassData, 1);
	NEW_CLASS(clsData->mem, IO::PhysicalMem(GPIOA_BASE, 0x4000));
	clsData->gpioPtr = (volatile UInt32 *)clsData->mem->GetPointer();
	NEW_CLASS(clsData->memC, IO::PhysicalMem(GPIOC_BASE, 0x4000));
	clsData->gpiocPtr = (volatile UInt32 *)clsData->memC->GetPointer();
	this->clsData = clsData;
}

IO::GPIOControl::~GPIOControl()
{
	DEL_CLASS(this->clsData->mem);
	DEL_CLASS(this->clsData->memC);
	MemFree(this->clsData);
}

Bool IO::GPIOControl::IsError()
{
	return this->clsData->mem->IsError() || this->clsData->memC->IsError();
}

UOSInt IO::GPIOControl::GetPinCount()
{
	return 128;
}

Bool IO::GPIOControl::IsPinHigh(UOSInt pinNum)
{
	Int32 index = pinNum & 31;
	OSInt ind = IsPinOutput(pinNum)?4:3;
	if (pinNum < 32)
	{
		return (this->clsData->gpioPtr[0 + ind] & (1 << index)) != 0;
	}
	else if (pinNum < 64)
	{
		return (this->clsData->gpioPtr[1024 + ind] & (1 << index)) != 0;
	}
	else if (pinNum < 96)
	{
		return (this->clsData->gpiocPtr[0 + ind] & (1 << index)) != 0;
	}
	else if (pinNum < 128)
	{
		return (this->clsData->gpioPtr[2048 + ind] & (1 << index)) != 0;
	}
	else
	{
		return false;
	}
}

Bool IO::GPIOControl::IsPinOutput(UOSInt pinNum)
{
	Int32 index = pinNum & 31;
	if (pinNum < 32)
	{
		return (this->clsData->gpioPtr[0] & (1 << index)) == 0;
	}
	else if (pinNum < 64)
	{
		return (this->clsData->gpioPtr[1024] & (1 << index)) == 0;
	}
	else if (pinNum < 96)
	{
		return (this->clsData->gpiocPtr[14] & (1 << index)) == 0;
	}
	else if (pinNum < 128)
	{
		return (this->clsData->gpioPtr[2048] & (1 << index)) == 0;
	}
	return false;
}

UOSInt IO::GPIOControl::GetPinMode(UOSInt pinNum)
{
	return IsPinOutput(pinNum)?1:0;
}

Bool IO::GPIOControl::SetPinOutput(UOSInt pinNum, Bool isOutput)
{
	Int32 index = pinNum & 31;
	OSInt ind = isOutput?1:2;
	if (pinNum < 32)
	{
		this->clsData->gpioPtr[0 + ind] = (1 << index);
	}
	else if (pinNum < 64)
	{
		this->clsData->gpioPtr[1024 + ind] = (1 << index);
	}
	else if (pinNum < 96)
	{
		this->clsData->gpiocPtr[0 + ind] = (1 << index);
	}
	else if (pinNum < 128)
	{
		this->clsData->gpioPtr[2048 + ind] = (1 << index);
	}
	else
	{
		return false;
	}
	return true;
}

Bool IO::GPIOControl::SetPinState(UOSInt pinNum, Bool isHigh)
{
	Int32 index = pinNum & 31;
	OSInt ind = isHigh?4:6;
	if (pinNum < 32)
	{
		this->clsData->gpioPtr[0 + ind] = (1 << index);
	}
	else if (pinNum < 64)
	{
		this->clsData->gpioPtr[1024 + ind] = (1 << index);
	}
	else if (pinNum < 96)
	{
		this->clsData->gpiocPtr[0 + ind] = (1 << index);
	}
	else if (pinNum < 128)
	{
		this->clsData->gpioPtr[2048 + ind] = (1 << index);
	}
	else
	{
		return false;
	}
	return true;
}

Bool IO::GPIOControl::SetPullType(UOSInt pinNum, IO::IOPin::PullType pt)
{
	return false;
}

void IO::GPIOControl::SetEventOnHigh(UOSInt pinNum, Bool enable)
{
}

void IO::GPIOControl::SetEventOnLow(UOSInt pinNum, Bool enable)
{
}

void IO::GPIOControl::SetEventOnRaise(UOSInt pinNum, Bool enable)
{
}

void IO::GPIOControl::SetEventOnFall(UOSInt pinNum, Bool enable)
{
}

Bool IO::GPIOControl::HasEvent(UOSInt pinNum)
{
	return false;
}

void IO::GPIOControl::ClearEvent(UOSInt pinNum)
{
}

Text::CString IO::GPIOControl::PinModeGetName(UOSInt pinNum, UOSInt pinMode)
{
	if (pinMode == 0)
	{
		return CSTR("Input");
	}
	else if (pinMode == 1)
	{
		return CSTR("Output");
	}
	return CSTR("Unknown");
}
