#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHTTPForwarderForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

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
		me->ui->ShowMsgOK(CSTR("Forward URL must be started with http:// or https://"), CSTR("HTTP Forwarder"), me);
		return;
	}
	Optional<Net::SSLEngine> ssl = 0;

	if (me->chkSSL->IsChecked())
	{
		NotNullPtr<Crypto::Cert::X509Cert> sslCert;
		NotNullPtr<Crypto::Cert::X509File> sslKey;
		if (!sslCert.Set(me->sslCert) || !sslKey.Set(me->sslKey))
		{
			me->ui->ShowMsgOK(CSTR("Please select SSL Cert/Key First"), CSTR("HTTP Forwarder"), me);
			return;
		}
		ssl = me->ssl;
		NotNullPtr<Net::SSLEngine> nnssl;
		if (!ssl.SetTo(nnssl) || !nnssl->ServerSetCertsASN1(sslCert, sslKey, me->caCerts))
		{
			me->ui->ShowMsgOK(CSTR("Error in initializing Cert/Key"), CSTR("HTTP Forwarder"), me);
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
			fwdHdlr.Delete();
			me->ui->ShowMsgOK(CSTR("Error in listening to port"), CSTR("HTTP Forwarder"), me);
		}
		else
		{
			me->fwdHdlr = fwdHdlr.Ptr();
			if (me->chkLog->IsChecked())
			{
				NEW_CLASS(me->fwdLog, IO::LogTool());
				Text::CStringNN logPath;
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
				me->ui->ShowMsgOK(CSTR("Error in starting HTTP Server"), CSTR("HTTP Forwarder"), me);
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
		SDEL_CLASS(me->fwdHdlr);
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
	SDEL_CLASS(me->fwdHdlr);
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
	i = this->caCerts.GetCount();
	while (i-- > 0)
	{
		this->caCerts.GetItem(i).Delete();
	}
	this->caCerts.Clear();
}

SSWR::AVIRead::AVIRHTTPForwarderForm::AVIRHTTPForwarderForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
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

	this->lblPort = ui->NewLabel(*this, CSTR("Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(*this, CSTR("12345"));
	this->txtPort->SetRect(104, 4, 50, 23, false);
	this->lblFwdURL = ui->NewLabel(*this, CSTR("Forward URL"));
	this->lblFwdURL->SetRect(4, 28, 100, 23, false);
	this->txtFwdURL = ui->NewTextBox(*this, CSTR(""));
	this->txtFwdURL->SetRect(104, 28, 500, 23, false);
	this->lblLog = ui->NewLabel(*this, CSTR("Log"));
	this->lblLog->SetRect(4, 52, 100, 23, false);
	this->chkLog = ui->NewCheckBox(*this, CSTR("Enable"), false);
	this->chkLog->SetRect(104, 52, 100, 23, false);
	this->chkLogContent = ui->NewCheckBox(*this, CSTR("Log Content"), false);
	this->chkLogContent->SetRect(204, 52, 100, 23, false);
	this->lblSSL = ui->NewLabel(*this, CSTR("SSL"));
	this->lblSSL->SetRect(4, 76, 100, 23, false);
	this->chkSSL = ui->NewCheckBox(*this, CSTR("Enable"), false);
	this->chkSSL->SetRect(104, 76, 100, 23, false);
	this->btnSSLCert = ui->NewButton(*this, CSTR("Cert/Key"));
	this->btnSSLCert->SetRect(204, 76, 75,23, false);
	this->btnSSLCert->HandleButtonClick(OnSSLCertClicked, this);
	this->lblSSLCert = ui->NewLabel(*this, CSTR(""));
	this->lblSSLCert->SetRect(284, 76, 200, 23, false);
	this->lblAllowKA = ui->NewLabel(*this, CSTR("Allow KA"));
	this->lblAllowKA->SetRect(4, 100, 100, 23, false);
	this->chkAllowKA = ui->NewCheckBox(*this, CSTR("Enable"), false);
	this->chkAllowKA->SetRect(104, 100, 100, 23, false);
	this->btnStart = ui->NewButton(*this, CSTR("Start"));
	this->btnStart->SetRect(104, 124, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
	this->btnStop = ui->NewButton(*this, CSTR("Stop"));
	this->btnStop->SetRect(184, 124, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClick, this);
}

SSWR::AVIRead::AVIRHTTPForwarderForm::~AVIRHTTPForwarderForm()
{
	SDEL_CLASS(this->svr);
	SDEL_CLASS(this->fwdHdlr);
	SDEL_CLASS(this->fwdLog);
	this->ssl.Delete();
	SDEL_CLASS(this->sslCert);
	SDEL_CLASS(this->sslKey);
	this->ClearCACerts();
}

void SSWR::AVIRead::AVIRHTTPForwarderForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
