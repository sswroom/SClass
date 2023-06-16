#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "IO/BuildTime.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHTTPClientCertTestForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRHTTPClientCertTestForm::OnStartClick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPClientCertTestForm *me = (SSWR::AVIRead::AVIRHTTPClientCertTestForm*)userObj;
	if (me->svr)
	{
		return;
	}
	UInt16 port = 0;
	Bool valid = true;
	Text::StringBuilderUTF8 sb;
	me->txtPort->GetText(&sb);
	Text::StrToUInt16S(sb.ToString(), &port, 0);
	Net::SSLEngine *ssl = me->ssl;
	ssl->ServerSetRequireClientCert(Net::SSLEngine::ClientCertType::Optional);
	sb.ClearStr();
	me->txtClientCA->GetText(&sb);
	if (sb.leng != 0)
	{
		ssl->ServerSetClientCA(sb.ToCString());
	}

	if (me->sslCert == 0 || me->sslKey == 0)
	{
		SDEL_CLASS(me->sslKey);
		SDEL_CLASS(me->sslCert);
		Crypto::Cert::X509Key *key = ssl->GenerateRSAKey();
		me->sslKey = key;
		Crypto::Cert::CertNames names;
		Crypto::Cert::CertExtensions ext;
		Data::ArrayList<Text::String*> sanList;
		MemClear(&names, sizeof(names));
		MemClear(&ext, sizeof(ext));
		names.countryName = Text::String::New(UTF8STRC("HK"));
		names.stateOrProvinceName = Text::String::New(UTF8STRC("Hong Kong"));
		names.localityName = Text::String::New(UTF8STRC("Hong Kong"));
		names.organizationName = Text::String::New(UTF8STRC("Simon Software Working Room"));
		names.organizationUnitName = Text::String::New(UTF8STRC("SSWR"));
		names.commonName = Text::String::New(UTF8STRC("localhost"));
		names.emailAddress = Text::String::New(UTF8STRC("sswroom@yahoo.com"));

		sanList.Add(Text::String::New(CSTR("localhost")));
		sanList.Add(Text::String::New(CSTR("127.0.0.1")));
		ext.subjectAltName = &sanList;
		ext.useSubjKeyId = true;
		key->GetKeyId(ext.subjKeyId);
		ext.useAuthKeyId = true;
		key->GetKeyId(ext.authKeyId);
		me->sslCert = Crypto::Cert::CertUtil::SelfSignedCertCreate(ssl, &names, key, 30, &ext);
		Crypto::Cert::CertNames::FreeNames(&names);
		LIST_FREE_STRING(&sanList);

		Text::StringBuilderUTF8 sb;
		me->sslCert->ToShortString(&sb);
		sb.AppendC(UTF8STRC(", "));
		me->sslKey->ToShortString(&sb);
		me->lblSSLCert->SetText(sb.ToCString());

		if (!ssl->ServerSetCertsASN1(me->sslCert, me->sslKey, 0))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in initializing Cert/Key"), CSTR("HTTP Client Cert Test"), me);
			return;
		}
	}
	else
	{
		Crypto::Cert::X509Cert *issuerCert = Crypto::Cert::CertUtil::FindIssuer(me->sslCert);
		if (!ssl->ServerSetCertsASN1(me->sslCert, me->sslKey, issuerCert))
		{
			SDEL_CLASS(issuerCert);
			UI::MessageDialog::ShowDialog(CSTR("Error in initializing Cert/Key"), CSTR("HTTP Client Cert Test"), me);
			return;
		}
		SDEL_CLASS(issuerCert);
	}
	if (port > 0 && port < 65535)
	{
		UTF8Char sbuff[128];
		UTF8Char *sptr;
		Data::DateTime dt;
		IO::BuildTime::GetBuildTime(&dt);
		dt.ToUTCTime();
		sptr = dt.ToString(Text::StrConcatC(sbuff, UTF8STRC("AVIRead/")), "yyyyMMddHHmmss");
		NEW_CLASS(me->svr, Net::WebServer::WebListener(me->core->GetSocketFactory(), ssl, me, port, 120, Sync::ThreadUtil::GetThreadCnt(), CSTRP(sbuff, sptr), false, Net::WebServer::KeepAlive::Default, false));
		if (me->svr->IsError())
		{
			valid = false;
			SDEL_CLASS(me->svr);
			UI::MessageDialog::ShowDialog(CSTR("Error in listening to port"), CSTR("HTTP Client Cert Test"), me);
		}
		else
		{
			if (!me->svr->Start())
			{
				valid = false;
				UI::MessageDialog::ShowDialog(CSTR("Error in starting HTTP Server"), CSTR("HTTP Client Cert Test"), me);
			}
		}
	}

	if (valid)
	{
		me->txtPort->SetReadOnly(true);
		me->btnSSLCert->SetEnabled(false);
		me->txtClientCA->SetReadOnly(true);
	}
	else
	{
		SDEL_CLASS(me->svr);
		SDEL_CLASS(me->log);
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientCertTestForm::OnStopClick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPClientCertTestForm *me = (SSWR::AVIRead::AVIRHTTPClientCertTestForm*)userObj;
	if (me->svr == 0)
	{
		return;
	}
	SDEL_CLASS(me->svr);
	SDEL_CLASS(me->log);
	me->txtPort->SetReadOnly(false);
	me->btnSSLCert->SetEnabled(true);
	me->txtClientCA->SetReadOnly(false);
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientCertTestForm::OnSSLCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPClientCertTestForm *me = (SSWR::AVIRead::AVIRHTTPClientCertTestForm*)userObj;
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(0, me->ui, me->core, me->ssl, me->sslCert, me->sslKey);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		SDEL_CLASS(me->sslCert);
		SDEL_CLASS(me->sslKey);
		me->sslCert = frm.GetCert();
		me->sslKey = frm.GetKey();
		Text::StringBuilderUTF8 sb;
		me->sslCert->ToShortString(&sb);
		sb.AppendC(UTF8STRC(", "));
		me->sslKey->ToShortString(&sb);
		me->lblSSLCert->SetText(sb.ToCString());
	}
}

SSWR::AVIRead::AVIRHTTPClientCertTestForm::AVIRHTTPClientCertTestForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("HTTP Client Cert Test"));
	this->SetFont(0, 0, 8.25, false);
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->sslCert = 0;
	this->sslKey = 0;
	this->svr = 0;
	this->log = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->grpParam, UI::GUIGroupBox(ui, this, CSTR("Parameters")));
	this->grpParam->SetRect(0, 0, 620, 96, false);
	this->grpParam->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->grpParam, CSTR("Port")));
	this->lblPort->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->grpParam, CSTR("12345")));
	this->txtPort->SetRect(108, 8, 50, 23, false);
	NEW_CLASS(this->lblSSL, UI::GUILabel(ui, this->grpParam, CSTR("SSL")));
	this->lblSSL->SetRect(8, 32, 100, 23, false);
	NEW_CLASS(this->btnSSLCert, UI::GUIButton(ui, this->grpParam, CSTR("Cert/Key")));
	this->btnSSLCert->SetRect(108, 32, 75,23, false);
	this->btnSSLCert->HandleButtonClick(OnSSLCertClicked, this);
	NEW_CLASS(this->lblSSLCert, UI::GUILabel(ui, this->grpParam, CSTR("")));
	this->lblSSLCert->SetRect(188, 32, 200, 23, false);
	NEW_CLASS(this->lblClientCA, UI::GUILabel(ui, this->grpParam, CSTR("Client CA")));
	this->lblClientCA->SetRect(8, 56, 100, 23, false);
	NEW_CLASS(this->txtClientCA, UI::GUITextBox(ui, this->grpParam, CSTR("")));
	this->txtClientCA->SetRect(108, 56, 50, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this, CSTR("Start")));
	this->btnStart->SetRect(108, 100, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
	NEW_CLASS(this->btnStop, UI::GUIButton(ui, this, CSTR("Stop")));
	this->btnStop->SetRect(208, 100, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClick, this);
}

SSWR::AVIRead::AVIRHTTPClientCertTestForm::~AVIRHTTPClientCertTestForm()
{
	SDEL_CLASS(this->svr);
	SDEL_CLASS(this->log);
	SDEL_CLASS(this->ssl);
	SDEL_CLASS(this->sslCert);
	SDEL_CLASS(this->sslKey);
}

void SSWR::AVIRead::AVIRHTTPClientCertTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRHTTPClientCertTestForm::WebRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Text::StringBuilderUTF8 sb;
	Crypto::Cert::X509Cert *cert = req->GetClientCert();
	if (cert)
	{
		cert->ToString(&sb);
	}
	else
	{
		sb.AppendC(UTF8STRC("Client Certificate not found\r\n"));
	}
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/plain"));
	resp->AddContentLength(sb.leng);
	resp->Write(sb.v, sb.leng);
}

void SSWR::AVIRead::AVIRHTTPClientCertTestForm::Release()
{

}
