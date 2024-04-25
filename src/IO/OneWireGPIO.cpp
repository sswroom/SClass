#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/OneWireGPIO.h"
#include "Sync/SimpleThread.h"

IO::OneWireGPIO::OneWireGPIO(NN<IO::IOPin> pin)
{
	this->pin = pin;
}

IO::OneWireGPIO::~OneWireGPIO()
{
}

Bool IO::OneWireGPIO::Init()
{
	this->pin->SetPinOutput(true);
	this->pin->SetPinState(false);
	Sync::SimpleThread::Sleepus(480);
	this->pin->SetPinOutput(false);
	Sync::SimpleThread::Sleepus(1);
	if (!this->pin->IsPinHigh())
	{
		return false;
	}
	Sync::SimpleThread::Sleepus(60);
	if (this->pin->IsPinHigh())
	{
		return false;
	}
	Sync::SimpleThread::Sleepus(420);
	if (!this->pin->IsPinHigh())
	{
		return false;
	}
	return true;
}

void IO::OneWireGPIO::SendBits(const UInt8 *buff, OSInt nBits)
{
	UInt8 mask = 1;
	while (nBits-- > 0)
	{
		if (*buff & mask)
		{
			this->pin->SetPinOutput(true);
			this->pin->SetPinState(false);
			Sync::SimpleThread::Sleepus(6);
			this->pin->SetPinOutput(false);
			this->clk.Start();
			while (this->clk.GetTimeDiff() < 0.000064)
			{
				this->pin->IsPinHigh();
			}
//			Sync::SimpleThread::Sleepus(64);
		}
		else
		{
			this->pin->SetPinOutput(true);
			this->pin->SetPinState(false);
			Sync::SimpleThread::Sleepus(60);
			this->pin->SetPinOutput(false);
			this->clk.Start();
			while (this->clk.GetTimeDiff() < 0.000010)
			{
				this->pin->IsPinHigh();
			}
//			Sync::SimpleThread::Sleepus(10);
		}
		mask = (UInt8)(mask << 1);
		if (mask == 0)
		{
			mask = 1;
			buff++;
		}
	}
}

OSInt IO::OneWireGPIO::ReadBits(UInt8 *buff, OSInt nBits)
{
	OSInt i = nBits;
	UInt8 v = 0;
	UInt8 mask = 1;
	while (i-- > 0)
	{
		this->pin->SetPinOutput(true);
		this->pin->SetPinState(false);
		Sync::SimpleThread::Sleepus(6);
		this->pin->SetPinOutput(false);
		this->clk.Start();
		while (this->clk.GetTimeDiff() < 0.000004)
		{
			this->pin->IsPinHigh();
		}
//		Sync::SimpleThread::Sleepus(4);
		if (this->pin->IsPinHigh())
		{
			v |= mask;
		}
		mask = (UInt8)((mask << 1) & 255);
		while (this->clk.GetTimeDiff() < 0.000059)
		{
			this->pin->IsPinHigh();
		}
//		Sync::SimpleThread::Sleepus(55);
		if (mask == 0)
		{
			*buff++ = v;
			mask = 1;
			v = 0;
		}
	}
	if (mask != 1)
	{
		*buff = v;
	}
	return nBits;
}

