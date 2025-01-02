#include "Stdafx.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRRadioScanForm.h"

SSWR::AVIRead::AVIRRadioScanForm::AVIRRadioScanForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 156, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Radio Scan"));
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

}

SSWR::AVIRead::AVIRRadioScanForm::~AVIRRadioScanForm()
{
}

void SSWR::AVIRead::AVIRRadioScanForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
