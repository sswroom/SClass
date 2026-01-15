#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRTimestampForm.h"

void __stdcall SSWR::AVIRead::AVIRTimestampForm::OnEpochSecClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTimestampForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTimestampForm>();
	Text::StringBuilderUTF8 sb;
	Int64 v;
	me->txtValue->GetText(sb);
	UInt32 ns = 0;
	UIntOS i = sb.IndexOf('.');
	if (i != INVALID_INDEX)
	{
		Text::CStringNN decimals = sb.ToCString().Substring(i + 1);
		ns = decimals.ToUInt32();
		sb.TrimToLength(i);
		i = decimals.leng;
		while (i < 9)
		{
			ns *= 10;
			i++;
		}
	}
	if (sb.ToInt64(v))
	{
		me->DisplayTime(Data::Timestamp(Data::TimeInstant(v, ns), 0));
	}
}

void __stdcall SSWR::AVIRead::AVIRTimestampForm::OnEpochMSClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTimestampForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTimestampForm>();
	Text::StringBuilderUTF8 sb;
	Int64 v;
	me->txtValue->GetText(sb);
	if (sb.ToInt64(v))
	{
		me->DisplayTime(Data::Timestamp::FromEpochMS(v, 0));
	}
}

void __stdcall SSWR::AVIRead::AVIRTimestampForm::OnEpochUSClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTimestampForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTimestampForm>();
	Text::StringBuilderUTF8 sb;
	Int64 v;
	me->txtValue->GetText(sb);
	if (sb.ToInt64(v))
	{
		me->DisplayTime(Data::Timestamp::FromEpochUS(v, 0));
	}
}

void __stdcall SSWR::AVIRead::AVIRTimestampForm::OnEpochNSClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTimestampForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTimestampForm>();
	Text::StringBuilderUTF8 sb;
	Int64 v;
	me->txtValue->GetText(sb);
	if (sb.ToInt64(v))
	{
		me->DisplayTime(Data::Timestamp::FromEpochNS(v, 0));
	}
}

void __stdcall SSWR::AVIRead::AVIRTimestampForm::OnDotNetTicksClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTimestampForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTimestampForm>();
	Text::StringBuilderUTF8 sb;
	Int64 v;
	me->txtValue->GetText(sb);
	if (sb.ToInt64(v))
	{
		me->DisplayTime(Data::Timestamp::FromDotNetTicks(v, 0));
	}
}

void __stdcall SSWR::AVIRead::AVIRTimestampForm::OnStrConvClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTimestampForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTimestampForm>();
	Text::StringBuilderUTF8 sb;
	me->txtStr->GetText(sb);
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
	UnsafeArray<UTF8Char> sptr;
	sptr = ts.ToUTCTime().ToString(sbuff);
	this->txtUTCTime->SetText(CSTRP(sbuff, sptr));
	sptr = ts.ToLocalTime().ToString(sbuff);
	this->txtLocalTime->SetText(CSTRP(sbuff, sptr));
}

SSWR::AVIRead::AVIRTimestampForm::AVIRTimestampForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 480, 216, ui)
{
	this->core = core;
	this->SetText(CSTR("Timestamp"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpValue = this->tcMain->AddTabPage(CSTR("Value"));
	this->lblValue = ui->NewLabel(this->tpValue, CSTR("Value"));
	this->lblValue->SetRect(4, 4, 100, 23, false);
	this->txtValue = ui->NewTextBox(this->tpValue, CSTR(""));
	this->txtValue->SetRect(104, 4, 150, 23, false);
	this->btnEpochSec = ui->NewButton(this->tpValue, CSTR("Epoch Second"));
	this->btnEpochSec->SetRect(4, 28, 75, 23, false);
	this->btnEpochSec->HandleButtonClick(OnEpochSecClicked, this);
	this->btnEpochMS = ui->NewButton(this->tpValue, CSTR("Epoch ms"));
	this->btnEpochMS->SetRect(84, 28, 75, 23, false);
	this->btnEpochMS->HandleButtonClick(OnEpochMSClicked, this);
	this->btnEpochUS = ui->NewButton(this->tpValue, CSTR("Epoch us"));
	this->btnEpochUS->SetRect(164, 28, 75, 23, false);
	this->btnEpochUS->HandleButtonClick(OnEpochUSClicked, this);
	this->btnEpochNS = ui->NewButton(this->tpValue, CSTR("Epoch ns"));
	this->btnEpochNS->SetRect(244, 28, 75, 23, false);
	this->btnEpochNS->HandleButtonClick(OnEpochNSClicked, this);
	this->btnDotNetTicks = ui->NewButton(this->tpValue, CSTR(".NET Ticks"));
	this->btnDotNetTicks->SetRect(324, 28, 75, 23, false);
	this->btnDotNetTicks->HandleButtonClick(OnDotNetTicksClicked, this);
	this->lblUTCTime = ui->NewLabel(this->tpValue, CSTR("UTC Time"));
	this->lblUTCTime->SetRect(4, 52, 100, 23, false);
	this->txtUTCTime = ui->NewTextBox(this->tpValue, CSTR(""));
	this->txtUTCTime->SetRect(104, 52, 200, 23, false);
	this->txtUTCTime->SetReadOnly(true);
	this->lblLocalTime = ui->NewLabel(this->tpValue, CSTR("Local Time"));
	this->lblLocalTime->SetRect(4, 76, 100, 23, false);
	this->txtLocalTime = ui->NewTextBox(this->tpValue, CSTR(""));
	this->txtLocalTime->SetRect(104, 76, 200, 23, false);
	this->txtLocalTime->SetReadOnly(true);

	this->tpStr = this->tcMain->AddTabPage(CSTR("String"));
	this->lblStr = ui->NewLabel(this->tpStr, CSTR("Date String"));
	this->lblStr->SetRect(4, 4, 100, 23, false);
	this->txtStr = ui->NewTextBox(this->tpStr, CSTR(""));
	this->txtStr->SetRect(104, 4, 200, 23, false);
	this->btnStrConv = ui->NewButton(this->tpStr, CSTR("Convert"));
	this->btnStrConv->SetRect(104, 28, 75, 23, false);
	this->btnStrConv->HandleButtonClick(OnStrConvClicked, this);
	this->lblStrEpochSec = ui->NewLabel(this->tpStr, CSTR("Epoch Second"));
	this->lblStrEpochSec->SetRect(4, 52, 100, 23, false);
	this->txtStrEpochSec = ui->NewTextBox(this->tpStr, CSTR(""));
	this->txtStrEpochSec->SetRect(104, 52, 200, 23, false);
	this->txtStrEpochSec->SetReadOnly(true);
	this->lblStrEpochMS = ui->NewLabel(this->tpStr, CSTR("Epoch MS"));
	this->lblStrEpochMS->SetRect(4, 76, 100, 23, false);
	this->txtStrEpochMS = ui->NewTextBox(this->tpStr, CSTR(""));
	this->txtStrEpochMS->SetRect(104, 76, 200, 23, false);
	this->txtStrEpochMS->SetReadOnly(true);
	this->lblStrEpochNS = ui->NewLabel(this->tpStr, CSTR("Epoch NS"));
	this->lblStrEpochNS->SetRect(4, 100, 100, 23, false);
	this->txtStrEpochNS = ui->NewTextBox(this->tpStr, CSTR(""));
	this->txtStrEpochNS->SetRect(104, 100, 200, 23, false);
	this->txtStrEpochNS->SetReadOnly(true);
	this->lblStrDotNetTicks = ui->NewLabel(this->tpStr, CSTR(".NET Ticks"));
	this->lblStrDotNetTicks->SetRect(4, 124, 100, 23, false);
	this->txtStrDotNetTicks = ui->NewTextBox(this->tpStr, CSTR(""));
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
