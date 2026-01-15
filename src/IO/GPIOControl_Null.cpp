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

UIntOS IO::GPIOControl::GetPinCount()
{
	return 0;
}

Bool IO::GPIOControl::IsPinHigh(UIntOS pinNum)
{
	return false;
}

Bool IO::GPIOControl::IsPinOutput(UIntOS pinNum)
{
	return false;
}

UIntOS IO::GPIOControl::GetPinMode(UIntOS pinNum)
{
	return 0;
}

Bool IO::GPIOControl::SetPinOutput(UIntOS pinNum, Bool isOutput)
{
	return false;
}

Bool IO::GPIOControl::SetPinState(UIntOS pinNum, Bool isHigh)
{
	return false;
}

Bool IO::GPIOControl::SetPullType(UIntOS pinNum, IO::IOPin::PullType pt)
{
	return false;
}

void IO::GPIOControl::SetEventOnHigh(UIntOS pinNum, Bool enable)
{
}

void IO::GPIOControl::SetEventOnLow(UIntOS pinNum, Bool enable)
{
}

void IO::GPIOControl::SetEventOnRaise(UIntOS pinNum, Bool enable)
{
}

void IO::GPIOControl::SetEventOnFall(UIntOS pinNum, Bool enable)
{
}

Bool IO::GPIOControl::HasEvent(UIntOS pinNum)
{
	return false;
}

void IO::GPIOControl::ClearEvent(UIntOS pinNum)
{
}

Text::CStringNN IO::GPIOControl::PinModeGetName(UIntOS pinNum, UIntOS pinState)
{
	return CSTR("Unknown");
}
