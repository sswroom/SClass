#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRHKIDForm.h"
#include "Text/StringTool.h"

void __stdcall SSWR::AVIRead::AVIRHKIDForm::OnCheckClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHKIDForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHKIDForm>();
	Text::StringBuilderUTF8 sb;
	if (me->txtHKID->GetText(sb))
	{
		if (Text::StringTool::IsHKID(sb.ToCString()))
		{
			me->lblHKIDResult->SetText(CSTR("Valid"));
		}
		else
		{
			me->lblHKIDResult->SetText(CSTR("Not valid"));
		}
	}
}

SSWR::AVIRead::AVIRHKIDForm::AVIRHKIDForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 500, 120, ui)
{
	this->core = core;
	this->SetText(CSTR("HKID Validate"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);

	this->lblHKID = ui->NewLabel(*this, CSTR("HKID"));
	this->lblHKID->SetRect(4, 4, 100, 23, false);
	this->txtHKID = ui->NewTextBox(*this, CSTR(""));
	this->txtHKID->SetRect(104, 4, 150, 23, false);
	this->btnCheck = ui->NewButton(*this, CSTR("Check"));
	this->btnCheck->SetRect(254, 4, 75, 23, false);
	this->btnCheck->HandleButtonClick(OnCheckClicked, this);
	this->lblHKIDResult = ui->NewLabel(*this, CSTR(""));
	this->lblHKIDResult->SetRect(334, 4, 100, 23, false);

	this->OnMonitorChanged();
}

SSWR::AVIRead::AVIRHKIDForm::~AVIRHKIDForm()
{
}

void SSWR::AVIRead::AVIRHKIDForm::OnMonitorChanged()
{
	Optional<MonitorHandle> hMon = this->GetHMonitor();
	this->SetDPI(this->core->GetMonitorHDPI(hMon), this->core->GetMonitorDDPI(hMon));
}
