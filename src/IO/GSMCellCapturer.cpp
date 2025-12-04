#include "Stdafx.h"
#include "IO/GSMCellCapturer.h"
#include "IO/Device/HuaweiGSMModemController.h"

void __stdcall IO::GSMCellCapturer::CheckThread(NN<Sync::Thread> thread)
{
	NN<IO::GSMCellCapturer> me = thread->GetUserObj().GetNN<IO::GSMCellCapturer>();
	Data::Timestamp nextCheck = Data::Timestamp::UtcNow();
	Data::Timestamp currTime;
	NN<IO::GSMModemController> modem;
	Data::ArrayListNN<IO::GSMModemController::CellSignal> cells;
	while (!thread->IsStopping())
	{
		currTime = Data::Timestamp::UtcNow();
		if (currTime >= nextCheck)
		{
			nextCheck = currTime.AddSecond(10);
			if (me->modem.SetTo(modem))
			{
				me->lastScanTime = Data::Timestamp::UtcNow();
				cells.Clear();
				modem->QueryCells(cells);
				{
					Sync::MutexUsage mutUsage(me->cellMut);
					me->cells.MemFreeAll();
					me->cells.AddAll(cells);
				}
				if (me->hdlr)
				{
					me->hdlr(cells, me->hdlrObj);
				}
			}
		}
		thread->Wait(1000);
	}
}

IO::GSMCellCapturer::GSMCellCapturer(NN<IO::ATCommandChannel> channel, Bool needRelease) : thread(CheckThread, this, CSTR("GSMCell"))
{
	this->channel = channel;
	this->needRelease = needRelease;
	this->manuf = 0;
	this->model = 0;
	this->modem = 0;
	this->hdlr = 0;
	this->hdlrObj = nullptr;
	this->lastScanTime = nullptr;
	this->Reload();
}

IO::GSMCellCapturer::~GSMCellCapturer()
{
	this->Stop();
	this->modem.Delete();
	if (this->needRelease)
	{
		this->channel.Delete();
	}
	OPTSTR_DEL(this->manuf);
	OPTSTR_DEL(this->model);
	this->cells.MemFreeAll();
}

void IO::GSMCellCapturer::Reload()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	NN<IO::GSMModemController> modem;
	this->modem.Delete();
	NEW_CLASSNN(modem, IO::GSMModemController(this->channel, false));
	if (modem->GSMGetManufacturer(sbuff).SetTo(sptr))
	{
		OPTSTR_DEL(this->manuf);
		this->manuf = Text::String::NewP(sbuff, sptr);
	}
	if (modem->GSMGetModelIdent(sbuff).SetTo(sptr))
	{
		OPTSTR_DEL(this->model);
		this->model = Text::String::NewP(sbuff, sptr);
	}
	if (this->manuf.SetTo(s))
	{
		if (s->StartsWith(CSTR("Huawei")))
		{
			modem.Delete();
			NEW_CLASSNN(modem, IO::Device::HuaweiGSMModemController(this->channel, false));
		}
	}
	this->modem = modem;
}

Bool IO::GSMCellCapturer::Start()
{
	if (this->IsError())
		return false;
	return this->thread.Start();
}

void IO::GSMCellCapturer::Stop()
{
	this->thread.Stop();
	this->thread.WaitForEnd();
}

void IO::GSMCellCapturer::SetUpdateHandler(UpdateHandler hdlr, AnyType userObj)
{
	this->hdlr = hdlr;
	this->hdlrObj = userObj;
}

Bool IO::GSMCellCapturer::IsError() const
{
	return this->manuf.IsNull() || this->model.IsNull();
}

NN<Data::ArrayListNN<IO::GSMModemController::CellSignal>> IO::GSMCellCapturer::GetCells(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->cellMut);
	return this->cells;
}

Data::Timestamp IO::GSMCellCapturer::GetLastScanTime() const
{
	return this->lastScanTime;
}
