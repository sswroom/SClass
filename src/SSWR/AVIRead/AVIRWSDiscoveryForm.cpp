#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRWSDiscoveryForm.h"

SSWR::AVIRead::AVIRWSDiscoveryForm::AVIRWSDiscoveryForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("WS Discovery"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASSNN(this->wsd, Net::WSDiscoveryClient(core->GetSocketFactory()));
	if (this->wsd->IsError())
	{
		ui->ShowMsgOK(CSTR("Error in initializing WS Discovery Client"), CSTR("WS Discovery"), this);
	}
	else
	{
		this->wsd->Probe(CSTR("NetworkVideoDisplay"), CSTR("http://www.onvif.org/ver10/network/wsdl"), CSTR("dn"));
	}
}

SSWR::AVIRead::AVIRWSDiscoveryForm::~AVIRWSDiscoveryForm()
{
	this->wsd.Delete();
}

void SSWR::AVIRead::AVIRWSDiscoveryForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
