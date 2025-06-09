#include "Stdafx.h"
#include "SSWR/AVIRead/AVIREdgeAnalyseForm.h"

SSWR::AVIRead::AVIREdgeAnalyseForm::AVIREdgeAnalyseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("Edge Analyse Device"));
	this->SetFont(0, 0, 8.25, false);
	this->OnMonitorChanged();

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpWebHook = this->tcMain->AddTabPage(CSTR("Web Hook"));
}

SSWR::AVIRead::AVIREdgeAnalyseForm::~AVIREdgeAnalyseForm()
{
}

void SSWR::AVIRead::AVIREdgeAnalyseForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
