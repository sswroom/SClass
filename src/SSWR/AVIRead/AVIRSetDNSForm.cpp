#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRSetDNSForm.h"

void __stdcall SSWR::AVIRead::AVIRSetDNSForm::OnOKClick(void *userObj)
{
	SSWR::AVIRead::AVIRSetDNSForm *me = (SSWR::AVIRead::AVIRSetDNSForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtDNSServer->GetText(sb);
	if (sb.leng > 0)
	{
		if (me->core->GetSocketFactory()->ForceDNSServer(sb.ToCString()))
		{
			me->Close();
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Input value is not valid IP address"), CSTR("Set DNS Server"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSetDNSForm::OnCancelClick(void *userObj)
{
	SSWR::AVIRead::AVIRSetDNSForm *me = (SSWR::AVIRead::AVIRSetDNSForm*)userObj;
	me->Close();
}

SSWR::AVIRead::AVIRSetDNSForm::AVIRSetDNSForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 456, 120, ui)
{
	this->SetText(CSTR("Set DNS Server"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblDNSServer = ui->NewLabel(*this, CSTR("DNS Server"));
	this->lblDNSServer->SetRect(8, 8, 100, 23, false);
	this->txtDNSServer = ui->NewTextBox(*this, CSTR("8.8.8.8"));
	this->txtDNSServer->SetRect(8, 32, 256, 23, false);


	this->btnOK = ui->NewButton(*this, CSTR("&OK"));
	this->btnOK->SetRect(112, 56, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(192, 56, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
}

SSWR::AVIRead::AVIRSetDNSForm::~AVIRSetDNSForm()
{
}

void SSWR::AVIRead::AVIRSetDNSForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
