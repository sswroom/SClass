#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "SSWR/AVIRead/AVIRStreamConvForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

void __stdcall SSWR::AVIRead::AVIRStreamConvForm::OnStream1Clicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamConvForm>();
	if (me->stm1.NotNull())
	{
		me->StopStream1();
	}
	else
	{
		IO::StreamType st;
		me->stm1 = me->core->OpenStream(st, me, 0, true);

		if (me->stm1.NotNull())
		{
			if (me->chkStreamLog1->IsChecked())
			{
				NEW_CLASSOPT(me->stmLog1, IO::FileStream(CSTR("Stm1Log.dat"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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

void __stdcall SSWR::AVIRead::AVIRStreamConvForm::OnStream2Clicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamConvForm>();
	if (me->stm2.NotNull())
	{
		me->StopStream2();
	}
	else
	{
		IO::StreamType st;
		me->stm2 = me->core->OpenStream(st, me, 0, true);

		if (me->stm2.NotNull())
		{
			if (me->chkStreamLog2->IsChecked())
			{
				NEW_CLASSOPT(me->stmLog2, IO::FileStream(CSTR("Stm2Log.dat"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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

void __stdcall SSWR::AVIRead::AVIRStreamConvForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamConvForm>();
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

UInt32 __stdcall SSWR::AVIRead::AVIRStreamConvForm::Stream1Thread(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamConvForm>();
	UInt8 buff[2048];
	UOSInt recvSize;
	NN<IO::Stream> stm1;
	NN<IO::Stream> stm2;
	NN<IO::FileStream> stmLog1;
	me->thread1Running = true;
	if (me->stm1.SetTo(stm1))
	{
		while (!me->thread1ToStop)
		{
			recvSize = stm1->Read(BYTEARR(buff));
			if (recvSize == 0)
			{
				me->remoteClosed1 = true;
			}
			else
			{
				if (me->stmLog1.SetTo(stmLog1))
				{
					stmLog1->Write(Data::ByteArrayR(buff, recvSize));
				}
				Sync::MutexUsage mutUsage(me->mut2);
				if (me->stm2.SetTo(stm2))
				{
					stm2->Write(Data::ByteArrayR(buff, recvSize));
				}
				mutUsage.EndUse();
				me->stm1DataSize += recvSize;
			}
		}
	}
	me->thread1Running = false;
	return 0;
}

UInt32 __stdcall SSWR::AVIRead::AVIRStreamConvForm::Stream2Thread(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamConvForm>();
	UInt8 buff[2048];
	UOSInt recvSize;
	NN<IO::Stream> stm1;
	NN<IO::Stream> stm2;
	NN<IO::FileStream> stmLog2;
	me->thread2Running = true;
	if (me->stm2.SetTo(stm2))
	{
		while (!me->thread2ToStop)
		{
			recvSize = stm2->Read(BYTEARR(buff));
			if (recvSize == 0)
			{
				me->remoteClosed2 = true;
			}
			else
			{
				if (me->stmLog2.SetTo(stmLog2))
				{
					stmLog2->Write(Data::ByteArrayR(buff, recvSize));
				}
				Sync::MutexUsage mutUsage(me->mut1);
				if (me->stm1.SetTo(stm1))
				{
					stm1->Write(Data::ByteArrayR(buff, recvSize));
				}
				mutUsage.EndUse();
				me->stm2DataSize += recvSize;
			}
		}
	}
	me->thread2Running = false;
	return 0;
}

void SSWR::AVIRead::AVIRStreamConvForm::StopStream1()
{
	NN<IO::Stream> stm1;
	if (this->stm1.SetTo(stm1))
	{
		this->thread1ToStop = true;
		Sync::MutexUsage mutUsage(this->mut1);
		stm1->Close();
		while (this->thread1Running)
		{
			Sync::SimpleThread::Sleep(10);
		}
		this->thread1ToStop = false;
		stm1.Delete();
		this->stm1 = 0;
		this->stmLog1.Delete();
		mutUsage.EndUse();
		this->txtStream1->SetText(CSTR("-"));
		this->btnStream1->SetText(CSTR("&Open"));
		this->remoteClosed1 = false;
	}
}

void SSWR::AVIRead::AVIRStreamConvForm::StopStream2()
{
	NN<IO::Stream> stm2;
	if (this->stm2.SetTo(stm2))
	{
		this->thread2ToStop = true;
		Sync::MutexUsage mutUsage(this->mut2);
		stm2->Close();
		while (this->thread2Running)
		{
			Sync::SimpleThread::Sleep(10);
		}
		this->thread2ToStop = false;
		stm2.Delete();
		this->stm2 = 0;
		this->stmLog2.Delete();
		mutUsage.EndUse();
		this->txtStream2->SetText(CSTR("-"));
		this->btnStream2->SetText(CSTR("&Open"));
		this->remoteClosed2 = false;
	}
}

SSWR::AVIRead::AVIRStreamConvForm::AVIRStreamConvForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 456, 200, ui)
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

	this->grpStream1 = ui->NewGroupBox(*this, CSTR("Stream 1"));
	this->grpStream1->SetRect(0, 0, 250, 48, false);
	this->grpStream1->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->pnlStream1 = ui->NewPanel(this->grpStream1);
	this->pnlStream1->SetRect(0, 0, 100, 104, false);
	this->pnlStream1->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblStream1 = ui->NewLabel(this->pnlStream1, CSTR("Stream Type"));
	this->lblStream1->SetRect(4, 4, 100, 23, false);
	this->txtStream1 = ui->NewTextBox(this->pnlStream1, CSTR("-"));
	this->txtStream1->SetRect(4, 28, 200, 23, false);
	this->txtStream1->SetReadOnly(true);
	this->chkStreamLog1 = ui->NewCheckBox(this->pnlStream1, CSTR("Log to file"), false);
	this->chkStreamLog1->SetRect(4, 52, 200, 23, false);
	this->btnStream1 = ui->NewButton(this->pnlStream1, CSTR("&Open"));
	this->btnStream1->SetRect(4, 76, 75, 23, false);
	this->btnStream1->HandleButtonClick(OnStream1Clicked, this);
	this->rlcStream1 = ui->NewRealtimeLineChart(this->grpStream1, this->core->GetDrawEngine(), 1, 120, 1000, 0);
	this->rlcStream1->SetDockType(UI::GUIControl::DOCK_FILL);
	this->hspStream = ui->NewHSplitter(*this, 3, false);

	this->grpStream2 = ui->NewGroupBox(*this, CSTR("Stream 2"));
	this->grpStream2->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlStream2 = ui->NewPanel(this->grpStream2);
	this->pnlStream2->SetRect(0, 0, 100, 104, false);
	this->pnlStream2->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblStream2 = ui->NewLabel(this->pnlStream2, CSTR("Stream Type"));
	this->lblStream2->SetRect(4, 4, 100, 23, false);
	this->txtStream2 = ui->NewTextBox(this->pnlStream2, CSTR("-"));
	this->txtStream2->SetRect(4, 28, 200, 23, false);
	this->txtStream2->SetReadOnly(true);
	this->chkStreamLog2 = ui->NewCheckBox(this->pnlStream2, CSTR("Log to file"), false);
	this->chkStreamLog2->SetRect(4, 52, 200, 23, false);
	this->btnStream2 = ui->NewButton(this->pnlStream2, CSTR("&Open"));
	this->btnStream2->SetRect(4, 76, 75, 23, false);
	this->btnStream2->HandleButtonClick(OnStream2Clicked, this);
	this->rlcStream2 = ui->NewRealtimeLineChart(this->grpStream2, this->core->GetDrawEngine(), 1, 120, 1000, 0);
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
