#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRSAMLTestForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"
#include "Text/XMLReader.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnFormFiles(void *userObj, Text::String **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRSAMLTestForm *me = (SSWR::AVIRead::AVIRSAMLTestForm*)userObj;
	Parser::ParserList *parsers = me->core->GetParserList();

	UOSInt i = 0;
	IO::ParsedObject *pobj;
	while (i < nFiles)
	{
		{
			IO::StmData::FileData fd(files[i], false);
			pobj = parsers->ParseFileType(&fd, IO::ParserType::ASN1Data);
		}
		if (pobj)
		{
			Net::ASN1Data *asn1 = (Net::ASN1Data*)pobj;
			Crypto::Cert::X509Key *key;
			if (asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
			{
				Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)asn1;
				switch (x509->GetFileType())
				{
				case Crypto::Cert::X509File::FileType::Cert:
					me->txtSignCert->SetText(files[i]->ToCString());
					break;
				case Crypto::Cert::X509File::FileType::CertRequest:
					break;
				case Crypto::Cert::X509File::FileType::Key:
					key = (Crypto::Cert::X509Key*)x509;
					if (key->IsPrivateKey())
					{
						me->txtSignKey->SetText(files[i]->ToCString());
					}
					break;
				case Crypto::Cert::X509File::FileType::PrivateKey:
					me->txtSignKey->SetText(files[i]->ToCString());
					break;
				case Crypto::Cert::X509File::FileType::FileList:
					me->txtSignCert->SetText(files[i]->ToCString());
					break;
				case Crypto::Cert::X509File::FileType::PublicKey:
				case Crypto::Cert::X509File::FileType::PKCS12:
				case Crypto::Cert::X509File::FileType::PKCS7:
				case Crypto::Cert::X509File::FileType::CRL:
					break;
				}
			}
			DEL_CLASS(pobj);
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnLogSel(void *userObj)
{
	Text::String *s;
	SSWR::AVIRead::AVIRSAMLTestForm *me = (SSWR::AVIRead::AVIRSAMLTestForm*)userObj;
	s = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(s->ToCString());
	s->Release();
}

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnSSLCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSAMLTestForm *me = (SSWR::AVIRead::AVIRSAMLTestForm*)userObj;
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
		sb.ClearStr();
		if (me->sslCert->GetSubjectCN(&sb))
		{
			me->txtHost->SetText(sb.ToCString());
		}		
	}
}

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSAMLTestForm *me = (SSWR::AVIRead::AVIRSAMLTestForm*)userObj;
	if (me->svr)
	{
		SDEL_CLASS(me->svr);
		if (me->samlHdlr)
		{
			me->samlHdlr->Release();
			me->samlHdlr = 0;
		}
		me->txtPort->SetReadOnly(false);
		me->btnSSLCert->SetEnabled(true);
		me->txtHost->SetReadOnly(false);
		me->txtSignCert->SetReadOnly(false);
		me->txtSignKey->SetReadOnly(false);
		me->txtMetadataPath->SetReadOnly(false);
		me->txtSSOPath->SetReadOnly(false);
		me->txtLogoutPath->SetReadOnly(false);
		me->btnStart->SetText(CSTR("Start"));
		return;
	}
	UInt16 port = 0;
	Bool valid = true;
	Text::StringBuilderUTF8 sb;
	me->txtPort->GetText(&sb);
	Text::StrToUInt16S(sb.ToString(), &port, 0);
	Net::SSLEngine *ssl = 0;

	if (me->sslCert == 0 || me->sslKey == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select SSL Cert/Key First"), CSTR("SAML Test"), me);
		return;
	}
	ssl = me->ssl;
	Crypto::Cert::X509Cert *issuerCert = Crypto::Cert::CertUtil::FindIssuer(me->sslCert);
	if (!ssl->SetServerCertsASN1(me->sslCert, me->sslKey, issuerCert))
	{
		SDEL_CLASS(issuerCert);
		UI::MessageDialog::ShowDialog(CSTR("Error in initializing Cert/Key"), CSTR("SAML Test"), me);
		return;
	}
	SDEL_CLASS(issuerCert);
	
	if (port > 0 && port < 65535)
	{
		Net::WebServer::SAMLConfig cfg;
		sb.ClearStr();
		me->txtHost->GetText(&sb);
		if (sb.GetLength() == 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter host"), CSTR("SAML Test"), me);
			return;
		}
		cfg.serverHost = sb.ToCString();

		Text::StringBuilderUTF8 sbSignCert;
		me->txtSignCert->GetText(&sbSignCert);
		if (sbSignCert.GetLength() == 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter Signature Cert"), CSTR("SAML Test"), me);
			return;
		}
		else if (IO::Path::GetPathType(sbSignCert.ToCString()) != IO::Path::PathType::File)
		{
			UI::MessageDialog::ShowDialog(CSTR("Signature Cert file path not valid"), CSTR("SAML Test"), me);
			return;
		}
		cfg.signCertPath = sbSignCert.ToCString();

		Text::StringBuilderUTF8 sbSignKey;
		me->txtSignKey->GetText(&sbSignKey);
		if (sbSignKey.GetLength() == 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter Signature Key"), CSTR("SAML Test"), me);
			return;
		}
		else if (IO::Path::GetPathType(sbSignKey.ToCString()) != IO::Path::PathType::File)
		{
			UI::MessageDialog::ShowDialog(CSTR("Signature Key file path not valid"), CSTR("SAML Test"), me);
			return;
		}
		cfg.signKeyPath = sbSignKey.ToCString();

		Text::StringBuilderUTF8 sbLogoutPath;
		me->txtLogoutPath->GetText(&sbLogoutPath);
		if (sbLogoutPath.GetLength() == 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter Logout Path"), CSTR("SAML Test"), me);
			return;
		}
		else if (sbLogoutPath.v[0] != '/')
		{
			UI::MessageDialog::ShowDialog(CSTR("Logout Path is not valid"), CSTR("SAML Test"), me);
			return;
		}
		cfg.logoutPath = sbLogoutPath.ToCString();

		Text::StringBuilderUTF8 sbMetadataPath;
		me->txtMetadataPath->GetText(&sbMetadataPath);
		if (sbMetadataPath.GetLength() == 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter Metadata Path"), CSTR("SAML Test"), me);
			return;
		}
		else if (sbMetadataPath.v[0] != '/')
		{
			UI::MessageDialog::ShowDialog(CSTR("Metadata Path is not valid"), CSTR("SAML Test"), me);
			return;
		}
		cfg.metadataPath = sbMetadataPath.ToCString();

		Text::StringBuilderUTF8 sbSSOPath;
		me->txtSSOPath->GetText(&sbSSOPath);
		if (sbSSOPath.GetLength() == 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter SSO Path"), CSTR("SAML Test"), me);
			return;
		}
		else if (sbSSOPath.v[0] != '/')
		{
			UI::MessageDialog::ShowDialog(CSTR("SSO Path is not valid"), CSTR("SAML Test"), me);
			return;
		}
		cfg.ssoPath = sbSSOPath.ToCString();

		NEW_CLASS(me->samlHdlr, Net::WebServer::SAMLHandler(&cfg, ssl, 0));
		if (me->samlHdlr->GetInitError() != Net::WebServer::SAMLError::None)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Error in initializing SAML: "));
			sb.Append(Net::WebServer::SAMLErrorGetName(me->samlHdlr->GetInitError()));
			UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("SAML Test"), me);
			me->samlHdlr->Release();
			return;
		}
		me->samlHdlr->HandleRAWSAMLResponse(OnSAMLResponse, me);
		me->samlHdlr->HandleLoginRequest(OnLoginRequest, me);
		NEW_CLASS(me->svr, Net::WebServer::WebListener(me->core->GetSocketFactory(), ssl, me->samlHdlr, port, 120, Sync::Thread::GetThreadCnt(), CSTR("SAMLTest/1.0"), false, false, false));
		if (me->svr->IsError())
		{
			SDEL_CLASS(me->svr);
			UI::MessageDialog::ShowDialog(CSTR("Error in listening to port"), CSTR("SAML Test"), me);
			valid = false;
		}
		else if (!me->svr->Start())
		{
			SDEL_CLASS(me->svr);
			UI::MessageDialog::ShowDialog(CSTR("Error in starting HTTP Server"), CSTR("SAML Test"), me);
			valid = false;
		}
		else
		{
			me->svr->SetAccessLog(&me->log, IO::ILogHandler::LogLevel::Raw);
		}
	}

	if (valid)
	{
		me->txtPort->SetReadOnly(true);
		me->btnSSLCert->SetEnabled(false);
		me->txtHost->SetReadOnly(true);
		me->txtSignCert->SetReadOnly(true);
		me->txtSignKey->SetReadOnly(true);
		me->txtMetadataPath->SetReadOnly(true);
		me->txtSSOPath->SetReadOnly(true);
		me->txtLogoutPath->SetReadOnly(true);
		me->btnStart->SetText(CSTR("Stop"));

		sb.ClearStr();
		me->samlHdlr->GetLogoutURL(&sb);
		me->txtLogoutURL->SetText(sb.ToCString());
		sb.ClearStr();
		me->samlHdlr->GetMetadataURL(&sb);
		me->txtMetadataURL->SetText(sb.ToCString());
		sb.ClearStr();
		me->samlHdlr->GetSSOURL(&sb);
		me->txtSSOURL->SetText(sb.ToCString());
	}
	else
	{
		SDEL_CLASS(me->svr);
		if (me->samlHdlr)
		{
			me->samlHdlr->Release();
			me->samlHdlr = 0;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRSAMLTestForm *me = (SSWR::AVIRead::AVIRSAMLTestForm*)userObj;
	Sync::MutexUsage mutUsage(&me->respMut);
	if (me->respNew)
	{
		me->txtSAMLResp->SetText(me->respNew->ToCString());
		Text::StringBuilderUTF8 sb;
		IO::MemoryStream mstm(me->respNew->v, me->respNew->leng, UTF8STRC("SSWR.AVIRead.AVIRSAMLTestForm.OnTimerTick.mstm"));
		Text::XMLReader::XMLWellFormat(me->core->GetEncFactory(), &mstm, 0, &sb);
		me->txtSAMLRespWF->SetText(sb.ToCString());
		me->respNew->Release();
		me->respNew = 0;
	}
}

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnSAMLResponse(void *userObj, Text::CString msg)
{
	SSWR::AVIRead::AVIRSAMLTestForm *me = (SSWR::AVIRead::AVIRSAMLTestForm*)userObj;
	Sync::MutexUsage mutUsage(&me->respMut);
	SDEL_STRING(me->respNew);
	me->respNew = Text::String::New(msg);
}

Bool __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnLoginRequest(void *userObj, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const Net::WebServer::SAMLMessage *msg)
{
	SSWR::AVIRead::AVIRSAMLTestForm *me = (SSWR::AVIRead::AVIRSAMLTestForm*)userObj;
	IO::MemoryStream mstm((UInt8*)msg->rawMessage.v, msg->rawMessage.leng, UTF8STRC("SSWR.AVIRead.AVIRSAMLTestForm.OnLoginRequest.mstm"));
	Text::StringBuilderUTF8 sb;
	Text::XMLReader::XMLWellFormat(me->core->GetEncFactory(), &mstm, 0, &sb);
	Text::String *msgContent = Text::XML::ToNewHTMLText(sb.ToString());
	sb.ClearStr();
	sb.AppendC(UTF8STRC("<html><head><title>SAML Message</title></head><body>"));
	sb.AppendC(UTF8STRC("<h1>RAW Response</h1>"));
	sb.Append(msgContent);
	sb.AppendC(UTF8STRC("</body></html>"));
	msgContent->Release();
	resp->AddDefHeaders(req);
	resp->AddCacheControl(0);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}

SSWR::AVIRead::AVIRSAMLTestForm::AVIRSAMLTestForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("SAML Test"));
	this->SetFont(0, 0, 8.25, false);
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->sslCert = 0;
	this->sslKey = 0;
	this->svr = 0;
	this->logger = 0;
	this->respNew = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->tpControl, CSTR("Port")));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->tpControl, CSTR("443")));
	this->txtPort->SetRect(104, 4, 50, 23, false);
	NEW_CLASS(this->lblSSL, UI::GUILabel(ui, this->tpControl, CSTR("SSL")));
	this->lblSSL->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->btnSSLCert, UI::GUIButton(ui, this->tpControl, CSTR("Cert/Key")));
	this->btnSSLCert->SetRect(104, 28, 75,23, false);
	this->btnSSLCert->HandleButtonClick(OnSSLCertClicked, this);
	NEW_CLASS(this->lblSSLCert, UI::GUILabel(ui, this->tpControl, CSTR("")));
	this->lblSSLCert->SetRect(184, 28, 200, 23, false);
	NEW_CLASS(this->lblHost, UI::GUILabel(ui, this->tpControl, CSTR("Host")));
	this->lblHost->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtHost, UI::GUITextBox(ui, this->tpControl, CSTR("localhost")));
	this->txtHost->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->lblSignCert, UI::GUILabel(ui, this->tpControl, CSTR("Signature Cert")));
	this->lblSignCert->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtSignCert, UI::GUITextBox(ui, this->tpControl, CSTR("")));
	this->txtSignCert->SetRect(104, 76, 500, 23, false);
	NEW_CLASS(this->lblSignKey, UI::GUILabel(ui, this->tpControl, CSTR("Signature Key")));
	this->lblSignKey->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtSignKey, UI::GUITextBox(ui, this->tpControl, CSTR("")));
	this->txtSignKey->SetRect(104, 100, 500, 23, false);
	NEW_CLASS(this->lblSSOPath, UI::GUILabel(ui, this->tpControl, CSTR("SSO Path")));
	this->lblSSOPath->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtSSOPath, UI::GUITextBox(ui, this->tpControl, CSTR("/sso")));
	this->txtSSOPath->SetRect(104, 124, 500, 23, false);
	NEW_CLASS(this->lblMetadataPath, UI::GUILabel(ui, this->tpControl, CSTR("Metadata Path")));
	this->lblMetadataPath->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtMetadataPath, UI::GUITextBox(ui, this->tpControl, CSTR("/metadata")));
	this->txtMetadataPath->SetRect(104, 148, 500, 23, false);
	NEW_CLASS(this->lblLogoutPath, UI::GUILabel(ui, this->tpControl, CSTR("Logout Path")));
	this->lblLogoutPath->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtLogoutPath, UI::GUITextBox(ui, this->tpControl, CSTR("/logout")));
	this->txtLogoutPath->SetRect(104, 172, 500, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->tpControl, CSTR("Start")));
	this->btnStart->SetRect(104, 196, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->lblSSOURL, UI::GUILabel(ui, this->tpControl, CSTR("SSO URL")));
	this->lblSSOURL->SetRect(4, 220, 100, 23, false);
	NEW_CLASS(this->txtSSOURL, UI::GUITextBox(ui, this->tpControl, CSTR("")));
	this->txtSSOURL->SetRect(104, 220, 500, 23, false);
	this->txtSSOURL->SetReadOnly(true);
	NEW_CLASS(this->lblMetadataURL, UI::GUILabel(ui, this->tpControl, CSTR("Metadata URL")));
	this->lblMetadataURL->SetRect(4, 244, 100, 23, false);
	NEW_CLASS(this->txtMetadataURL, UI::GUITextBox(ui, this->tpControl, CSTR("")));
	this->txtMetadataURL->SetRect(104, 244, 500, 23, false);
	this->txtMetadataURL->SetReadOnly(true);
	NEW_CLASS(this->lblLogoutURL, UI::GUILabel(ui, this->tpControl, CSTR("Logout URL")));
	this->lblLogoutURL->SetRect(4, 268, 100, 23, false);
	NEW_CLASS(this->txtLogoutURL, UI::GUITextBox(ui, this->tpControl, CSTR("")));
	this->txtLogoutURL->SetRect(104, 268, 500, 23, false);
	this->txtLogoutURL->SetReadOnly(true);

	this->tpSAMLResp = this->tcMain->AddTabPage(CSTR("SAML Resp"));
	NEW_CLASS(this->txtSAMLResp, UI::GUITextBox(ui, this->tpSAMLResp, CSTR(""), true));
	this->txtSAMLResp->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtSAMLResp->SetReadOnly(true);

	this->tpSAMLRespWF = this->tcMain->AddTabPage(CSTR("Resp WF"));
	NEW_CLASS(this->txtSAMLRespWF, UI::GUITextBox(ui, this->tpSAMLRespWF, CSTR(""), true));
	this->txtSAMLRespWF->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtSAMLRespWF->SetReadOnly(true);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, CSTR("")));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSel, this);

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 500, true));
	this->log.AddLogHandler(this->logger, IO::ILogHandler::LogLevel::Raw);

	this->HandleDropFiles(OnFormFiles, this);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRSAMLTestForm::~AVIRSAMLTestForm()
{
	SDEL_CLASS(this->svr);
	if (this->samlHdlr)
	{
		this->samlHdlr->Release();
		this->samlHdlr = 0;
	}
	this->log.RemoveLogHandler(this->logger);
	SDEL_CLASS(this->logger);
	SDEL_CLASS(this->ssl);
	SDEL_CLASS(this->sslCert);
	SDEL_CLASS(this->sslKey);
	SDEL_STRING(this->respNew);
}

void SSWR::AVIRead::AVIRSAMLTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}