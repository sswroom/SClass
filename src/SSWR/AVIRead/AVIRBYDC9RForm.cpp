#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRBYDC9RForm.h"

SSWR::AVIRead::AVIRBYDC9RForm::AVIRBYDC9RForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("BYD C9R"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

}

SSWR::AVIRead::AVIRBYDC9RForm::~AVIRBYDC9RForm()
{
}

void SSWR::AVIRead::AVIRBYDC9RForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
