#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRSSLInfoForm.h"

void __stdcall SSWR::AVIRead::AVIRSSLInfoForm::OnCheckClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSSLInfoForm *me = (SSWR::AVIRead::AVIRSSLInfoForm *)userObj;
	Text::StringBuilderUTF8 sb;
	UInt16 port;
	me->txtPort->GetText(sb);
	if (!sb.ToUInt16(&port))
	{
		me->txtStatus->SetText(CSTR("Please enter valid port"));
		return;
	}
	sb.ClearStr();
	me->txtHost->GetText(sb);
	Net::SocketUtil::AddressInfo addr;
	if (sb.GetLength() == 0)
	{
		me->txtStatus->SetText(CSTR("Please enter Host name"));
		return;
	}
	else if (!me->sockf->DNSResolveIP(sb.ToCString(), &addr))
	{
		me->txtStatus->SetText(CSTR("Error in resolving host name"));
		return;
	}

	Socket *s;
	if (addr.addrType == Net::AddrType::IPv4)
	{
		s = me->sockf->CreateTCPSocketv4();
	}
	else if (addr.addrType == Net::AddrType::IPv6)
	{
		s = me->sockf->CreateTCPSocketv6();
	}
	else
	{
		me->txtStatus->SetText(CSTR("Error in address type"));
		return;
	}
	if (s == 0)
	{
		me->txtStatus->SetText(CSTR("Error in creating socket"));
		return;
	}
	if (!me->sockf->Connect(s, &addr, port, 30000))
	{
		me->sockf->DestroySocket(s);
		me->txtStatus->SetText(CSTR("Error in connecting to remote host"));
		return;
	}
	Net::SSLEngine::ErrorType err;
	Net::SSLClient *cli = me->ssl->ClientInit(s, sb.ToCString(), &err);
	if (cli == 0)
	{
		me->sockf->DestroySocket(s);
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Error in initializing SSL: "));
		sb.Append(Net::SSLEngine::ErrorTypeGetName(err));
		me->txtStatus->SetText(sb.ToCString());
		return;
	}
	SDEL_CLASS(me->currCerts);
	NotNullPtr<const Data::ReadingList<Crypto::Cert::Certificate *>> certs;
	if (certs.Set(cli->GetRemoteCerts()))
	{
		me->currCerts = Crypto::Cert::X509File::CreateFromCerts(certs).Ptr();
		Text::StringBuilderUTF8 sb;
		me->currCerts->ToString(sb);
		me->txtCert->SetText(sb.ToCString());
	}
	else
	{
		me->txtCert->SetText(CSTR(""));
	}
	DEL_CLASS(cli);
	me->txtStatus->SetText(CSTR("Success"));
}

void __stdcall SSWR::AVIRead::AVIRSSLInfoForm::OnCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSSLInfoForm *me = (SSWR::AVIRead::AVIRSSLInfoForm *)userObj;
	if (me->currCerts)
	{
		me->core->OpenObject(me->currCerts->Clone().Ptr());
	}
}

SSWR::AVIRead::AVIRSSLInfoForm::AVIRSSLInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("SSL Info"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->sockf = this->core->GetSocketFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->sockf, true);
	this->currCerts = 0;
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
	this->txtStatus->SetRect(104, 76, 400, 23, false);
	this->txtStatus->SetReadOnly(true);
	NEW_CLASS(this->lblCert, UI::GUILabel(ui, this, CSTR("Cert")));
	this->lblCert->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtCert, UI::GUITextBox(ui, this, CSTR(""), true));
	this->txtCert->SetRect(104, 100, 150, 144, false);
	this->txtCert->SetReadOnly(true);
	NEW_CLASS(this->btnCert, UI::GUIButton(ui, this, CSTR("View")));
	this->btnCert->SetRect(254, 100, 75, 23, false);
	this->btnCert->HandleButtonClick(OnCertClicked, this);
}

SSWR::AVIRead::AVIRSSLInfoForm::~AVIRSSLInfoForm()
{
	SDEL_CLASS(this->ssl);
	SDEL_CLASS(this->currCerts);
}

void SSWR::AVIRead::AVIRSSLInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
