#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOPin.h"
#include "Text/MyString.h"

IO::GPIOPin::GPIOPin(NN<IO::GPIOControl> gpio, UInt16 pinNum)
{
	this->gpio = gpio;
	this->pinNum = pinNum;
}

IO::GPIOPin::~GPIOPin()
{
}

Bool IO::GPIOPin::IsError()
{
	return this->gpio->IsError();
}

Bool IO::GPIOPin::IsPinHigh()
{
	return this->gpio->IsPinHigh(this->pinNum);
}

Bool IO::GPIOPin::IsPinOutput()
{
	return this->gpio->IsPinOutput(this->pinNum);
}

void IO::GPIOPin::SetPinOutput(Bool isOutput)
{
	this->gpio->SetPinOutput(this->pinNum, isOutput);
}

void IO::GPIOPin::SetPinState(Bool isHigh)
{
	this->gpio->SetPinState(this->pinNum, isHigh);
}

Bool IO::GPIOPin::SetPullType(PullType pt)
{
	return this->gpio->SetPullType(this->pinNum, pt);
}

UTF8Char *IO::GPIOPin::GetName(UTF8Char *buff)
{
	return Text::StrUOSInt(Text::StrConcatC(buff, UTF8STRC("GPIO")), this->pinNum);
}

void IO::GPIOPin::SetEventOnHigh(Bool enable)
{
	this->gpio->SetEventOnHigh(this->pinNum, enable);
}

void IO::GPIOPin::SetEventOnLow(Bool enable)
{
	this->gpio->SetEventOnLow(this->pinNum, enable);
}

void IO::GPIOPin::SetEventOnRaise(Bool enable)
{
	this->gpio->SetEventOnRaise(this->pinNum, enable);
}

void IO::GPIOPin::SetEventOnFall(Bool enable)
{
	this->gpio->SetEventOnFall(this->pinNum, enable);
}

Bool IO::GPIOPin::HasEvent()
{
	return this->gpio->HasEvent(this->pinNum);
}

void IO::GPIOPin::ClearEvent()
{
	this->gpio->ClearEvent(this->pinNum);
}
