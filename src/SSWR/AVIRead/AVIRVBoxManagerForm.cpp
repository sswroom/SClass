#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRVBoxManagerForm.h"

SSWR::AVIRead::AVIRVBoxManagerForm::AVIRVBoxManagerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("VirtualBox Manager"));
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblVersion, UI::GUILabel(ui, this, CSTR("Version")));
	this->lblVersion->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtVersion, UI::GUITextBox(ui, this, CSTR("")));
	this->txtVersion->SetReadOnly(true);
	this->txtVersion->SetRect(104, 4, 200, 23, false);
}

SSWR::AVIRead::AVIRVBoxManagerForm::~AVIRVBoxManagerForm()
{
}

void SSWR::AVIRead::AVIRVBoxManagerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
