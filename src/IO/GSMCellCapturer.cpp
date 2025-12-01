#include "Stdafx.h"
#include "IO/GSMCellCapturer.h"

IO::GSMCellCapturer::GSMCellCapturer(NN<IO::ATCommandChannel> channel, Bool needRelease)
{
	this->channel = channel;
	this->needRelease = false;
	this->manuf = 0;
	this->model = 0;
	this->Reload();
}

IO::GSMCellCapturer::~GSMCellCapturer()
{
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
	IO::GSMModemController modem(this->channel, false);
	if (modem.GSMGetManufacturer(sbuff).SetTo(sptr))
	{
		OPTSTR_DEL(this->manuf);
		this->manuf = Text::String::NewP(sbuff, sptr);
	}
	if (modem.GSMGetModelIdent(sbuff).SetTo(sptr))
	{
		OPTSTR_DEL(this->model);
		this->model = Text::String::NewP(sbuff, sptr);
	}
}

Bool IO::GSMCellCapturer::IsError() const
{
	return this->manuf.IsNull() || this->model.IsNull();
}
