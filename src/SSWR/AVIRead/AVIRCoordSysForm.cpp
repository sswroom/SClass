#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRCoordSysForm.h"

SSWR::AVIRead::AVIRCoordSysForm::AVIRCoordSysForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Math::CoordinateSystem *csys) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"Coordinate System");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->csys = csys;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	Text::StringBuilderUTF8 sb;
	this->csys->ToString(&sb);
	NEW_CLASS(this->txtDesc, UI::GUITextBox(ui, this, sb.ToString(), true));
	this->txtDesc->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtDesc->SetReadOnly(true);
}

SSWR::AVIRead::AVIRCoordSysForm::~AVIRCoordSysForm()
{
	DEL_CLASS(this->csys);
}

void SSWR::AVIRead::AVIRCoordSysForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}