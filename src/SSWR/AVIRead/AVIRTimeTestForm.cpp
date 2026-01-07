#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRTimeTestForm.h"
#include "Sync/SimpleThread.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRTimeTestForm::OnSleepMSClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTimeTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTimeTestForm>();
	Text::StringBuilderUTF8 sb;
	UInt32 t;
	Double tDiff;
	me->txtSleepMS->GetText(sb);
	Manage::HiResClock clk;
	if (sb.ToUInt32(t))
	{
		clk.Start();
		Sync::SimpleThread::Sleep(t);
		tDiff = clk.GetTimeDiff();
		sb.ClearStr();
		sb.AppendDouble(tDiff);
		me->txtActualTime->SetText(sb.ToCString());
	}
	else
	{
		me->txtActualTime->SetText(CSTR("Invalid Input"));
	}
}

void __stdcall SSWR::AVIRead::AVIRTimeTestForm::OnSleepUSClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTimeTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTimeTestForm>();
	Text::StringBuilderUTF8 sb;
	UInt32 t;
	Double tDiff;
	me->txtSleepUS->GetText(sb);
	Manage::HiResClock clk;
	if (sb.ToUInt32(t))
	{
		clk.Start();
		Sync::SimpleThread::Sleepus(t);
		tDiff = clk.GetTimeDiff();
		sb.ClearStr();
		sb.AppendDouble(tDiff);
		me->txtActualTime->SetText(sb.ToCString());
	}
	else
	{
		me->txtActualTime->SetText(CSTR("Invalid Input"));
	}
}

SSWR::AVIRead::AVIRTimeTestForm::AVIRTimeTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 320, 120, ui)
{
	this->core = core;
	this->SetText(CSTR("Timing Test"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblSleepMS = ui->NewLabel(*this, CSTR("Sleep Time (ms)"));
	this->lblSleepMS->SetRect(4, 4, 100, 23, false);
	this->txtSleepMS = ui->NewTextBox(*this, CSTR("100"));
	this->txtSleepMS->SetRect(104, 4, 100, 23, false);
	this->btnSleepMS = ui->NewButton(*this, CSTR("Test"));
	this->btnSleepMS->SetRect(204, 4, 75, 23, false);
	this->btnSleepMS->HandleButtonClick(OnSleepMSClicked, this);
	this->lblSleepUS = ui->NewLabel(*this, CSTR("Sleep Time (us)"));
	this->lblSleepUS->SetRect(4, 28, 100, 23, false);
	this->txtSleepUS = ui->NewTextBox(*this, CSTR("100"));
	this->txtSleepUS->SetRect(104, 28, 100, 23, false);
	this->btnSleepUS = ui->NewButton(*this, CSTR("Test"));
	this->btnSleepUS->SetRect(204, 28, 75, 23, false);
	this->btnSleepUS->HandleButtonClick(OnSleepUSClicked, this);
	this->lblActualTime = ui->NewLabel(*this, CSTR("Actual Time (s)"));
	this->lblActualTime->SetRect(4, 52, 100, 23, false);
	this->txtActualTime = ui->NewTextBox(*this, CSTR(""));
	this->txtActualTime->SetRect(104, 52, 150, 23, false);
	this->txtActualTime->SetReadOnly(true);
}

SSWR::AVIRead::AVIRTimeTestForm::~AVIRTimeTestForm()
{
}

void SSWR::AVIRead::AVIRTimeTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
