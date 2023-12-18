#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "IO/MemoryReadingStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Net/SAMLUtil.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRSAMLTestForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"
#include "Text/XMLReader.h"

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnFormFiles(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRSAMLTestForm *me = (SSWR::AVIRead::AVIRSAMLTestForm*)userObj;
	NotNullPtr<Parser::ParserList> parsers = me->core->GetParserList();

	UOSInt i = 0;
	IO::ParsedObject *pobj;
	while (i < nFiles)
	{
		{
			IO::StmData::FileData fd(files[i], false);
			pobj = parsers->ParseFileType(fd, IO::ParserType::ASN1Data);
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
	SSWR::AVIRead::AVIRSAMLTestForm *me = (SSWR::AVIRead::AVIRSAMLTestForm*)userObj;
	Optional<Text::String> s = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(Text::String::OrEmpty(s)->ToCString());
	OPTSTR_DEL(s);
}

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnSSLCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSAMLTestForm *me = (SSWR::AVIRead::AVIRSAMLTestForm*)userObj;
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
		sb.ClearStr();
		if (me->sslCert->GetSubjectCN(sb))
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
		SDEL_CLASS(me->samlHdlr);
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
	me->txtPort->GetText(sb);
	Text::StrToUInt16S(sb.ToString(), port, 0);
	Optional<Net::SSLEngine> ssl = 0;
	NotNullPtr<Crypto::Cert::X509Cert> sslCert;
	NotNullPtr<Crypto::Cert::X509File> sslKey;

	if (!sslCert.Set(me->sslCert) || !sslKey.Set(me->sslKey))
	{
		me->ui->ShowMsgOK(CSTR("Please select SSL Cert/Key First"), CSTR("SAML Test"), me);
		return;
	}
	ssl = me->ssl;
	NotNullPtr<Net::SSLEngine> nnssl;
	if (!ssl.SetTo(nnssl) || !nnssl->ServerSetCertsASN1(sslCert, sslKey, me->caCerts))
	{
		me->ui->ShowMsgOK(CSTR("Error in initializing Cert/Key"), CSTR("SAML Test"), me);
		return;
	}
	
	if (port > 0 && port < 65535)
	{
		Net::WebServer::SAMLConfig cfg;
		sb.ClearStr();
		me->txtHost->GetText(sb);
		if (sb.GetLength() == 0)
		{
			me->ui->ShowMsgOK(CSTR("Please enter host"), CSTR("SAML Test"), me);
			return;
		}
		cfg.serverHost = sb.ToCString();

		Text::StringBuilderUTF8 sbSignCert;
		me->txtSignCert->GetText(sbSignCert);
		if (sbSignCert.GetLength() == 0)
		{
			me->ui->ShowMsgOK(CSTR("Please enter Signature Cert"), CSTR("SAML Test"), me);
			return;
		}
		else if (IO::Path::GetPathType(sbSignCert.ToCString()) != IO::Path::PathType::File)
		{
			me->ui->ShowMsgOK(CSTR("Signature Cert file path not valid"), CSTR("SAML Test"), me);
			return;
		}
		cfg.signCertPath = sbSignCert.ToCString();

		Text::StringBuilderUTF8 sbSignKey;
		me->txtSignKey->GetText(sbSignKey);
		if (sbSignKey.GetLength() == 0)
		{
			me->ui->ShowMsgOK(CSTR("Please enter Signature Key"), CSTR("SAML Test"), me);
			return;
		}
		else if (IO::Path::GetPathType(sbSignKey.ToCString()) != IO::Path::PathType::File)
		{
			me->ui->ShowMsgOK(CSTR("Signature Key file path not valid"), CSTR("SAML Test"), me);
			return;
		}
		cfg.signKeyPath = sbSignKey.ToCString();

		Text::StringBuilderUTF8 sbLogoutPath;
		me->txtLogoutPath->GetText(sbLogoutPath);
		if (sbLogoutPath.GetLength() == 0)
		{
			me->ui->ShowMsgOK(CSTR("Please enter Logout Path"), CSTR("SAML Test"), me);
			return;
		}
		else if (sbLogoutPath.v[0] != '/')
		{
			me->ui->ShowMsgOK(CSTR("Logout Path is not valid"), CSTR("SAML Test"), me);
			return;
		}
		cfg.logoutPath = sbLogoutPath.ToCString();

		Text::StringBuilderUTF8 sbMetadataPath;
		me->txtMetadataPath->GetText(sbMetadataPath);
		if (sbMetadataPath.GetLength() == 0)
		{
			me->ui->ShowMsgOK(CSTR("Please enter Metadata Path"), CSTR("SAML Test"), me);
			return;
		}
		else if (sbMetadataPath.v[0] != '/')
		{
			me->ui->ShowMsgOK(CSTR("Metadata Path is not valid"), CSTR("SAML Test"), me);
			return;
		}
		cfg.metadataPath = sbMetadataPath.ToCString();

		Text::StringBuilderUTF8 sbSSOPath;
		me->txtSSOPath->GetText(sbSSOPath);
		if (sbSSOPath.GetLength() == 0)
		{
			me->ui->ShowMsgOK(CSTR("Please enter SSO Path"), CSTR("SAML Test"), me);
			return;
		}
		else if (sbSSOPath.v[0] != '/')
		{
			me->ui->ShowMsgOK(CSTR("SSO Path is not valid"), CSTR("SAML Test"), me);
			return;
		}
		cfg.ssoPath = sbSSOPath.ToCString();

		NotNullPtr<Net::WebServer::SAMLHandler> samlHdlr;
		NEW_CLASSNN(samlHdlr, Net::WebServer::SAMLHandler(&cfg, ssl, 0));
		if (samlHdlr->GetInitError() != Net::WebServer::SAMLError::None)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Error in initializing SAML: "));
			sb.Append(Net::WebServer::SAMLErrorGetName(samlHdlr->GetInitError()));
			me->ui->ShowMsgOK(sb.ToCString(), CSTR("SAML Test"), me);
			samlHdlr.Delete();
			return;
		}
		samlHdlr->HandleRAWSAMLResponse(OnSAMLResponse, me);
		samlHdlr->HandleLoginRequest(OnLoginRequest, me);
		NEW_CLASS(me->svr, Net::WebServer::WebListener(me->core->GetSocketFactory(), ssl, samlHdlr, port, 120, 2, Sync::ThreadUtil::GetThreadCnt(), CSTR("SAMLTest/1.0"), false, Net::WebServer::KeepAlive::Default, false));
		if (me->svr->IsError())
		{
			SDEL_CLASS(me->svr);
			samlHdlr.Delete();
			me->ui->ShowMsgOK(CSTR("Error in listening to port"), CSTR("SAML Test"), me);
			valid = false;
		}
		else if (!me->svr->Start())
		{
			SDEL_CLASS(me->svr);
			samlHdlr.Delete();
			me->ui->ShowMsgOK(CSTR("Error in starting HTTP Server"), CSTR("SAML Test"), me);
			valid = false;
		}
		else
		{
			me->samlHdlr = samlHdlr.Ptr();
			me->svr->SetAccessLog(&me->log, IO::LogHandler::LogLevel::Raw);
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
		me->samlHdlr->GetLogoutURL(sb);
		me->txtLogoutURL->SetText(sb.ToCString());
		sb.ClearStr();
		me->samlHdlr->GetMetadataURL(sb);
		me->txtMetadataURL->SetText(sb.ToCString());
		sb.ClearStr();
		me->samlHdlr->GetSSOURL(sb);
		me->txtSSOURL->SetText(sb.ToCString());
	}
	else
	{
		SDEL_CLASS(me->svr);
		SDEL_CLASS(me->samlHdlr);
	}
}

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRSAMLTestForm *me = (SSWR::AVIRead::AVIRSAMLTestForm*)userObj;
	Sync::MutexUsage mutUsage(me->respMut);
	if (me->respNew)
	{
		me->txtSAMLResp->SetText(me->respNew->ToCString());
		Text::StringBuilderUTF8 sb;
		{
			IO::MemoryReadingStream mstm(me->respNew->v, me->respNew->leng);
			Text::XMLReader::XMLWellFormat(me->core->GetEncFactory(), mstm, 0, sb);
		}
		me->txtSAMLRespWF->SetText(sb.ToCString());

		NotNullPtr<Crypto::Cert::X509Key> key;
		if (key.Set(me->samlHdlr->GetKey()->CreateKey()))
		{
			sb.ClearStr();
			NotNullPtr<Net::SSLEngine> ssl;
			if (me->ssl.SetTo(ssl))
			{
				if (Net::SAMLUtil::DecryptResponse(ssl, me->core->GetEncFactory(), key, me->respNew->ToCString(), sb))
				{
					IO::MemoryReadingStream mstm(sb.v, sb.GetLength());
					Text::StringBuilderUTF8 sb2;
					Text::XMLReader::XMLWellFormat(me->core->GetEncFactory(), mstm, 0, sb2);
					me->txtSAMLDecrypt->SetText(sb2.ToCString());
				}
				else
				{
					me->txtSAMLDecrypt->SetText(sb.ToCString());
				}
			}
			else
			{
				me->txtSAMLDecrypt->SetText(CSTR("Error in SSL Engine"));
			}
			key.Delete();
		}

		me->respNew->Release();
		me->respNew = 0;
	}
}

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnSAMLResponse(void *userObj, Text::CString msg)
{
	SSWR::AVIRead::AVIRSAMLTestForm *me = (SSWR::AVIRead::AVIRSAMLTestForm*)userObj;
	Sync::MutexUsage mutUsage(me->respMut);
	SDEL_STRING(me->respNew);
	me->respNew = Text::String::New(msg).Ptr();
}

Bool __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnLoginRequest(void *userObj, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, const Net::WebServer::SAMLMessage *msg)
{
	SSWR::AVIRead::AVIRSAMLTestForm *me = (SSWR::AVIRead::AVIRSAMLTestForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Text::String *decMsg = 0;
	NotNullPtr<Crypto::Cert::X509Key> key;
	if (key.Set(me->samlHdlr->GetKey()->CreateKey()))
	{
		NotNullPtr<Net::SSLEngine> ssl;
		if (me->ssl.SetTo(ssl) && Net::SAMLUtil::DecryptResponse(ssl, me->core->GetEncFactory(), key, msg->rawMessage, sb))
		{
			IO::MemoryReadingStream mstm(sb.v, sb.GetLength());
			Text::StringBuilderUTF8 sb2;
			Text::XMLReader::XMLWellFormat(me->core->GetEncFactory(), mstm, 0, sb2);
			decMsg = Text::XML::ToNewHTMLTextXMLColor(sb2.ToString()).Ptr();
		}
		key.Delete();
	}
	{
		IO::MemoryReadingStream mstm(msg->rawMessage.v, msg->rawMessage.leng);
		sb.ClearStr();
		Text::XMLReader::XMLWellFormat(me->core->GetEncFactory(), mstm, 0, sb);
	}
	NotNullPtr<Text::String> msgContent = Text::XML::ToNewHTMLTextXMLColor(sb.ToString());
	sb.ClearStr();
	sb.AppendC(UTF8STRC("<html><head><title>SAML Message</title></head><body>"));
	if (decMsg)
	{
		sb.AppendC(UTF8STRC("<h1>Decrypted Content</h1>"));
		sb.Append(decMsg);
		sb.AppendC(UTF8STRC("<hr/>"));
		decMsg->Release();
	}
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

void SSWR::AVIRead::AVIRSAMLTestForm::ClearCACerts()
{
	this->caCerts.DeleteAll();
}

SSWR::AVIRead::AVIRSAMLTestForm::AVIRSAMLTestForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("SAML Test"));
	this->SetFont(0, 0, 8.25, false);
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->sslCert = 0;
	this->sslKey = 0;
	this->svr = 0;
	this->respNew = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, *this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->tpControl, CSTR("Port")));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->tpControl, CSTR("443")));
	this->txtPort->SetRect(104, 4, 50, 23, false);
	NEW_CLASS(this->lblSSL, UI::GUILabel(ui, this->tpControl, CSTR("SSL")));
	this->lblSSL->SetRect(4, 28, 100, 23, false);
	this->btnSSLCert = ui->NewButton(this->tpControl, CSTR("Cert/Key"));
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
	this->btnStart = ui->NewButton(this->tpControl, CSTR("Start"));
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

	this->tpSAMLDecrypt = this->tcMain->AddTabPage(CSTR("SAML Decrypt"));
	NEW_CLASS(this->txtSAMLDecrypt, UI::GUITextBox(ui, this->tpSAMLDecrypt, CSTR(""), true));
	this->txtSAMLDecrypt->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtSAMLDecrypt->SetReadOnly(true);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, CSTR("")));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASSNN(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSel, this);

	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 500, true));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);

	this->HandleDropFiles(OnFormFiles, this);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRSAMLTestForm::~AVIRSAMLTestForm()
{
	SDEL_CLASS(this->svr);
	SDEL_CLASS(this->samlHdlr);
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
	this->ssl.Delete();
	SDEL_CLASS(this->sslCert);
	SDEL_CLASS(this->sslKey);
	this->ClearCACerts();
	SDEL_STRING(this->respNew);
}

void SSWR::AVIRead::AVIRSAMLTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
