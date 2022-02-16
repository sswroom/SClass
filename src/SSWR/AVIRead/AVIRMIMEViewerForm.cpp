#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRMIMEViewerForm.h"

SSWR::AVIRead::AVIRMIMEViewerForm::AVIRMIMEViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Text::IMIMEObj *obj) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("MIME Viewer"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->obj = obj;
	this->sess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->viewer = SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer::CreateViewer(core, ui, this, this->sess, obj);
}

SSWR::AVIRead::AVIRMIMEViewerForm::~AVIRMIMEViewerForm()
{
	this->ClearChildren();

	SDEL_CLASS(this->viewer);
	SDEL_CLASS(this->obj);
	this->core->GetColorMgr()->DeleteSess(this->sess);
}

void SSWR::AVIRead::AVIRMIMEViewerForm::OnMonitorChanged()
{
	MonitorHandle *hMon = this->GetHMonitor();
	this->SetDPI(this->core->GetMonitorHDPI(hMon), this->core->GetMonitorDDPI(hMon));
	this->sess->ChangeMonitor(hMon);
}
