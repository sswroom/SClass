#include "Stdafx.h"
#include "IO/PowerInfo.h"
#include "SSWR/AVIRead/AVIRPowerForm.h"

void __stdcall SSWR::AVIRead::AVIRPowerForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRPowerForm *me = (SSWR::AVIRead::AVIRPowerForm*)userObj;
	IO::PowerInfo::PowerStatus power;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	if (IO::PowerInfo::GetPowerStatus(&power))
	{
		if (power.hasBattery)
		{
			sptr = Text::StrConcatC(Text::StrUInt32(sbuff, power.batteryPercent), UTF8STRC("%"));
			me->txtBattery->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, power.timeLeftSec);
			me->txtTimeLeft->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			me->txtBattery->SetText(CSTR("Battery not found"));
			me->txtTimeLeft->SetText(CSTR("-"));
		}
	}
	else
	{
		me->txtBattery->SetText(CSTR("Cannot read battery status"));
		me->txtTimeLeft->SetText(CSTR("-"));
	}
	Bool noSystemOff = me->chkNoSleep->IsChecked();
	Bool noDispOff = me->chkNoDispOff->IsChecked();
	if (noSystemOff || noDispOff)
	{
		me->ui->UseDevice(noSystemOff, noDispOff);
	}
}

void __stdcall SSWR::AVIRead::AVIRPowerForm::OnSleepClicked(void *userObj)
{
	SSWR::AVIRead::AVIRPowerForm *me = (SSWR::AVIRead::AVIRPowerForm*)userObj;
	me->ui->Suspend();
}

void __stdcall SSWR::AVIRead::AVIRPowerForm::OnDisplayOffClicked(void *userObj)
{
	SSWR::AVIRead::AVIRPowerForm *me = (SSWR::AVIRead::AVIRPowerForm*)userObj;
	me->ui->DisplayOff();
}

SSWR::AVIRead::AVIRPowerForm::AVIRPowerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 320, 168, ui)
{
	this->SetText(CSTR("Power Control"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblBattery, UI::GUILabel(ui, this, CSTR("Battery")));
	this->lblBattery->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtBattery, UI::GUITextBox(ui, this, CSTR("")));
	this->txtBattery->SetRect(104, 4, 200, 23, false);
	this->txtBattery->SetReadOnly(true);
	NEW_CLASS(this->lblTimeLeft, UI::GUILabel(ui, this, CSTR("Time Left")));
	this->lblTimeLeft->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtTimeLeft, UI::GUITextBox(ui, this, CSTR("")));
	this->txtTimeLeft->SetRect(104, 28, 200, 23, false);
	this->txtTimeLeft->SetReadOnly(true);
	NEW_CLASS(this->chkNoSleep, UI::GUICheckBox(ui, this, CSTR("No Sleep"), false));
	this->chkNoSleep->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->chkNoDispOff, UI::GUICheckBox(ui, this, CSTR("No Disp Off"), false));
	this->chkNoDispOff->SetRect(104, 76, 200, 23, false);
	NEW_CLASS(this->btnSleep, UI::GUIButton(ui, this, CSTR("Sleep")));
	this->btnSleep->SetRect(104, 100, 75, 23, false);
	this->btnSleep->HandleButtonClick(OnSleepClicked, this);
	NEW_CLASS(this->btnDisplayOff, UI::GUIButton(ui, this, CSTR("Display Off")));
	this->btnDisplayOff->SetRect(184, 100, 75, 23, false);
	this->btnDisplayOff->HandleButtonClick(OnDisplayOffClicked, this);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRPowerForm::~AVIRPowerForm()
{
}

void SSWR::AVIRead::AVIRPowerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
