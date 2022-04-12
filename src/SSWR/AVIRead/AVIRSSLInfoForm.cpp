#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRSSLInfoForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSSLInfoForm::OnCheckClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSSLInfoForm *me = (SSWR::AVIRead::AVIRSSLInfoForm *)userObj;
	Text::StringBuilderUTF8 sb;
	UInt16 port;
	me->txtPort->GetText(&sb);
	if (!sb.ToUInt16(&port))
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter valid port"), CSTR("SSL Info"), me);
		return;
	}
	sb.ClearStr();
	me->txtHost->GetText(&sb);
	Net::SocketUtil::AddressInfo addr;
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter Host name"), CSTR("SSL Info"), me);
		return;
	}
	else if (!me->sockf->DNSResolveIP(sb.ToCString(), &addr))
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in resolving host name"), CSTR("SSL Info"), me);
		return;
	}
}

void __stdcall SSWR::AVIRead::AVIRSSLInfoForm::OnCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSSLInfoForm *me = (SSWR::AVIRead::AVIRSSLInfoForm *)userObj;
}

SSWR::AVIRead::AVIRSSLInfoForm::AVIRSSLInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("SSL Info"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->sockf = this->core->GetSocketFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->sockf, true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblHost, UI::GUILabel(ui, this, CSTR("Host")));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtHost, UI::GUITextBox(ui, this, CSTR("127.0.0.1")));
	this->txtHost->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this, CSTR("Port")));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this, CSTR("443")));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->btnCheck, UI::GUIButton(ui, this, CSTR("Check")));
	this->btnCheck->SetRect(204, 28, 75, 23, false);
	this->btnCheck->HandleButtonClick(OnCheckClicked, this);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, CSTR("Status")));
	this->lblStatus->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this, CSTR("")));
	this->txtStatus->SetRect(104, 76, 150, 23, false);
	this->txtStatus->SetReadOnly(true);
	NEW_CLASS(this->lblCert, UI::GUILabel(ui, this, CSTR("Status")));
	this->lblCert->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtCert, UI::GUITextBox(ui, this, CSTR("")));
	this->txtCert->SetRect(104, 100, 150, 23, false);
	this->txtCert->SetReadOnly(true);
	NEW_CLASS(this->btnCert, UI::GUIButton(ui, this, CSTR("View")));
	this->btnCert->SetRect(254, 100, 75, 23, false);
	this->btnCert->HandleButtonClick(OnCertClicked, this);
}

SSWR::AVIRead::AVIRSSLInfoForm::~AVIRSSLInfoForm()
{
}

void SSWR::AVIRead::AVIRSSLInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
