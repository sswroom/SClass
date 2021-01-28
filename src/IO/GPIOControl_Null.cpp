#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOControl.h"
#include "Text/MyString.h"

IO::GPIOControl::GPIOControl()
{
}

IO::GPIOControl::~GPIOControl()
{
}

Bool IO::GPIOControl::IsError()
{
	return true;
}

UOSInt IO::GPIOControl::GetPinCount()
{
	return 0;
}

Bool IO::GPIOControl::IsPinHigh(UOSInt pinNum)
{
	return false;
}

Bool IO::GPIOControl::IsPinOutput(UOSInt pinNum)
{
	return false;
}

UOSInt IO::GPIOControl::GetPinMode(UOSInt pinNum)
{
	return 0;
}

Bool IO::GPIOControl::SetPinOutput(UOSInt pinNum, Bool isOutput)
{
	return false;
}

Bool IO::GPIOControl::SetPinState(UOSInt pinNum, Bool isHigh)
{
	return false;
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

const UTF8Char *IO::GPIOControl::PinModeGetName(UOSInt pinNum, UOSInt pinState)
{
	return (const UTF8Char*)"Unknown";
}
