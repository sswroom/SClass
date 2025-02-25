#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Data/ArrayListNN.h"
#include "IO/BuildTime.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHTTPClientCertTestForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRHTTPClientCertTestForm::OnStartClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPClientCertTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPClientCertTestForm>();
	NN<Net::SSLEngine> ssl;
	NN<Net::WebServer::WebListener> svr;
	if (me->svr.NotNull() || !me->ssl.SetTo(ssl))
	{
		return;
	}
	UInt16 port = 0;
	Bool valid = true;
	Text::StringBuilderUTF8 sb;
	me->txtPort->GetText(sb);
	Text::StrToUInt16S(sb.ToString(), port, 0);
	NN<Crypto::Cert::X509Cert> sslCert;
	NN<Crypto::Cert::X509File> sslKey;
	ssl->ServerSetRequireClientCert(Net::SSLEngine::ClientCertType::Optional);
	sb.ClearStr();
	me->txtClientCA->GetText(sb);
	if (sb.leng != 0)
	{
		ssl->ServerSetClientCA(sb.ToCString());
	}

	if (!me->sslCert.SetTo(sslCert) || !me->sslKey.SetTo(sslKey))
	{
		NN<Crypto::Cert::X509Key> key;
		if (!ssl->GenerateRSAKey(Net::SSLEngine::GetRSAKeyLength()).SetTo(key))
		{
			me->ui->ShowMsgOK(CSTR("Error in initializing Key"), CSTR("HTTP Client Cert Test"), me);
			return;
		}
		me->sslKey = key.Ptr();
		Crypto::Cert::CertNames names;
		Crypto::Cert::CertExtensions ext;
		Data::ArrayListStringNN sanList;
		MemClear(&names, sizeof(names));
		MemClear(&ext, sizeof(ext));
		names.countryName = Text::String::New(UTF8STRC("HK")).Ptr();
		names.stateOrProvinceName = Text::String::New(UTF8STRC("Hong Kong")).Ptr();
		names.localityName = Text::String::New(UTF8STRC("Hong Kong")).Ptr();
		names.organizationName = Text::String::New(UTF8STRC("Simon Software Working Room")).Ptr();
		names.organizationUnitName = Text::String::New(UTF8STRC("SSWR")).Ptr();
		names.commonName = Text::String::New(UTF8STRC("localhost")).Ptr();
		names.emailAddress = Text::String::New(UTF8STRC("sswroom@yahoo.com")).Ptr();

		sanList.Add(Text::String::New(CSTR("localhost")));
		sanList.Add(Text::String::New(CSTR("127.0.0.1")));
		ext.subjectAltName = &sanList;
		ext.useSubjKeyId = true;
		key->GetKeyId(BYTEARR(ext.subjKeyId));
		ext.useAuthKeyId = true;
		key->GetKeyId(BYTEARR(ext.authKeyId));
		me->sslCert = Crypto::Cert::CertUtil::SelfSignedCertCreate(ssl, names, key, 30, &ext);
		Crypto::Cert::CertNames::FreeNames(names);
		sanList.FreeAll();
		if (!me->sslCert.SetTo(sslCert) || !me->sslKey.SetTo(sslKey))
		{
			me->ui->ShowMsgOK(CSTR("Error in initializing Certificate"), CSTR("HTTP Client Cert Test"), me);
			return;
		}

		Text::StringBuilderUTF8 sb;
		sslCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		sslKey->ToShortString(sb);
		me->lblSSLCert->SetText(sb.ToCString());

		Data::ArrayListNN<Crypto::Cert::X509Cert> caCerts;
		if (!ssl->ServerSetCertsASN1(sslCert, key, caCerts))
		{
			me->ui->ShowMsgOK(CSTR("Error in initializing Cert/Key"), CSTR("HTTP Client Cert Test"), me);
			return;
		}
	}
	else
	{
		if (!ssl->ServerSetCertsASN1(sslCert, sslKey, me->caCerts))
		{
			me->ui->ShowMsgOK(CSTR("Error in initializing Cert/Key"), CSTR("HTTP Client Cert Test"), me);
			return;
		}
	}
	if (port > 0 && port < 65535)
	{
		UTF8Char sbuff[128];
		UnsafeArray<UTF8Char> sptr;
		Data::DateTime dt;
		IO::BuildTime::GetBuildTime(dt);
		dt.ToUTCTime();
		sptr = dt.ToString(Text::StrConcatC(sbuff, UTF8STRC("AVIRead/")), "yyyyMMddHHmmss");
		NEW_CLASSNN(svr, Net::WebServer::WebListener(me->core->GetTCPClientFactory(), ssl, me, port, 120, 2, Sync::ThreadUtil::GetThreadCnt(), CSTRP(sbuff, sptr), false, Net::WebServer::KeepAlive::Default, false));
		if (svr->IsError())
		{
			valid = false;
			svr.Delete();
			me->ui->ShowMsgOK(CSTR("Error in listening to port"), CSTR("HTTP Client Cert Test"), me);
		}
		else
		{
			me->svr = svr;
			if (!svr->Start())
			{
				valid = false;
				me->ui->ShowMsgOK(CSTR("Error in starting HTTP Server"), CSTR("HTTP Client Cert Test"), me);
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
		me->svr.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientCertTestForm::OnStopClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPClientCertTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPClientCertTestForm>();
	if (me->svr.IsNull())
	{
		return;
	}
	me->svr.Delete();
	me->txtPort->SetReadOnly(false);
	me->btnSSLCert->SetEnabled(true);
	me->txtClientCA->SetReadOnly(false);
}

void __stdcall SSWR::AVIRead::AVIRHTTPClientCertTestForm::OnSSLCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPClientCertTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPClientCertTestForm>();
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(0, me->ui, me->core, me->ssl, me->sslCert, me->sslKey, me->caCerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		NN<Crypto::Cert::X509Cert> nnCert;
		NN<Crypto::Cert::X509File> nnKey;
		me->sslCert.Delete();
		me->sslKey.Delete();
		me->ClearCACerts();
		me->sslCert = frm.GetCert();
		me->sslKey = frm.GetKey();
		frm.GetCACerts(me->caCerts);
		Text::StringBuilderUTF8 sb;
		if (me->sslCert.SetTo(nnCert)) nnCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		if (me->sslKey.SetTo(nnKey)) nnKey->ToShortString(sb);
		me->lblSSLCert->SetText(sb.ToCString());
	}
}

void SSWR::AVIRead::AVIRHTTPClientCertTestForm::ClearCACerts()
{
	UOSInt i = this->caCerts.GetCount();
	while (i-- > 0)
	{
		this->caCerts.GetItem(i).Delete();
	}
	this->caCerts.Clear();
}

SSWR::AVIRead::AVIRHTTPClientCertTestForm::AVIRHTTPClientCertTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("HTTP Client Cert Test"));
	this->SetFont(0, 0, 8.25, false);
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->sslCert = 0;
	this->sslKey = 0;
	this->svr = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->grpParam = ui->NewGroupBox(*this, CSTR("Parameters"));
	this->grpParam->SetRect(0, 0, 620, 96, false);
	this->grpParam->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPort = ui->NewLabel(this->grpParam, CSTR("Port"));
	this->lblPort->SetRect(8, 8, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->grpParam, CSTR("12345"));
	this->txtPort->SetRect(108, 8, 50, 23, false);
	this->lblSSL = ui->NewLabel(this->grpParam, CSTR("SSL"));
	this->lblSSL->SetRect(8, 32, 100, 23, false);
	this->btnSSLCert = ui->NewButton(this->grpParam, CSTR("Cert/Key"));
	this->btnSSLCert->SetRect(108, 32, 75,23, false);
	this->btnSSLCert->HandleButtonClick(OnSSLCertClicked, this);
	this->lblSSLCert = ui->NewLabel(this->grpParam, CSTR(""));
	this->lblSSLCert->SetRect(188, 32, 200, 23, false);
	this->lblClientCA = ui->NewLabel(this->grpParam, CSTR("Client CA"));
	this->lblClientCA->SetRect(8, 56, 100, 23, false);
	this->txtClientCA = ui->NewTextBox(this->grpParam, CSTR(""));
	this->txtClientCA->SetRect(108, 56, 50, 23, false);
	this->btnStart = ui->NewButton(*this, CSTR("Start"));
	this->btnStart->SetRect(108, 100, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
	this->btnStop = ui->NewButton(*this, CSTR("Stop"));
	this->btnStop->SetRect(208, 100, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClick, this);
}

SSWR::AVIRead::AVIRHTTPClientCertTestForm::~AVIRHTTPClientCertTestForm()
{
	this->svr.Delete();
	this->ssl.Delete();
	this->sslCert.Delete();
	this->sslKey.Delete();
	this->ClearCACerts();
}

void SSWR::AVIRead::AVIRHTTPClientCertTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRHTTPClientCertTestForm::DoWebRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::StringBuilderUTF8 sb;
	NN<Crypto::Cert::X509Cert> cert;
	if (req->GetClientCert().SetTo(cert))
	{
		cert->ToString(sb);
	}
	else
	{
		sb.AppendC(UTF8STRC("Client Certificate not found\r\n"));
	}
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("text/plain"));
	resp->AddContentLength(sb.leng);
	resp->Write(sb.ToByteArray());
}

void SSWR::AVIRead::AVIRHTTPClientCertTestForm::Release()
{

}
