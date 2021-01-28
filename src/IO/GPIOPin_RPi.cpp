#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOPin.h"
#include <wiringPi.h>

IO::GPIOPin::GPIOPin(OSInt pinNum)
{
	this->pinNum = pinNum;
	wiringPiSetup();
}

IO::GPIOPin::~GPIOPin()
{
}

Bool IO::GPIOPin::IsError()
{
	return false;
}

Bool IO::GPIOPin::IsPinHigh()
{
	return digitalRead(this->pinNum) == HIGH;
}

void IO::GPIOPin::SetPinOutput(Bool isOutput)
{
	pinMode( this->pinNum, (isOutput?OUTPUT:INPUT) );
}

void IO::GPIOPin::SetPinState(Bool isHigh)
{
	digitalWrite(this->pinNum, (isHigh?HIGH:LOW) );
}

