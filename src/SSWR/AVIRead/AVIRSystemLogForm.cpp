#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRSystemLogForm.h"

SSWR::AVIRead::AVIRSystemLogForm::AVIRSystemLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("System Log"));
	this->SetFont(nullptr, 8.25, false);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->logger = UI::ListBoxLogger::CreateUI(*this, ui, *this, 200, false);
	this->core->GetLog()->AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);

}

SSWR::AVIRead::AVIRSystemLogForm::~AVIRSystemLogForm()
{
	this->core->GetLog()->RemoveLogHandler(this->logger);
	this->logger.Delete();
}

void SSWR::AVIRead::AVIRSystemLogForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
