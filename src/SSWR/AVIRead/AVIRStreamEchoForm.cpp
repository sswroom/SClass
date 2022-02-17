#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "SSWR/AVIRead/AVIRStreamEchoForm.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"

void __stdcall SSWR::AVIRead::AVIRStreamEchoForm::OnStreamClicked(void *userObj)
{
	SSWR::AVIRead::AVIRStreamEchoForm *me = (SSWR::AVIRead::AVIRStreamEchoForm *)userObj;
	if (me->stm)
	{
		me->StopStream();
	}
	else
	{
		SSWR::AVIRead::AVIRCore::StreamType st;
		me->stm = me->core->OpenStream(&st, me, 0, false);
		if (me->stm)
		{
			me->txtStream->SetText(SSWR::AVIRead::AVIRCore::GetStreamTypeName(st));
			me->btnStream->SetText(CSTR("&Close"));
			me->remoteClosed = false;
			me->threadRunning = false;
			me->threadToStop = false;
			me->recvCount = 0;
			me->recvUpdated = false;

			Sync::Thread::Create(RecvThread, me);
			while (!me->threadRunning && !me->remoteClosed)
			{
				Sync::Thread::Sleep(10);
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRStreamEchoForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRStreamEchoForm *me = (SSWR::AVIRead::AVIRStreamEchoForm *)userObj;
	if (me->remoteClosed)
	{
		me->remoteClosed = false;
		me->StopStream();
	}
	if (me->recvUpdated)
	{
		UTF8Char sbuff[22];
		UTF8Char *sptr;
		me->recvUpdated = false;
		sptr = Text::StrUInt64(sbuff, me->recvCount);
		me->txtDataSize->SetText(CSTRP(sbuff, sptr));
	}
}

UInt32 __stdcall SSWR::AVIRead::AVIRStreamEchoForm::RecvThread(void *userObj)
{
	SSWR::AVIRead::AVIRStreamEchoForm *me = (SSWR::AVIRead::AVIRStreamEchoForm *)userObj;
	UInt8 buff[2048];
	UOSInt recvSize;
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		recvSize = me->stm->Read(buff, 2048);
		if (recvSize <= 0)
		{
			me->remoteClosed = true;
		}
		else
		{
			me->recvCount += recvSize;
			me->recvUpdated = true;
			me->stm->Write(buff, recvSize);
		}
	}
	me->threadRunning = false;
	return 0;
}

void SSWR::AVIRead::AVIRStreamEchoForm::StopStream()
{
	if (this->stm)
	{
		this->stm->Close();
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
		this->threadToStop = false;
		DEL_CLASS(this->stm);
		this->stm = 0;
		this->txtStream->SetText(CSTR("-"));
		this->btnStream->SetText(CSTR("&Open"));
		this->remoteClosed = false;
	}
}

SSWR::AVIRead::AVIRStreamEchoForm::AVIRStreamEchoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 456, 200, ui)
{
	this->SetText(CSTR("Stream Echo"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	
	this->core = core;
	this->siLabDriver = this->core->GetSiLabDriver();
	this->stm = 0;
	this->threadToStop = false;
	this->threadRunning = false;
	this->recvUpdated = false;
	this->recvCount = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->grpStream, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Stream"));
	this->grpStream->SetRect(0, 0, 100, 48, false);
	this->grpStream->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblStream, UI::GUILabel(ui, this->grpStream, (const UTF8Char*)"Stream Type"));
	this->lblStream->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtStream, UI::GUITextBox(ui, this->grpStream, CSTR("-")));
	this->txtStream->SetRect(104, 4, 200, 23, false);
	this->txtStream->SetReadOnly(true);
	NEW_CLASS(this->btnStream, UI::GUIButton(ui, this->grpStream, CSTR("&Open")));
	this->btnStream->SetRect(304, 4, 75, 23, false);
	this->btnStream->HandleButtonClick(OnStreamClicked, this);
	
	NEW_CLASS(this->grpStatus, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Status"));
	this->grpStatus->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblDataSize, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Data Size"));
	this->lblDataSize->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtDataSize, UI::GUITextBox(ui, this->grpStatus, CSTR("0")));
	this->txtDataSize->SetRect(100, 0, 150, 23, false);
	this->txtDataSize->SetReadOnly(true);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRStreamEchoForm::~AVIRStreamEchoForm()
{
	StopStream();
}

void SSWR::AVIRead::AVIRStreamEchoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
