#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOControl.h"
#include <avr/interrupt.h>
#include <avr/io.h>

#define HANDLERCNT 4

typedef struct
{
	IO::GPIOControl::InterruptHandler hdlr;
	void *userObj;
} GPIOControl_InterruptInfo;

static GPIOControl_InterruptInfo GPIOControl_intHdlrs[HANDLERCNT] = {0};

ISR(PCINT0_vect)
{
	OSInt i = 0;
	while (i < HANDLERCNT)
	{
		if (GPIOControl_intHdlrs[i].hdlr)
		{
			GPIOControl_intHdlrs[i].hdlr(GPIOControl_intHdlrs[i].userObj);
		}
		i++;
	}
}

ISR(PCINT1_vect, ISR_ALIASOF(PCINT0_vect));

ISR(PCINT2_vect, ISR_ALIASOF(PCINT0_vect));

IO::GPIOControl::GPIOControl()
{
}

IO::GPIOControl::~GPIOControl()
{
}

Bool IO::GPIOControl::IsError()
{
	return false;
}

UOSInt IO::GPIOControl::GetPinCount()
{
	return 23;
}


Bool IO::GPIOControl::IsPinHigh(UOSInt pinNum)
{
	if (pinNum < 8)
	{
		return (PINB & (1 << pinNum)) != 0;
	}
	else if (pinNum < 15)
	{
		return (PINC & (1 << (pinNum - 8))) != 0;
	}
	else if (pinNum < 23)
	{
		return (PIND & (1 << (pinNum - 15))) != 0;
	}
	else
	{
		return false;
	}
}

Bool IO::GPIOControl::SetPinOutput(UOSInt pinNum, Bool isOutput)
{
	if (isOutput)
	{
		if (pinNum < 8)
		{
			DDRB |= (1 << pinNum);
		}
		else if (pinNum < 15)
		{
			DDRC |= (1 << (pinNum - 8));
		}
		else if (pinNum < 23)
		{
			DDRD |= (1 << (pinNum - 15));
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (pinNum < 8)
		{
			DDRB &= ~(1 << pinNum);
		}
		else if (pinNum < 15)
		{
			DDRC &= ~(1 << (pinNum - 8));
		}
		else if (pinNum < 23)
		{
			DDRD &= ~(1 << (pinNum - 15));
		}
		else
		{
			return false;
		}
	}
	return true;
}

Bool IO::GPIOControl::SetPinState(UOSInt pinNum, Bool isHigh)
{
	if (isHigh)
	{
		if (pinNum < 8)
		{
			PORTB |= (1 << pinNum);
		}
		else if (pinNum < 15)
		{
			PORTC |= (1 << (pinNum - 8));
		}
		else if (pinNum < 23)
		{
			PORTD |= (1 << (pinNum - 15));
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (pinNum < 8)
		{
			PORTB &= ~(1 << pinNum);
		}
		else if (pinNum < 15)
		{
			PORTC &= ~(1 << (pinNum - 8));
		}
		else if (pinNum < 23)
		{
			PORTD &= ~(1 << (pinNum - 15));
		}
		else
		{
			return false;
		}
	}
	return true;
}

Bool IO::GPIOControl::SetPullType(UOSInt pinNum, IO::IOPin::PullType pt)
{
}

Bool IO::GPIOControl::InterruptEnable(UOSInt pinNum, Bool enable)
{
	if (pinNum < 8)
	{
		PCICR |= 1 << PCIE0;
		if (enable)
		{
			PCMSK0 |= 1 << (pinNum);
		}
		else
		{
			PCMSK0 &= ~(1 << (pinNum));
		}
	}
	else if (pinNum < 15)
	{
		PCICR |= 1 << PCIE1;
		if (enable)
		{
			PCMSK1 |= 1 << (pinNum - 8);
		}
		else
		{
			PCMSK1 &= ~(1 << (pinNum - 8));
		}
	}
	else if (pinNum < 23)
	{
		PCICR |= 1 << PCIE2;
		if (enable)
		{
			PCMSK2 |= 1 << (pinNum - 15);
		}
		else
		{
			PCMSK2 &= ~(1 << (pinNum - 15));
		}
	}
}

void IO::GPIOControl::HandleInterrupt(InterruptHandler hdlr, void *userObj)
{
	OSInt i = 0;
	while (i < HANDLERCNT)
	{
		if (GPIOControl_intHdlrs[i].hdlr == 0)
		{
			GPIOControl_intHdlrs[i].userObj = userObj;
			GPIOControl_intHdlrs[i].hdlr = hdlr;
			break;
		}
		i++;
	}
}

void IO::GPIOControl::UnhandleInterrupt(InterruptHandler hdlr, void *userObj)
{
	OSInt i = 0;
	while (i < HANDLERCNT)
	{
		if (GPIOControl_intHdlrs[i].hdlr == hdlr && GPIOControl_intHdlrs[i].userObj == userObj)
		{
			GPIOControl_intHdlrs[i].hdlr = 0;
			GPIOControl_intHdlrs[i].userObj = 0;
			break;
		}
		i++;
	}
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
}

void IO::GPIOControl::ClearEvent(UOSInt pinNum)
{
}

