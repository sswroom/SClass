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

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnFormFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRSAMLTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSAMLTestForm>();
	NN<Parser::ParserList> parsers = me->core->GetParserList();

	UOSInt i = 0;
	UOSInt nFiles = files.GetCount();
	NN<IO::ParsedObject> pobj;
	while (i < nFiles)
	{
		IO::StmData::FileData fd(files[i], false);
		if (parsers->ParseFileType(fd, IO::ParserType::ASN1Data).SetTo(pobj))
		{
			NN<Net::ASN1Data> asn1 = NN<Net::ASN1Data>::ConvertFrom(pobj);
			NN<Crypto::Cert::X509Key> key;
			if (asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
			{
				NN<Crypto::Cert::X509File> x509 = NN<Crypto::Cert::X509File>::ConvertFrom(asn1);
				switch (x509->GetFileType())
				{
				case Crypto::Cert::X509File::FileType::Cert:
					me->txtSignCert->SetText(files[i]->ToCString());
					break;
				case Crypto::Cert::X509File::FileType::CertRequest:
					break;
				case Crypto::Cert::X509File::FileType::Key:
					key = NN<Crypto::Cert::X509Key>::ConvertFrom(x509);
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
			pobj.Delete();
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnLogSel(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSAMLTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSAMLTestForm>();
	Optional<Text::String> s = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(Text::String::OrEmpty(s)->ToCString());
	OPTSTR_DEL(s);
}

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnSSLCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSAMLTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSAMLTestForm>();
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
		sb.ClearStr();
		if (me->sslCert.SetTo(nnCert) && nnCert->GetSubjectCN(sb))
		{
			me->txtHost->SetText(sb.ToCString());
		}		
	}
}

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSAMLTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSAMLTestForm>();
		NN<Net::WebServer::SAMLHandler> samlHdlr;
		NN<Net::WebServer::WebListener> svr;
	if (me->svr.NotNull())
	{
		me->svr.Delete();
		me->samlHdlr.Delete();
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
	NN<Crypto::Cert::X509Cert> sslCert;
	NN<Crypto::Cert::X509File> sslKey;

	if (!me->sslCert.SetTo(sslCert) || !me->sslKey.SetTo(sslKey))
	{
		me->ui->ShowMsgOK(CSTR("Please select SSL Cert/Key First"), CSTR("SAML Test"), me);
		return;
	}
	ssl = me->ssl;
	NN<Net::SSLEngine> nnssl;
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

		NEW_CLASSNN(samlHdlr, Net::WebServer::SAMLHandler(cfg, ssl, 0));
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
		NEW_CLASSNN(svr, Net::WebServer::WebListener(me->core->GetTCPClientFactory(), ssl, samlHdlr, port, 120, 2, Sync::ThreadUtil::GetThreadCnt(), CSTR("SAMLTest/1.0"), false, Net::WebServer::KeepAlive::Default, false));
		if (svr->IsError())
		{
			svr.Delete();
			samlHdlr.Delete();
			me->ui->ShowMsgOK(CSTR("Error in listening to port"), CSTR("SAML Test"), me);
			valid = false;
		}
		else if (!svr->Start())
		{
			svr.Delete();
			samlHdlr.Delete();
			me->ui->ShowMsgOK(CSTR("Error in starting HTTP Server"), CSTR("SAML Test"), me);
			valid = false;
		}
		else
		{
			me->samlHdlr = samlHdlr;
			me->svr = svr;
			svr->SetAccessLog(&me->log, IO::LogHandler::LogLevel::Raw);
		}
	}

	if (valid && me->samlHdlr.SetTo(samlHdlr))
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
		samlHdlr->GetLogoutURL(sb);
		me->txtLogoutURL->SetText(sb.ToCString());
		sb.ClearStr();
		samlHdlr->GetMetadataURL(sb);
		me->txtMetadataURL->SetText(sb.ToCString());
		sb.ClearStr();
		samlHdlr->GetSSOURL(sb);
		me->txtSSOURL->SetText(sb.ToCString());
	}
	else
	{
		me->svr.Delete();
		me->samlHdlr.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSAMLTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSAMLTestForm>();
	Sync::MutexUsage mutUsage(me->respMut);
	NN<Text::String> respNew;
	if (me->respNew.SetTo(respNew))
	{
		me->txtSAMLResp->SetText(respNew->ToCString());
		Text::StringBuilderUTF8 sb;
		{
			IO::MemoryReadingStream mstm(respNew->v, respNew->leng);
			Text::XMLReader::XMLWellFormat(me->core->GetEncFactory(), mstm, 0, sb);
		}
		me->txtSAMLRespWF->SetText(sb.ToCString());

		NN<Crypto::Cert::X509Key> key;
		if (me->CreateSAMLKey().SetTo(key))
		{
			sb.ClearStr();
			NN<Net::SSLEngine> ssl;
			if (me->ssl.SetTo(ssl))
			{
				if (Net::SAMLUtil::DecryptResponse(ssl, me->core->GetEncFactory(), key, respNew->ToCString(), sb))
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

		respNew->Release();
		me->respNew = 0;
	}
}

void __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnSAMLResponse(AnyType userObj, Text::CStringNN msg)
{
	NN<SSWR::AVIRead::AVIRSAMLTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSAMLTestForm>();
	Sync::MutexUsage mutUsage(me->respMut);
	OPTSTR_DEL(me->respNew);
	me->respNew = Text::String::New(msg);
}

Bool __stdcall SSWR::AVIRead::AVIRSAMLTestForm::OnLoginRequest(AnyType userObj, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, NN<const Net::WebServer::SAMLMessage> msg)
{
	NN<SSWR::AVIRead::AVIRSAMLTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSAMLTestForm>();
	Text::StringBuilderUTF8 sb;
	Optional<Text::String> decMsg = 0;
	NN<Text::String> nndecMsg;
	NN<Crypto::Cert::X509Key> key;
	if (me->CreateSAMLKey().SetTo(key))
	{
		NN<Net::SSLEngine> ssl;
		if (me->ssl.SetTo(ssl) && Net::SAMLUtil::DecryptResponse(ssl, me->core->GetEncFactory(), key, msg->rawMessage.OrEmpty(), sb))
		{
			IO::MemoryReadingStream mstm(sb.v, sb.GetLength());
			Text::StringBuilderUTF8 sb2;
			Text::XMLReader::XMLWellFormat(me->core->GetEncFactory(), mstm, 0, sb2);
			decMsg = Text::XML::ToNewHTMLTextXMLColor(sb2.ToString());
		}
		key.Delete();
	}
	{
		IO::MemoryReadingStream mstm(msg->rawMessage.OrEmpty().v, msg->rawMessage.leng);
		sb.ClearStr();
		Text::XMLReader::XMLWellFormat(me->core->GetEncFactory(), mstm, 0, sb);
	}
	NN<Text::String> msgContent = Text::XML::ToNewHTMLTextXMLColor(sb.ToString());
	sb.ClearStr();
	sb.AppendC(UTF8STRC("<html><head><title>SAML Message</title></head><body>"));
	if (decMsg.SetTo(nndecMsg))
	{
		sb.AppendC(UTF8STRC("<h1>Decrypted Content</h1>"));
		sb.Append(nndecMsg);
		sb.AppendC(UTF8STRC("<hr/>"));
		nndecMsg->Release();
	}
	sb.AppendC(UTF8STRC("<h1>RAW Response</h1>"));
	sb.Append(msgContent);
	sb.AppendC(UTF8STRC("</body></html>"));
	msgContent->Release();
	resp->AddDefHeaders(req);
	resp->AddCacheControl(0);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToByteArray());
	return true;
}

void SSWR::AVIRead::AVIRSAMLTestForm::ClearCACerts()
{
	this->caCerts.DeleteAll();
}

Optional<Crypto::Cert::X509Key> SSWR::AVIRead::AVIRSAMLTestForm::CreateSAMLKey()
{
	NN<Net::WebServer::SAMLHandler> samlHdlr;
	if (!this->samlHdlr.SetTo(samlHdlr))
		return 0;
	NN<Crypto::Cert::X509PrivKey> key;
	if (samlHdlr->GetKey().SetTo(key))
		return key->CreateKey();
	return 0;
}

SSWR::AVIRead::AVIRSAMLTestForm::AVIRSAMLTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("SAML Test"));
	this->SetFont(0, 0, 8.25, false);
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->sslCert = 0;
	this->sslKey = 0;
	this->svr = 0;
	this->respNew = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	this->lblPort = ui->NewLabel(this->tpControl, CSTR("Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->tpControl, CSTR("443"));
	this->txtPort->SetRect(104, 4, 50, 23, false);
	this->lblSSL = ui->NewLabel(this->tpControl, CSTR("SSL"));
	this->lblSSL->SetRect(4, 28, 100, 23, false);
	this->btnSSLCert = ui->NewButton(this->tpControl, CSTR("Cert/Key"));
	this->btnSSLCert->SetRect(104, 28, 75,23, false);
	this->btnSSLCert->HandleButtonClick(OnSSLCertClicked, this);
	this->lblSSLCert = ui->NewLabel(this->tpControl, CSTR(""));
	this->lblSSLCert->SetRect(184, 28, 200, 23, false);
	this->lblHost = ui->NewLabel(this->tpControl, CSTR("Host"));
	this->lblHost->SetRect(4, 52, 100, 23, false);
	this->txtHost = ui->NewTextBox(this->tpControl, CSTR("localhost"));
	this->txtHost->SetRect(104, 52, 200, 23, false);
	this->lblSignCert = ui->NewLabel(this->tpControl, CSTR("Signature Cert"));
	this->lblSignCert->SetRect(4, 76, 100, 23, false);
	this->txtSignCert = ui->NewTextBox(this->tpControl, CSTR(""));
	this->txtSignCert->SetRect(104, 76, 500, 23, false);
	this->lblSignKey = ui->NewLabel(this->tpControl, CSTR("Signature Key"));
	this->lblSignKey->SetRect(4, 100, 100, 23, false);
	this->txtSignKey = ui->NewTextBox(this->tpControl, CSTR(""));
	this->txtSignKey->SetRect(104, 100, 500, 23, false);
	this->lblSSOPath = ui->NewLabel(this->tpControl, CSTR("SSO Path"));
	this->lblSSOPath->SetRect(4, 124, 100, 23, false);
	this->txtSSOPath = ui->NewTextBox(this->tpControl, CSTR("/sso"));
	this->txtSSOPath->SetRect(104, 124, 500, 23, false);
	this->lblMetadataPath = ui->NewLabel(this->tpControl, CSTR("Metadata Path"));
	this->lblMetadataPath->SetRect(4, 148, 100, 23, false);
	this->txtMetadataPath = ui->NewTextBox(this->tpControl, CSTR("/metadata"));
	this->txtMetadataPath->SetRect(104, 148, 500, 23, false);
	this->lblLogoutPath = ui->NewLabel(this->tpControl, CSTR("Logout Path"));
	this->lblLogoutPath->SetRect(4, 172, 100, 23, false);
	this->txtLogoutPath = ui->NewTextBox(this->tpControl, CSTR("/logout"));
	this->txtLogoutPath->SetRect(104, 172, 500, 23, false);
	this->btnStart = ui->NewButton(this->tpControl, CSTR("Start"));
	this->btnStart->SetRect(104, 196, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->lblSSOURL = ui->NewLabel(this->tpControl, CSTR("SSO URL"));
	this->lblSSOURL->SetRect(4, 220, 100, 23, false);
	this->txtSSOURL = ui->NewTextBox(this->tpControl, CSTR(""));
	this->txtSSOURL->SetRect(104, 220, 500, 23, false);
	this->txtSSOURL->SetReadOnly(true);
	this->lblMetadataURL = ui->NewLabel(this->tpControl, CSTR("Metadata URL"));
	this->lblMetadataURL->SetRect(4, 244, 100, 23, false);
	this->txtMetadataURL = ui->NewTextBox(this->tpControl, CSTR(""));
	this->txtMetadataURL->SetRect(104, 244, 500, 23, false);
	this->txtMetadataURL->SetReadOnly(true);
	this->lblLogoutURL = ui->NewLabel(this->tpControl, CSTR("Logout URL"));
	this->lblLogoutURL->SetRect(4, 268, 100, 23, false);
	this->txtLogoutURL = ui->NewTextBox(this->tpControl, CSTR(""));
	this->txtLogoutURL->SetRect(104, 268, 500, 23, false);
	this->txtLogoutURL->SetReadOnly(true);

	this->tpSAMLResp = this->tcMain->AddTabPage(CSTR("SAML Resp"));
	this->txtSAMLResp = ui->NewTextBox(this->tpSAMLResp, CSTR(""), true);
	this->txtSAMLResp->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtSAMLResp->SetReadOnly(true);

	this->tpSAMLRespWF = this->tcMain->AddTabPage(CSTR("Resp WF"));
	this->txtSAMLRespWF = ui->NewTextBox(this->tpSAMLRespWF, CSTR(""), true);
	this->txtSAMLRespWF->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtSAMLRespWF->SetReadOnly(true);

	this->tpSAMLDecrypt = this->tcMain->AddTabPage(CSTR("SAML Decrypt"));
	this->txtSAMLDecrypt = ui->NewTextBox(this->tpSAMLDecrypt, CSTR(""), true);
	this->txtSAMLDecrypt->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtSAMLDecrypt->SetReadOnly(true);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSel, this);

	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 500, true));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);

	this->HandleDropFiles(OnFormFiles, this);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRSAMLTestForm::~AVIRSAMLTestForm()
{
	this->svr.Delete();
	this->samlHdlr.Delete();
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
	this->ssl.Delete();
	this->sslCert.Delete();
	this->sslKey.Delete();
	this->ClearCACerts();
	OPTSTR_DEL(this->respNew);
}

void SSWR::AVIRead::AVIRSAMLTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
