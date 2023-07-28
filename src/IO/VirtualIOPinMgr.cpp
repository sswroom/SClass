#include "Stdafx.h"
#include "IO/VirtualIOPinMgr.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

IO::VirtualIOPinMgr::VirtualIOPinMgr(UOSInt pinCnt)
{
	this->pinCnt = pinCnt;
	this->pins = MemAlloc(PinStatus*, this->pinCnt);
	PinStatus *status;
	UOSInt i;
	i = 0;
	while (i < pinCnt)
	{
		NEW_CLASS(status, PinStatus());
		status->pinNum = (UInt32)i;
		status->useCnt = 1;
		status->pullHigh = false;
		status->outputCnt = 0;
		status->outputHigh = false;
		this->pins[i] = status;
		i++;
	}
}

IO::VirtualIOPinMgr::~VirtualIOPinMgr()
{
	PinStatus *status;
	UOSInt i = this->pinCnt;
	Bool toRel;
	while (i-- > 0)
	{
		status = this->pins[i];
		Sync::MutexUsage mutUsage(status->mut);
		toRel = (status->useCnt-- <= 1);
		mutUsage.EndUse();

		if (toRel)
		{
			DEL_CLASS(status);
		}
	}
	MemFree(this->pins);
}

IO::IOPin *IO::VirtualIOPinMgr::CreatePin(UInt32 pinNum)
{
	if (pinNum >= this->pinCnt)
		return 0;
	IO::IOPin *pin;
	NEW_CLASS(pin, IO::VirtualIOPin(this->pins[pinNum]));
	return pin;
}

UOSInt IO::VirtualIOPinMgr::GetAvailablePins(Data::ArrayList<Int32> *pinList)
{
	UOSInt i = 0;
	UOSInt j = this->pinCnt;
	while (i < j)
	{
		pinList->Add((Int32)i);
		i++;
	}
	return j;
}

IO::VirtualIOPin::VirtualIOPin(IO::VirtualIOPinMgr::PinStatus *pinStatus)
{
	this->pinStatus = pinStatus;
	this->isOutput = false;
	Sync::MutexUsage mutUsage(this->pinStatus->mut);
	this->pinStatus->useCnt++;
	mutUsage.EndUse();
}

IO::VirtualIOPin::~VirtualIOPin()
{
	Bool isRel;
	Sync::MutexUsage mutUsage(this->pinStatus->mut);
	isRel = (this->pinStatus->useCnt-- <= 1);
	if (this->isOutput)
	{
		this->pinStatus->outputCnt--;
		this->isOutput = false;
	}
	mutUsage.EndUse();
	if (isRel)
	{
		DEL_CLASS(this->pinStatus);
	}
}

Bool IO::VirtualIOPin::IsPinHigh()
{
	Bool ret;
	Sync::MutexUsage mutUsage(this->pinStatus->mut);
	if (this->pinStatus->outputCnt > 0)
	{
		ret = this->pinStatus->outputHigh;
	}
	else
	{
		ret = this->pinStatus->pullHigh;
	}
	mutUsage.EndUse();
	return ret;
}

Bool IO::VirtualIOPin::IsPinOutput()
{
	return this->isOutput;
}

void IO::VirtualIOPin::SetPinOutput(Bool isOutput)
{
	if (this->isOutput != isOutput)
	{
		if (isOutput)
		{
			this->isOutput = true;
			Sync::MutexUsage mutUsage(this->pinStatus->mut);
			this->pinStatus->outputCnt++;
		}
		else
		{
			this->isOutput = false;
			Sync::MutexUsage mutUsage(this->pinStatus->mut);
			this->pinStatus->outputCnt--;
		}
	}
}

void IO::VirtualIOPin::SetPinState(Bool isHigh)
{
	if (this->isOutput)
	{
		Sync::MutexUsage mutUsage(this->pinStatus->mut);
		this->pinStatus->outputHigh = isHigh;
	}
}

Bool IO::VirtualIOPin::SetPullType(PullType pt)
{
	if (pt == IO::IOPin::PT_UP)
	{
		this->pinStatus->pullHigh = true;
	}
	else if (pt == IO::IOPin::PT_DOWN)
	{
		this->pinStatus->pullHigh = false;
	}
	return true;
}

UTF8Char *IO::VirtualIOPin::GetName(UTF8Char *buff)
{
	return Text::StrUInt32(Text::StrConcatC(buff, UTF8STRC("VirtualIOPin")), this->pinStatus->pinNum);
}
