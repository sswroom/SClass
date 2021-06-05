#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRTimeTestForm.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRTimeTestForm::OnSleepMSClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTimeTestForm *me = (SSWR::AVIRead::AVIRTimeTestForm *)userObj;
	Text::StringBuilderUTF8 sb;
	UInt32 t;
	Double tDiff;
	me->txtSleepMS->GetText(&sb);
	Manage::HiResClock clk;
	if (sb.ToUInt32(&t))
	{
		clk.Start();
		Sync::Thread::Sleep(t);
		tDiff = clk.GetTimeDiff();
		sb.ClearStr();
		Text::SBAppendF64(&sb, tDiff);
		me->txtActualTime->SetText(sb.ToString());
	}
	else
	{
		me->txtActualTime->SetText((const UTF8Char*)"Invalid Input");
	}
}

void __stdcall SSWR::AVIRead::AVIRTimeTestForm::OnSleepUSClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTimeTestForm *me = (SSWR::AVIRead::AVIRTimeTestForm *)userObj;
	Text::StringBuilderUTF8 sb;
	UInt32 t;
	Double tDiff;
	me->txtSleepUS->GetText(&sb);
	Manage::HiResClock clk;
	if (sb.ToUInt32(&t))
	{
		clk.Start();
		Sync::Thread::Sleepus(t);
		tDiff = clk.GetTimeDiff();
		sb.ClearStr();
		Text::SBAppendF64(&sb, tDiff);
		me->txtActualTime->SetText(sb.ToString());
	}
	else
	{
		me->txtActualTime->SetText((const UTF8Char*)"Invalid Input");
	}
}

SSWR::AVIRead::AVIRTimeTestForm::AVIRTimeTestForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 320, 120, ui)
{
	this->core = core;
	this->SetText((const UTF8Char*)"Timing Test");
	this->SetFont(0, 8.25, false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblSleepMS, UI::GUILabel(ui, this, (const UTF8Char*)"Sleep Time (ms)"));
	this->lblSleepMS->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtSleepMS, UI::GUITextBox(ui, this, (const UTF8Char*)"100"));
	this->txtSleepMS->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->btnSleepMS, UI::GUIButton(ui, this, (const UTF8Char*)"Test"));
	this->btnSleepMS->SetRect(204, 4, 75, 23, false);
	this->btnSleepMS->HandleButtonClick(OnSleepMSClicked, this);
	NEW_CLASS(this->lblSleepUS, UI::GUILabel(ui, this, (const UTF8Char*)"Sleep Time (us)"));
	this->lblSleepUS->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtSleepUS, UI::GUITextBox(ui, this, (const UTF8Char*)"100"));
	this->txtSleepUS->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->btnSleepUS, UI::GUIButton(ui, this, (const UTF8Char*)"Test"));
	this->btnSleepUS->SetRect(204, 28, 75, 23, false);
	this->btnSleepUS->HandleButtonClick(OnSleepUSClicked, this);
	NEW_CLASS(this->lblActualTime, UI::GUILabel(ui, this, (const UTF8Char*)"Actual Time (s)"));
	this->lblActualTime->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtActualTime, UI::GUITextBox(ui, this, (const UTF8Char*)""));
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
