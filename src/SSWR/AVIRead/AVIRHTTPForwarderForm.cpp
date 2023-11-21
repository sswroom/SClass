#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHTTPForwarderForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRHTTPForwarderForm::OnStartClick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPForwarderForm *me = (SSWR::AVIRead::AVIRHTTPForwarderForm*)userObj;
	if (me->svr)
	{
		return;
	}
	UInt16 port = 0;
	Bool valid = true;
	Text::StringBuilderUTF8 sb;
	me->txtPort->GetText(sb);
	Text::StrToUInt16S(sb.ToString(), port, 0);
	sb.ClearStr();
	me->txtFwdURL->GetText(sb);
	if (!sb.StartsWith(UTF8STRC("http://")) && !sb.StartsWith(UTF8STRC("https://")))
	{
		UI::MessageDialog::ShowDialog(CSTR("Forward URL must be started with http:// or https://"), CSTR("HTTP Forwarder"), me);
		return;
	}
	Net::SSLEngine *ssl = 0;

	if (me->chkSSL->IsChecked())
	{
		NotNullPtr<Crypto::Cert::X509Cert> sslCert;
		NotNullPtr<Crypto::Cert::X509File> sslKey;
		if (!sslCert.Set(me->sslCert) || !sslKey.Set(me->sslKey))
		{
			UI::MessageDialog::ShowDialog(CSTR("Please select SSL Cert/Key First"), CSTR("HTTP Forwarder"), me);
			return;
		}
		ssl = me->ssl;
		if (!ssl->ServerSetCertsASN1(sslCert, sslKey, me->caCerts))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in initializing Cert/Key"), CSTR("HTTP Forwarder"), me);
			return;
		}
	}
	if (port > 0 && port < 65535)
	{
		NotNullPtr<Net::WebServer::HTTPForwardHandler> fwdHdlr;
		NEW_CLASSNN(fwdHdlr, Net::WebServer::HTTPForwardHandler(me->core->GetSocketFactory(), me->ssl, sb.ToCString(), Net::WebServer::HTTPForwardHandler::ForwardType::Transparent));
		NEW_CLASS(me->svr, Net::WebServer::WebListener(me->core->GetSocketFactory(), ssl, fwdHdlr, port, 120, 2, Sync::ThreadUtil::GetThreadCnt(), CSTR("sswr"), false, me->chkAllowKA->IsChecked()?Net::WebServer::KeepAlive::Always:Net::WebServer::KeepAlive::Default, false));
		if (me->svr->IsError())
		{
			valid = false;
			SDEL_CLASS(me->svr);
			fwdHdlr->Release();
			UI::MessageDialog::ShowDialog(CSTR("Error in listening to port"), CSTR("HTTP Forwarder"), me);
		}
		else
		{
			me->fwdHdlr = fwdHdlr.Ptr();
			if (me->chkLog->IsChecked())
			{
				NEW_CLASS(me->fwdLog, IO::LogTool());
				Text::CString logPath;
				if (IO::Path::PATH_SEPERATOR == '/')
					logPath = CSTR("log/fwd");
				else
					logPath = CSTR("log\\fwd");
				me->fwdLog->AddFileLog(logPath, IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, 0, 0);
				me->fwdHdlr->SetLog(me->fwdLog, me->chkLogContent->IsChecked());
			}
			if (!me->svr->Start())
			{
				valid = false;
				UI::MessageDialog::ShowDialog(CSTR("Error in starting HTTP Server"), CSTR("HTTP Forwarder"), me);
			}
		}
	}

	if (valid)
	{
		me->txtPort->SetReadOnly(true);
		me->txtFwdURL->SetReadOnly(true);
		me->chkAllowKA->SetEnabled(false);
		me->chkSSL->SetEnabled(false);
		me->btnSSLCert->SetEnabled(false);
	}
	else
	{
		SDEL_CLASS(me->svr);
		if (me->fwdHdlr)
		{
			me->fwdHdlr->Release();
			me->fwdHdlr = 0;
		}
		SDEL_CLASS(me->fwdLog);
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPForwarderForm::OnStopClick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPForwarderForm *me = (SSWR::AVIRead::AVIRHTTPForwarderForm*)userObj;
	if (me->svr == 0)
	{
		return;
	}
	SDEL_CLASS(me->svr);
	if (me->fwdHdlr)
	{
		me->fwdHdlr->Release();
		me->fwdHdlr = 0;
	}
	SDEL_CLASS(me->fwdLog);
	me->txtPort->SetReadOnly(false);
	me->txtFwdURL->SetReadOnly(false);
	me->chkAllowKA->SetEnabled(true);
	me->chkSSL->SetEnabled(true);
	me->btnSSLCert->SetEnabled(true);
}

void __stdcall SSWR::AVIRead::AVIRHTTPForwarderForm::OnSSLCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPForwarderForm *me = (SSWR::AVIRead::AVIRHTTPForwarderForm*)userObj;
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(0, me->ui, me->core, me->ssl, me->sslCert, me->sslKey, me->caCerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		SDEL_CLASS(me->sslCert);
		SDEL_CLASS(me->sslKey);
		me->ClearCACerts();
		me->sslCert = frm.GetCert();
		me->sslKey = frm.GetKey();
		frm.GetCACerts(me->caCerts);
		Text::StringBuilderUTF8 sb;
		me->sslCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		me->sslKey->ToShortString(sb);
		me->lblSSLCert->SetText(sb.ToCString());
	}
}

void SSWR::AVIRead::AVIRHTTPForwarderForm::ClearCACerts()
{
	UOSInt i;
	Crypto::Cert::X509Cert *cert;
	i = this->caCerts.GetCount();
	while (i-- > 0)
	{
		cert = this->caCerts.GetItem(i);
		DEL_CLASS(cert);
	}
	this->caCerts.Clear();
}

SSWR::AVIRead::AVIRHTTPForwarderForm::AVIRHTTPForwarderForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("HTTP Forwarder"));
	this->SetFont(0, 0, 8.25, false);
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->sslCert = 0;
	this->sslKey = 0;
	this->svr = 0;
	this->fwdLog = 0;
	this->fwdHdlr = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblPort, UI::GUILabel(ui, *this, CSTR("Port")));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, *this, CSTR("12345")));
	this->txtPort->SetRect(104, 4, 50, 23, false);
	NEW_CLASS(this->lblFwdURL, UI::GUILabel(ui, *this, CSTR("Forward URL")));
	this->lblFwdURL->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtFwdURL, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtFwdURL->SetRect(104, 28, 500, 23, false);
	NEW_CLASS(this->lblLog, UI::GUILabel(ui, *this, CSTR("Log")));
	this->lblLog->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->chkLog, UI::GUICheckBox(ui, *this, CSTR("Enable"), false));
	this->chkLog->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->chkLogContent, UI::GUICheckBox(ui, *this, CSTR("Log Content"), false));
	this->chkLogContent->SetRect(204, 52, 100, 23, false);
	NEW_CLASS(this->lblSSL, UI::GUILabel(ui, *this, CSTR("SSL")));
	this->lblSSL->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->chkSSL, UI::GUICheckBox(ui, *this, CSTR("Enable"), false));
	this->chkSSL->SetRect(104, 76, 100, 23, false);
	NEW_CLASS(this->btnSSLCert, UI::GUIButton(ui, *this, CSTR("Cert/Key")));
	this->btnSSLCert->SetRect(204, 76, 75,23, false);
	this->btnSSLCert->HandleButtonClick(OnSSLCertClicked, this);
	NEW_CLASS(this->lblSSLCert, UI::GUILabel(ui, *this, CSTR("")));
	this->lblSSLCert->SetRect(284, 76, 200, 23, false);
	NEW_CLASS(this->lblAllowKA, UI::GUILabel(ui, *this, CSTR("Allow KA")));
	this->lblAllowKA->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->chkAllowKA, UI::GUICheckBox(ui, *this, CSTR("Enable"), false));
	this->chkAllowKA->SetRect(104, 100, 100, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, *this, CSTR("Start")));
	this->btnStart->SetRect(104, 124, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
	NEW_CLASS(this->btnStop, UI::GUIButton(ui, *this, CSTR("Stop")));
	this->btnStop->SetRect(184, 124, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClick, this);
}

SSWR::AVIRead::AVIRHTTPForwarderForm::~AVIRHTTPForwarderForm()
{
	SDEL_CLASS(this->svr);
	if (this->fwdHdlr)
	{
		this->fwdHdlr->Release();
		this->fwdHdlr = 0;
	}
	SDEL_CLASS(this->fwdLog);
	SDEL_CLASS(this->ssl);
	SDEL_CLASS(this->sslCert);
	SDEL_CLASS(this->sslKey);
	this->ClearCACerts();
}

void SSWR::AVIRead::AVIRHTTPForwarderForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
