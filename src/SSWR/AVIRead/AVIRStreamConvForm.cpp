#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "SSWR/AVIRead/AVIRStreamConvForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRStreamConvForm::OnStream1Clicked(void *userObj)
{
	SSWR::AVIRead::AVIRStreamConvForm *me = (SSWR::AVIRead::AVIRStreamConvForm *)userObj;
	if (me->stm1)
	{
		me->StopStream1();
	}
	else
	{
		IO::StreamType st;
		me->stm1 = me->core->OpenStream(&st, me, 0, true);

		if (me->stm1)
		{
			if (me->chkStreamLog1->IsChecked())
			{
				NEW_CLASS(me->stmLog1, IO::FileStream(CSTR("Stm1Log.dat"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			}
			me->txtStream1->SetText(IO::StreamTypeGetName(st));
			me->btnStream1->SetText(CSTR("&Close"));
			me->remoteClosed1 = false;
			me->thread1Running = false;
			me->thread1ToStop = false;

			Sync::ThreadUtil::Create(Stream1Thread, me);
			while (!me->thread1Running && !me->remoteClosed1)
			{
				Sync::SimpleThread::Sleep(10);
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRStreamConvForm::OnStream2Clicked(void *userObj)
{
	SSWR::AVIRead::AVIRStreamConvForm *me = (SSWR::AVIRead::AVIRStreamConvForm *)userObj;
	if (me->stm2)
	{
		me->StopStream2();
	}
	else
	{
		IO::StreamType st;
		me->stm2 = me->core->OpenStream(&st, me, 0, true);

		if (me->stm2)
		{
			if (me->chkStreamLog2->IsChecked())
			{
				NEW_CLASS(me->stmLog2, IO::FileStream(CSTR("Stm2Log.dat"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			}
			me->txtStream2->SetText(IO::StreamTypeGetName(st));
			me->btnStream2->SetText(CSTR("&Close"));
			me->remoteClosed2 = false;
			me->thread2Running = false;
			me->thread2ToStop = false;

			Sync::ThreadUtil::Create(Stream2Thread, me);
			while (!me->thread2Running && !me->remoteClosed2)
			{
				Sync::SimpleThread::Sleep(10);
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRStreamConvForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRStreamConvForm *me = (SSWR::AVIRead::AVIRStreamConvForm *)userObj;
	if (me->remoteClosed1)
	{
		me->remoteClosed1 = false;
		me->StopStream1();
	}
	if (me->remoteClosed2)
	{
		me->remoteClosed2 = false;
		me->StopStream2();
	}
	UInt64 val = me->stm1DataSize;
	Double dv = (Double)(val - me->stm1LastSize);
	me->rlcStream1->AddSample(&dv);
	me->stm1LastSize = val;
	val = me->stm2DataSize;
	dv = (Double)(val - me->stm2LastSize);
	me->rlcStream2->AddSample(&dv);
	me->stm2LastSize = val;
}

UInt32 __stdcall SSWR::AVIRead::AVIRStreamConvForm::Stream1Thread(void *userObj)
{
	SSWR::AVIRead::AVIRStreamConvForm *me = (SSWR::AVIRead::AVIRStreamConvForm *)userObj;
	UInt8 buff[2048];
	UOSInt recvSize;
	me->thread1Running = true;
	while (!me->thread1ToStop)
	{
		recvSize = me->stm1->Read(BYTEARR(buff));
		if (recvSize == 0)
		{
			me->remoteClosed1 = true;
		}
		else
		{
			if (me->stmLog1)
			{
				me->stmLog1->Write(buff, recvSize);
			}
			Sync::MutexUsage mutUsage(&me->mut2);
			if (me->stm2)
			{
				me->stm2->Write(buff, recvSize);
			}
			mutUsage.EndUse();
			me->stm1DataSize += recvSize;
		}
	}
	me->thread1Running = false;
	return 0;
}

UInt32 __stdcall SSWR::AVIRead::AVIRStreamConvForm::Stream2Thread(void *userObj)
{
	SSWR::AVIRead::AVIRStreamConvForm *me = (SSWR::AVIRead::AVIRStreamConvForm *)userObj;
	UInt8 buff[2048];
	UOSInt recvSize;
	me->thread2Running = true;
	while (!me->thread2ToStop)
	{
		recvSize = me->stm2->Read(BYTEARR(buff));
		if (recvSize == 0)
		{
			me->remoteClosed2 = true;
		}
		else
		{
			if (me->stmLog2)
			{
				me->stmLog2->Write(buff, recvSize);
			}
			Sync::MutexUsage mutUsage(&me->mut1);
			if (me->stm1)
			{
				me->stm1->Write(buff, recvSize);
			}
			mutUsage.EndUse();
			me->stm2DataSize += recvSize;
		}
	}
	me->thread2Running = false;
	return 0;
}

void SSWR::AVIRead::AVIRStreamConvForm::StopStream1()
{
	if (this->stm1)
	{
		this->thread1ToStop = true;
		Sync::MutexUsage mutUsage(&this->mut1);
		this->stm1->Close();
		while (this->thread1Running)
		{
			Sync::SimpleThread::Sleep(10);
		}
		this->thread1ToStop = false;
		DEL_CLASS(this->stm1);
		this->stm1 = 0;
		SDEL_CLASS(this->stmLog1);
		mutUsage.EndUse();
		this->txtStream1->SetText(CSTR("-"));
		this->btnStream1->SetText(CSTR("&Open"));
		this->remoteClosed1 = false;
	}
}

void SSWR::AVIRead::AVIRStreamConvForm::StopStream2()
{
	if (this->stm2)
	{
		this->thread2ToStop = true;
		Sync::MutexUsage mutUsage(&this->mut2);
		this->stm2->Close();
		while (this->thread2Running)
		{
			Sync::SimpleThread::Sleep(10);
		}
		this->thread2ToStop = false;
		DEL_CLASS(this->stm2);
		this->stm2 = 0;
		SDEL_CLASS(this->stmLog2);
		mutUsage.EndUse();
		this->txtStream2->SetText(CSTR("-"));
		this->btnStream2->SetText(CSTR("&Open"));
		this->remoteClosed2 = false;
	}
}

SSWR::AVIRead::AVIRStreamConvForm::AVIRStreamConvForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 456, 200, ui)
{
	this->SetText(CSTR("Stream Converter"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->siLabDriver = this->core->GetSiLabDriver();
	this->stm1DataSize = 0;
	this->stm1LastSize = 0;
	this->stm2DataSize = 0;
	this->stm2LastSize = 0;
	this->stm1 = 0;
	this->stmLog1 = 0;
	this->stm2 = 0;
	this->stmLog2 = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->grpStream1, UI::GUIGroupBox(ui, this, CSTR("Stream 1")));
	this->grpStream1->SetRect(0, 0, 250, 48, false);
	this->grpStream1->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->pnlStream1, UI::GUIPanel(ui, this->grpStream1));
	this->pnlStream1->SetRect(0, 0, 100, 104, false);
	this->pnlStream1->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblStream1, UI::GUILabel(ui, this->pnlStream1, CSTR("Stream Type")));
	this->lblStream1->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtStream1, UI::GUITextBox(ui, this->pnlStream1, CSTR("-")));
	this->txtStream1->SetRect(4, 28, 200, 23, false);
	this->txtStream1->SetReadOnly(true);
	NEW_CLASS(this->chkStreamLog1, UI::GUICheckBox(ui, this->pnlStream1, CSTR("Log to file"), false));
	this->chkStreamLog1->SetRect(4, 52, 200, 23, false);
	NEW_CLASS(this->btnStream1, UI::GUIButton(ui, this->pnlStream1, CSTR("&Open")));
	this->btnStream1->SetRect(4, 76, 75, 23, false);
	this->btnStream1->HandleButtonClick(OnStream1Clicked, this);
	NEW_CLASS(this->rlcStream1, UI::GUIRealtimeLineChart(ui, this->grpStream1, this->core->GetDrawEngine(), 1, 120, 1000));
	this->rlcStream1->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->hspStream, UI::GUIHSplitter(ui, this, 3, false));

	NEW_CLASS(this->grpStream2, UI::GUIGroupBox(ui, this, CSTR("Stream 2")));
	this->grpStream2->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlStream2, UI::GUIPanel(ui, this->grpStream2));
	this->pnlStream2->SetRect(0, 0, 100, 104, false);
	this->pnlStream2->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblStream2, UI::GUILabel(ui, this->pnlStream2, CSTR("Stream Type")));
	this->lblStream2->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtStream2, UI::GUITextBox(ui, this->pnlStream2, CSTR("-")));
	this->txtStream2->SetRect(4, 28, 200, 23, false);
	this->txtStream2->SetReadOnly(true);
	NEW_CLASS(this->chkStreamLog2, UI::GUICheckBox(ui, this->pnlStream2, CSTR("Log to file"), false));
	this->chkStreamLog2->SetRect(4, 52, 200, 23, false);
	NEW_CLASS(this->btnStream2, UI::GUIButton(ui, this->pnlStream2, CSTR("&Open")));
	this->btnStream2->SetRect(4, 76, 75, 23, false);
	this->btnStream2->HandleButtonClick(OnStream2Clicked, this);
	NEW_CLASS(this->rlcStream2, UI::GUIRealtimeLineChart(ui, this->grpStream2, this->core->GetDrawEngine(), 1, 120, 1000));
	this->rlcStream2->SetDockType(UI::GUIControl::DOCK_FILL);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRStreamConvForm::~AVIRStreamConvForm()
{
	StopStream1();
	StopStream2();
}

void SSWR::AVIRead::AVIRStreamConvForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
