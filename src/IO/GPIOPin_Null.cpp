#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOPin.h"
#include "Text/MyString.h"

IO::GPIOPin::GPIOPin(IO::GPIOControl *gpio, UInt16 pinNum)
{
	this->gpio = gpio;
	this->pinNum = pinNum;
}

IO::GPIOPin::~GPIOPin()
{
}

Bool IO::GPIOPin::IsError()
{
	return true;
}

Bool IO::GPIOPin::IsPinHigh()
{
	return false;
}

Bool IO::GPIOPin::IsPinOutput()
{
	return false;
}

void IO::GPIOPin::SetPinOutput(Bool isOutput)
{
}

void IO::GPIOPin::SetPinState(Bool isHigh)
{
}

Bool IO::GPIOPin::SetPullType(PullType pt)
{
	return false;
}

UTF8Char *IO::GPIOPin::GetName(UTF8Char *buff)
{
	return Text::StrUOSInt(Text::StrConcatC(buff, UTF8STRC("GPIO")), this->pinNum);
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
	return false;
}

void IO::GPIOPin::ClearEvent()
{
}
