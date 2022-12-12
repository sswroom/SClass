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

void __stdcall SSWR::AVIRead::AVIRTimestampForm::OnStrConvClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTimestampForm *me = (SSWR::AVIRead::AVIRTimestampForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtStr->GetText(&sb);
	Data::Timestamp ts = Data::Timestamp::FromStr(sb.ToCString(), 0);
	sb.ClearStr();
	sb.AppendI64(ts.ToUnixTimestamp());
	me->txtStrEpochSec->SetText(sb.ToCString());
	sb.ClearStr();
	sb.AppendI64(ts.ToEpochMS());
	me->txtStrEpochMS->SetText(sb.ToCString());
	sb.ClearStr();
	sb.AppendI64(ts.ToEpochNS());
	me->txtStrEpochNS->SetText(sb.ToCString());
	sb.ClearStr();
	sb.AppendI64(ts.ToDotNetTicks());
	me->txtStrDotNetTicks->SetText(sb.ToCString());
}

void SSWR::AVIRead::AVIRTimestampForm::DisplayTime(const Data::Timestamp &ts)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	sptr = ts.ToUTCTime().ToString(sbuff);
	this->txtUTCTime->SetText(CSTRP(sbuff, sptr));
	sptr = ts.ToLocalTime().ToString(sbuff);
	this->txtLocalTime->SetText(CSTRP(sbuff, sptr));
}

SSWR::AVIRead::AVIRTimestampForm::AVIRTimestampForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 480, 216, ui)
{
	this->core = core;
	this->SetText(CSTR("Timestamp"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpValue = this->tcMain->AddTabPage(CSTR("Value"));
	NEW_CLASS(this->lblValue, UI::GUILabel(ui, this->tpValue, CSTR("Value")));
	this->lblValue->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtValue, UI::GUITextBox(ui, this->tpValue, CSTR("")));
	this->txtValue->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->btnEpochSec, UI::GUIButton(ui, this->tpValue, CSTR("Epoch Second")));
	this->btnEpochSec->SetRect(4, 28, 75, 23, false);
	this->btnEpochSec->HandleButtonClick(OnEpochSecClicked, this);
	NEW_CLASS(this->btnEpochMS, UI::GUIButton(ui, this->tpValue, CSTR("Epoch ms")));
	this->btnEpochMS->SetRect(84, 28, 75, 23, false);
	this->btnEpochMS->HandleButtonClick(OnEpochMSClicked, this);
	NEW_CLASS(this->btnEpochUS, UI::GUIButton(ui, this->tpValue, CSTR("Epoch us")));
	this->btnEpochUS->SetRect(164, 28, 75, 23, false);
	this->btnEpochUS->HandleButtonClick(OnEpochUSClicked, this);
	NEW_CLASS(this->btnEpochNS, UI::GUIButton(ui, this->tpValue, CSTR("Epoch ns")));
	this->btnEpochNS->SetRect(244, 28, 75, 23, false);
	this->btnEpochNS->HandleButtonClick(OnEpochNSClicked, this);
	NEW_CLASS(this->btnDotNetTicks, UI::GUIButton(ui, this->tpValue, CSTR(".NET Ticks")));
	this->btnDotNetTicks->SetRect(324, 28, 75, 23, false);
	this->btnDotNetTicks->HandleButtonClick(OnDotNetTicksClicked, this);
	NEW_CLASS(this->lblUTCTime, UI::GUILabel(ui, this->tpValue, CSTR("UTC Time")));
	this->lblUTCTime->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtUTCTime, UI::GUITextBox(ui, this->tpValue, CSTR("")));
	this->txtUTCTime->SetRect(104, 52, 200, 23, false);
	this->txtUTCTime->SetReadOnly(true);
	NEW_CLASS(this->lblLocalTime, UI::GUILabel(ui, this->tpValue, CSTR("Local Time")));
	this->lblLocalTime->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtLocalTime, UI::GUITextBox(ui, this->tpValue, CSTR("")));
	this->txtLocalTime->SetRect(104, 76, 200, 23, false);
	this->txtLocalTime->SetReadOnly(true);

	this->tpStr = this->tcMain->AddTabPage(CSTR("String"));
	NEW_CLASS(this->lblStr, UI::GUILabel(ui, this->tpStr, CSTR("Date String")));
	this->lblStr->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtStr, UI::GUITextBox(ui, this->tpStr, CSTR("")));
	this->txtStr->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->btnStrConv, UI::GUIButton(ui, this->tpStr, CSTR("Convert")));
	this->btnStrConv->SetRect(104, 28, 75, 23, false);
	this->btnStrConv->HandleButtonClick(OnStrConvClicked, this);
	NEW_CLASS(this->lblStrEpochSec, UI::GUILabel(ui, this->tpStr, CSTR("Epoch Second")));
	this->lblStrEpochSec->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtStrEpochSec, UI::GUITextBox(ui, this->tpStr, CSTR("")));
	this->txtStrEpochSec->SetRect(104, 52, 200, 23, false);
	this->txtStrEpochSec->SetReadOnly(true);
	NEW_CLASS(this->lblStrEpochMS, UI::GUILabel(ui, this->tpStr, CSTR("Epoch MS")));
	this->lblStrEpochMS->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtStrEpochMS, UI::GUITextBox(ui, this->tpStr, CSTR("")));
	this->txtStrEpochMS->SetRect(104, 76, 200, 23, false);
	this->txtStrEpochMS->SetReadOnly(true);
	NEW_CLASS(this->lblStrEpochNS, UI::GUILabel(ui, this->tpStr, CSTR("Epoch NS")));
	this->lblStrEpochNS->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtStrEpochNS, UI::GUITextBox(ui, this->tpStr, CSTR("")));
	this->txtStrEpochNS->SetRect(104, 100, 200, 23, false);
	this->txtStrEpochNS->SetReadOnly(true);
	NEW_CLASS(this->lblStrDotNetTicks, UI::GUILabel(ui, this->tpStr, CSTR(".NET Ticks")));
	this->lblStrDotNetTicks->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtStrDotNetTicks, UI::GUITextBox(ui, this->tpStr, CSTR("")));
	this->txtStrDotNetTicks->SetRect(104, 124, 200, 23, false);
	this->txtStrDotNetTicks->SetReadOnly(true);
}

SSWR::AVIRead::AVIRTimestampForm::~AVIRTimestampForm()
{
}

void SSWR::AVIRead::AVIRTimestampForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
