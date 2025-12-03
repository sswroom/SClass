#include "Stdafx.h"
#include "IO/GSMCellCapturer.h"
#include "IO/Device/HuaweiGSMModemController.h"

void __stdcall IO::GSMCellCapturer::CheckThread(NN<Sync::Thread> thread)
{
	while (!thread->IsStopping())
	{
		thread->Wait(1000);
	}
}

IO::GSMCellCapturer::GSMCellCapturer(NN<IO::ATCommandChannel> channel, Bool needRelease) : thread(CheckThread, this, CSTR("GSMCell"))
{
	this->channel = channel;
	this->needRelease = false;
	this->manuf = 0;
	this->model = 0;
	this->modem = 0;
	this->hdlr = 0;
	this->hdlrObj = nullptr;
	this->Reload();
}

IO::GSMCellCapturer::~GSMCellCapturer()
{
	this->modem.Delete();
	if (this->needRelease)
	{
		this->channel.Delete();
	}
	OPTSTR_DEL(this->manuf);
	OPTSTR_DEL(this->model);
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
