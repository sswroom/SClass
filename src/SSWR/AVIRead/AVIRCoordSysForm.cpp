#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRCoordSysForm.h"

SSWR::AVIRead::AVIRCoordSysForm::AVIRCoordSysForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Math::CoordinateSystem> csys) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Coordinate System"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->csys = csys;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	Text::StringBuilderUTF8 sb;
	this->csys->ToString(sb);
	this->txtDesc = ui->NewTextBox(*this, sb.ToCString(), true);
	this->txtDesc->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtDesc->SetReadOnly(true);
}

SSWR::AVIRead::AVIRCoordSysForm::~AVIRCoordSysForm()
{
	this->csys.Delete();
}

void SSWR::AVIRead::AVIRCoordSysForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
