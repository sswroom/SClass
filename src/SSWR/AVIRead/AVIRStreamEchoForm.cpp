#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "SSWR/AVIRead/AVIRStreamEchoForm.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/Encoding.h"

void __stdcall SSWR::AVIRead::AVIRStreamEchoForm::OnStreamClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamEchoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamEchoForm>();
	if (me->stm)
	{
		me->StopStream();
	}
	else
	{
		IO::StreamType st;
		me->stm = me->core->OpenStream(st, me, 0, false);
		if (me->stm)
		{
			me->txtStream->SetText(IO::StreamTypeGetName(st));
			me->btnStream->SetText(CSTR("&Close"));
			me->remoteClosed = false;
			me->threadRunning = false;
			me->threadToStop = false;
			me->recvCount = 0;
			me->recvUpdated = false;

			Sync::ThreadUtil::Create(RecvThread, me.Ptr());
			while (!me->threadRunning && !me->remoteClosed)
			{
				Sync::SimpleThread::Sleep(10);
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRStreamEchoForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamEchoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamEchoForm>();
	if (me->remoteClosed)
	{
		me->remoteClosed = false;
		me->StopStream();
	}
	if (me->recvUpdated)
	{
		UTF8Char sbuff[22];
		UnsafeArray<UTF8Char> sptr;
		me->recvUpdated = false;
		sptr = Text::StrUInt64(sbuff, me->recvCount);
		me->txtDataSize->SetText(CSTRP(sbuff, sptr));
	}
}

UInt32 __stdcall SSWR::AVIRead::AVIRStreamEchoForm::RecvThread(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamEchoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamEchoForm>();
	UInt8 buff[2048];
	UOSInt recvSize;
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		recvSize = me->stm->Read(BYTEARR(buff));
		if (recvSize <= 0)
		{
			me->remoteClosed = true;
		}
		else
		{
			me->recvCount += recvSize;
			me->recvUpdated = true;
			me->stm->Write(Data::ByteArrayR(buff, recvSize));
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
			Sync::SimpleThread::Sleep(10);
		}
		this->threadToStop = false;
		DEL_CLASS(this->stm);
		this->stm = 0;
		this->txtStream->SetText(CSTR("-"));
		this->btnStream->SetText(CSTR("&Open"));
		this->remoteClosed = false;
	}
}

SSWR::AVIRead::AVIRStreamEchoForm::AVIRStreamEchoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 456, 200, ui)
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

	this->grpStream = ui->NewGroupBox(*this, CSTR("Stream"));
	this->grpStream->SetRect(0, 0, 100, 48, false);
	this->grpStream->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblStream = ui->NewLabel(this->grpStream, CSTR("Stream Type"));
	this->lblStream->SetRect(4, 4, 100, 23, false);
	this->txtStream = ui->NewTextBox(this->grpStream, CSTR("-"));
	this->txtStream->SetRect(104, 4, 200, 23, false);
	this->txtStream->SetReadOnly(true);
	this->btnStream = ui->NewButton(this->grpStream, CSTR("&Open"));
	this->btnStream->SetRect(304, 4, 75, 23, false);
	this->btnStream->HandleButtonClick(OnStreamClicked, this);
	
	this->grpStatus = ui->NewGroupBox(*this, CSTR("Status"));
	this->grpStatus->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblDataSize = ui->NewLabel(this->grpStatus, CSTR("Data Size"));
	this->lblDataSize->SetRect(0, 0, 100, 23, false);
	this->txtDataSize = ui->NewTextBox(this->grpStatus, CSTR("0"));
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
