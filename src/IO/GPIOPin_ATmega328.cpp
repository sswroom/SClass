#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOPin.h"
#include "Text/MyString.h"

#include <avr/io.h>
/*#define PINB (*(volatile UInt8*)0x23)
#define DDRB (*(volatile UInt8*)0x24)
#define PORTB (*(volatile UInt8*)0x25)
#define PINC (*(volatile UInt8*)0x26)
#define DDRC (*(volatile UInt8*)0x27)
#define PORTC (*(volatile UInt8*)0x28)
#define PIND (*(volatile UInt8*)0x29)
#define DDRD (*(volatile UInt8*)0x2A)
#define PORTD (*(volatile UInt8*)0x2B)*/

IO::GPIOPin::GPIOPin(IntOS pinNum)
{
	this->pinNum = pinNum;
}

IO::GPIOPin::~GPIOPin()
{
}

Bool IO::GPIOPin::IsError()
{
	return this->pinNum < 0 || this->pinNum >= 23;
}

Bool IO::GPIOPin::IsPinHigh()
{
	if (this->pinNum < 0)
	{
		return false;
	}
	else if (this->pinNum < 8)
	{
		return (PINB & (1 << this->pinNum)) != 0;
	}
	else if (this->pinNum < 15)
	{
		return (PINC & (1 << (this->pinNum - 8))) != 0;
	}
	else if (this->pinNum < 23)
	{
		return (PIND & (1 << (this->pinNum - 15))) != 0;
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
		if (this->pinNum < 0)
		{
		}
		else if (this->pinNum < 8)
		{
			DDRB |= (1 << this->pinNum);
		}
		else if (this->pinNum < 15)
		{
			DDRC |= (1 << (this->pinNum - 8));
		}
		else if (this->pinNum < 23)
		{
			DDRD |= (1 << (this->pinNum - 15));
		}
		else
		{
		}
	}
	else
	{
		if (this->pinNum < 0)
		{
		}
		else if (this->pinNum < 8)
		{
			DDRB &= ~(1 << this->pinNum);
		}
		else if (this->pinNum < 15)
		{
			DDRC &= ~(1 << (this->pinNum - 8));
		}
		else if (this->pinNum < 23)
		{
			DDRD &= ~(1 << (this->pinNum - 15));
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
		if (this->pinNum < 0)
		{
		}
		else if (this->pinNum < 8)
		{
			PORTB |= (1 << this->pinNum);
		}
		else if (this->pinNum < 15)
		{
			PORTC |= (1 << (this->pinNum - 8));
		}
		else if (this->pinNum < 23)
		{
			PORTD |= (1 << (this->pinNum - 15));
		}
		else
		{
		}
	}
	else
	{
		if (this->pinNum < 0)
		{
		}
		else if (this->pinNum < 8)
		{
			PORTB &= ~(1 << this->pinNum);
		}
		else if (this->pinNum < 15)
		{
			PORTC &= ~(1 << (this->pinNum - 8));
		}
		else if (this->pinNum < 23)
		{
			PORTD &= ~(1 << (this->pinNum - 15));
		}
		else
		{
		}
	}
}

Bool IO::GPIOPin::SetPullType(PullType pt)
{
}

UTF8Char *IO::GPIOPin::GetName(UTF8Char *buff)
{
	return Text::StrIntOS(Text::StrConcatC(buff, UTF8STRC("GPIO")), this->pinNum);
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

IntOS IO::GPIOPin::GetAvailablePins(Data::ArrayList<Int32> *pinList)
{
	IntOS i = 0;
	IntOS j = 23;
	while (i < j)
	{
		pinList->Add((Int32)i);
		i++;
	}
	return j;
}

