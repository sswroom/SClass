#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRTimestampForm.h"

void __stdcall SSWR::AVIRead::AVIRTimestampForm::OnEpochSecClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTimestampForm *me = (SSWR::AVIRead::AVIRTimestampForm *)userObj;
	Text::StringBuilderUTF8 sb;
	Int64 v;
	me->txtValue->GetText(&sb);
	if (sb.ToInt64(&v))
	{
		me->DisplayTime(Data::Timestamp::FromEpochSec(v, 0));
	}
}

void __stdcall SSWR::AVIRead::AVIRTimestampForm::OnEpochMSClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTimestampForm *me = (SSWR::AVIRead::AVIRTimestampForm *)userObj;
	Text::StringBuilderUTF8 sb;
	Int64 v;
	me->txtValue->GetText(&sb);
	if (sb.ToInt64(&v))
	{
		me->DisplayTime(Data::Timestamp::FromEpochMS(v, 0));
	}
}

void __stdcall SSWR::AVIRead::AVIRTimestampForm::OnEpochUSClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTimestampForm *me = (SSWR::AVIRead::AVIRTimestampForm *)userObj;
	Text::StringBuilderUTF8 sb;
	Int64 v;
	me->txtValue->GetText(&sb);
	if (sb.ToInt64(&v))
	{
		me->DisplayTime(Data::Timestamp::FromEpochUS(v, 0));
	}
}

void __stdcall SSWR::AVIRead::AVIRTimestampForm::OnEpochNSClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTimestampForm *me = (SSWR::AVIRead::AVIRTimestampForm *)userObj;
	Text::StringBuilderUTF8 sb;
	Int64 v;
	me->txtValue->GetText(&sb);
	if (sb.ToInt64(&v))
	{
		me->DisplayTime(Data::Timestamp::FromEpochNS(v, 0));
	}
}

void __stdcall SSWR::AVIRead::AVIRTimestampForm::OnDotNetTicksClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTimestampForm *me = (SSWR::AVIRead::AVIRTimestampForm *)userObj;
	Text::StringBuilderUTF8 sb;
	Int64 v;
	me->txtValue->GetText(&sb);
	if (sb.ToInt64(&v))
	{
		me->DisplayTime(Data::Timestamp::FromDotNetTicks(v, 0));
	}
}

void SSWR::AVIRead::AVIRTimestampForm::DisplayTime(Data::Timestamp ts)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	sptr = ts.ToUTCTime().ToString(sbuff);
	this->txtUTCTime->SetText(CSTRP(sbuff, sptr));
	sptr = ts.ToLocalTime().ToString(sbuff);
	this->txtLocalTime->SetText(CSTRP(sbuff, sptr));
}

SSWR::AVIRead::AVIRTimestampForm::AVIRTimestampForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 480, 120, ui)
{
	this->core = core;
	this->SetText(CSTR("Timestamp"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblValue, UI::GUILabel(ui, this, CSTR("Value")));
	this->lblValue->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtValue, UI::GUITextBox(ui, this, CSTR("")));
	this->txtValue->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->btnEpochSec, UI::GUIButton(ui, this, CSTR("Epoch Second")));
	this->btnEpochSec->SetRect(4, 28, 75, 23, false);
	this->btnEpochSec->HandleButtonClick(OnEpochSecClicked, this);
	NEW_CLASS(this->btnEpochMS, UI::GUIButton(ui, this, CSTR("Epoch ms")));
	this->btnEpochMS->SetRect(84, 28, 75, 23, false);
	this->btnEpochMS->HandleButtonClick(OnEpochMSClicked, this);
	NEW_CLASS(this->btnEpochUS, UI::GUIButton(ui, this, CSTR("Epoch us")));
	this->btnEpochUS->SetRect(164, 28, 75, 23, false);
	this->btnEpochUS->HandleButtonClick(OnEpochUSClicked, this);
	NEW_CLASS(this->btnEpochNS, UI::GUIButton(ui, this, CSTR("Epoch ns")));
	this->btnEpochNS->SetRect(244, 28, 75, 23, false);
	this->btnEpochNS->HandleButtonClick(OnEpochNSClicked, this);
	NEW_CLASS(this->btnDotNetTicks, UI::GUIButton(ui, this, CSTR(".NET Ticks")));
	this->btnDotNetTicks->SetRect(324, 28, 75, 23, false);
	this->btnDotNetTicks->HandleButtonClick(OnDotNetTicksClicked, this);
	NEW_CLASS(this->lblUTCTime, UI::GUILabel(ui, this, CSTR("UTC Time")));
	this->lblUTCTime->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtUTCTime, UI::GUITextBox(ui, this, CSTR("")));
	this->txtUTCTime->SetRect(104, 52, 200, 23, false);
	this->txtUTCTime->SetReadOnly(true);
	NEW_CLASS(this->lblLocalTime, UI::GUILabel(ui, this, CSTR("Local Time")));
	this->lblLocalTime->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtLocalTime, UI::GUITextBox(ui, this, CSTR("")));
	this->txtLocalTime->SetRect(104, 76, 200, 23, false);
	this->txtLocalTime->SetReadOnly(true);
}

SSWR::AVIRead::AVIRTimestampForm::~AVIRTimestampForm()
{
}

void SSWR::AVIRead::AVIRTimestampForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
