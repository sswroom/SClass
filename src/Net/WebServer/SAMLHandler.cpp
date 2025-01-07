#include "Stdafx.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Net/WebServer/SAMLHandler.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/Base64Enc.h"

// https://<host>/adfs/ls/idpinitiatedsignon.aspx
// https://<host>/FederationMetadata/2007-06/FederationMetadata.xml
// https://www.samltool.com/decrypt.php
// https://support.f5.com/csp/article/K51854802

Net::WebServer::SAMLHandler::~SAMLHandler()
{
	OPTSTR_DEL(this->serverHost);
	OPTSTR_DEL(this->metadataPath);
	OPTSTR_DEL(this->logoutPath);
	OPTSTR_DEL(this->ssoPath);
	this->signCert.Delete();
	this->signKey.Delete();
}

Bool Net::WebServer::SAMLHandler::ProcessRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<Crypto::Cert::X509Cert> cert;
	NN<Crypto::Cert::X509PrivKey> privKey;
	NN<Text::String> metadataPath;
	NN<Text::String> logoutPath;
	NN<Text::String> ssoPath;
	NN<Text::String> serverHost;
	if (this->initErr == SAMLError::None && this->signCert.SetTo(cert) && this->signKey.SetTo(privKey) && this->serverHost.SetTo(serverHost))
	{
		if (this->metadataPath.SetTo(metadataPath) && metadataPath->Equals(subReq.v, subReq.leng) && this->logoutPath.SetTo(logoutPath) && this->ssoPath.SetTo(ssoPath))
		{
			Text::TextBinEnc::Base64Enc b64;
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
			sb.AppendC(UTF8STRC("<md:EntityDescriptor xmlns:md=\"urn:oasis:names:tc:SAML:2.0:metadata\" ID=\""));
			sptr = Text::StrConcatC(sbuff, UTF8STRC("https://"));
			sptr = serverHost->ConcatTo(sptr);
			sptr = metadataPath->ConcatTo(sptr);
			Text::StrReplace(sbuff, ':', '_');
			Text::StrReplace(sbuff, '/', '_');
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sb.AppendC(UTF8STRC("\" entityID=\""));
			sptr = Text::StrConcatC(sbuff, UTF8STRC("https://"));
			sptr = serverHost->ConcatTo(sptr);
			sptr = metadataPath->ConcatTo(sptr);
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sb.AppendC(UTF8STRC("\">"));
			sb.AppendC(UTF8STRC("<md:SPSSODescriptor AuthnRequestsSigned=\"true\" WantAssertionsSigned=\"true\" protocolSupportEnumeration=\"urn:oasis:names:tc:SAML:2.0:protocol\">"));
			sb.AppendC(UTF8STRC("<md:KeyDescriptor use=\"signing\">"));
			sb.AppendC(UTF8STRC("<ds:KeyInfo xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\">"));
			sb.AppendC(UTF8STRC("<ds:X509Data>"));
			sb.AppendC(UTF8STRC("<ds:X509Certificate>"));
			b64.EncodeBin(sb, cert->GetASN1Buff(), cert->GetASN1BuffSize(), Text::LineBreakType::CRLF, 76);
			sb.AppendC(UTF8STRC("</ds:X509Certificate>"));
			sb.AppendC(UTF8STRC("</ds:X509Data>"));
			sb.AppendC(UTF8STRC("</ds:KeyInfo>"));
			sb.AppendC(UTF8STRC("</md:KeyDescriptor>"));
			sb.AppendC(UTF8STRC("<md:KeyDescriptor use=\"encryption\">"));
			sb.AppendC(UTF8STRC("<ds:KeyInfo xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\">"));
			sb.AppendC(UTF8STRC("<ds:X509Data>"));
			sb.AppendC(UTF8STRC("<ds:X509Certificate>"));
			b64.EncodeBin(sb, cert->GetASN1Buff(), cert->GetASN1BuffSize(), Text::LineBreakType::CRLF, 76);
			sb.AppendC(UTF8STRC("</ds:X509Certificate>"));
			sb.AppendC(UTF8STRC("</ds:X509Data>"));
			sb.AppendC(UTF8STRC("</ds:KeyInfo>"));
			sb.AppendC(UTF8STRC("</md:KeyDescriptor>"));
			sb.AppendC(UTF8STRC("<md:SingleLogoutService Binding=\"urn:oasis:names:tc:SAML:2.0:bindings:HTTP-POST\" Location=\"https://"));
			sb.Append(serverHost);
			sb.Append(logoutPath);
			sb.AppendC(UTF8STRC("\"/>"));
			sb.AppendC(UTF8STRC("<md:SingleLogoutService Binding=\"urn:oasis:names:tc:SAML:2.0:bindings:HTTP-Redirect\" Location=\"https://"));
			sb.Append(serverHost);
			sb.Append(logoutPath);
			sb.AppendC(UTF8STRC("\"/>"));
			sb.AppendC(UTF8STRC("<md:NameIDFormat>"));
			sb.AppendC(UTF8STRC("urn:oasis:names:tc:SAML:1.1:nameid-format:emailAddress"));
			sb.AppendC(UTF8STRC("</md:NameIDFormat>"));
			sb.AppendC(UTF8STRC("<md:NameIDFormat>"));
			sb.AppendC(UTF8STRC("urn:oasis:names:tc:SAML:2.0:nameid-format:transient"));
			sb.AppendC(UTF8STRC("</md:NameIDFormat>"));
			sb.AppendC(UTF8STRC("<md:NameIDFormat>"));
			sb.AppendC(UTF8STRC("urn:oasis:names:tc:SAML:2.0:nameid-format:persistent"));
			sb.AppendC(UTF8STRC("</md:NameIDFormat>"));
			sb.AppendC(UTF8STRC("<md:NameIDFormat>"));
			sb.AppendC(UTF8STRC("urn:oasis:names:tc:SAML:1.1:nameid-format:unspecified"));
			sb.AppendC(UTF8STRC("</md:NameIDFormat>"));
			sb.AppendC(UTF8STRC("<md:NameIDFormat>"));
			sb.AppendC(UTF8STRC("urn:oasis:names:tc:SAML:1.1:nameid-format:X509SubjectName"));
			sb.AppendC(UTF8STRC("</md:NameIDFormat>"));
			sb.AppendC(UTF8STRC("<md:AssertionConsumerService Binding=\"urn:oasis:names:tc:SAML:2.0:bindings:HTTP-POST\" Location=\"https://"));
			sb.Append(serverHost);
			sb.Append(ssoPath);
			sb.AppendC(UTF8STRC("\" index=\"0\" isDefault=\"true\"/>"));
			sb.AppendC(UTF8STRC("<md:AssertionConsumerService Binding=\"urn:oasis:names:tc:SAML:2.0:bindings:HTTP-Artifact\" Location=\"https://"));
			sb.Append(serverHost);
			sb.Append(ssoPath);
			sb.AppendC(UTF8STRC("\" index=\"1\"/>"));
			sb.AppendC(UTF8STRC("</md:SPSSODescriptor>"));
			sb.AppendC(UTF8STRC("</md:EntityDescriptor>"));
			this->AddResponseHeaders(req, resp);
			resp->AddCacheControl(0);
			resp->AddContentType(CSTR("application/samlmetadata+xml"));
			return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/samlmetadata+xml"), sb.GetLength(), sb.ToString());
		}
		else if (this->ssoPath.SetTo(ssoPath) && ssoPath->Equals(subReq.v, subReq.leng))
		{
			Bool succ = false;
			if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
			{
				req->ParseHTTPForm();
				NN<Text::String> s;
				if (req->GetHTTPFormStr(CSTR("SAMLResponse")).SetTo(s))
				{
					Text::TextBinEnc::Base64Enc b64;
					UInt8 *buff = MemAlloc(UInt8, s->leng + 1);
					UOSInt buffSize;
					buffSize = b64.DecodeBin(s->ToCString(), buff);
					buff[buffSize] = 0;
					if (this->rawRespHdlr)
					{
						this->rawRespHdlr(this->rawRespObj, Text::CStringNN(buff, buffSize));
					}
					if (this->loginHdlr)
					{
						SAMLMessage msg;
						msg.rawMessage = Text::CString(buff, buffSize);
						succ = this->loginHdlr(this->loginObj, req, resp, msg);
					}
					MemFree(buff);
				}
			}
			if (!succ)
			{
				resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			}
			return true;
		}
		else if (this->logoutPath.SetTo(logoutPath) && logoutPath->Equals(subReq.v, subReq.leng))
		{
			resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
			return true;
		}
		//////////////////////////////////
	}

	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}
	NN<Net::WebServer::WebStandardHandler> defHdlr;
	if (this->defHdlr.SetTo(defHdlr))
	{
		return defHdlr->ProcessRequest(req, resp, subReq);
	}
	return false;
}

Net::WebServer::SAMLHandler::SAMLHandler(NN<SAMLConfig> cfg, Optional<Net::SSLEngine> ssl, Optional<WebStandardHandler> defHdlr)
{
	Text::CStringNN nns;
	this->defHdlr = defHdlr;
	this->ssl = ssl;
	this->serverHost = 0;
	this->metadataPath = 0;
	this->logoutPath = 0;
	this->ssoPath = 0;
	this->signCert = 0;
	this->signKey = 0;
	this->rawRespHdlr = 0;
	this->rawRespObj = 0;
	this->loginHdlr = 0;
	this->loginObj = 0;
	if (!cfg->serverHost.SetTo(nns) || nns.leng == 0)
	{
		this->initErr = SAMLError::ServerHost;
		return;
	}
	this->serverHost = Text::String::New(nns);
	if (!cfg->metadataPath.SetTo(nns) || nns.leng == 0 || nns.v[0] != '/')
	{
		this->initErr = SAMLError::MetadataPath;
		return;
	}
	this->metadataPath = Text::String::New(nns);
	if (!cfg->logoutPath.SetTo(nns) || nns.leng == 0 || nns.v[0] != '/')
	{
		this->initErr = SAMLError::LogoutPath;
		return;
	}
	this->logoutPath = Text::String::New(nns);
	if (!cfg->ssoPath.SetTo(nns) || nns.leng == 0 || nns.v[0] != '/')
	{
		this->initErr = SAMLError::SSOPath;
		return;
	}
	this->ssoPath = Text::String::New(nns);
	Parser::FileParser::X509Parser parser;
	if (cfg->signCertPath.leng == 0)
	{
		this->initErr = SAMLError::SignCert;
		return;
	}
	NN<IO::ParsedObject> pobj;
	if (!parser.ParseFilePath(cfg->signCertPath).SetTo(pobj))
	{
		this->initErr = SAMLError::SignCert;
		return;
	}
	NN<Crypto::Cert::X509File> x509;
	if (!Parser::FileParser::X509Parser::ToType(pobj, Crypto::Cert::X509File::FileType::Cert).SetTo(x509))
	{
		this->initErr = SAMLError::SignCert;
		return;
	}
	this->signCert = NN<Crypto::Cert::X509Cert>::ConvertFrom(x509);

	if (cfg->signKeyPath.leng == 0)
	{
		this->initErr = SAMLError::SignCert;
		return;
	}
	if (!parser.ParseFilePath(cfg->signKeyPath).SetTo(pobj))
	{
		this->initErr = SAMLError::SignKey;
		return;
	}
	if (!Parser::FileParser::X509Parser::ToType(pobj, Crypto::Cert::X509File::FileType::PrivateKey).SetTo(x509))
	{
		this->initErr = SAMLError::SignKey;
		return;
	}
	this->signKey = NN<Crypto::Cert::X509PrivKey>::ConvertFrom(x509);

	this->initErr = SAMLError::None;
}

Net::WebServer::SAMLError Net::WebServer::SAMLHandler::GetInitError()
{
	return this->initErr;
}

Bool Net::WebServer::SAMLHandler::GetLogoutURL(NN<Text::StringBuilderUTF8> sb)
{
	NN<Text::String> serverHost;
	NN<Text::String> logoutPath;
	if (!this->serverHost.SetTo(serverHost) || !this->logoutPath.SetTo(logoutPath))
	{
		return false;
	}
	sb->AppendC(UTF8STRC("https://"));
	sb->Append(serverHost);
	sb->Append(logoutPath);
	return true;
}

Bool Net::WebServer::SAMLHandler::GetMetadataURL(NN<Text::StringBuilderUTF8> sb)
{
	NN<Text::String> serverHost;
	NN<Text::String> metadataPath;
	if (!this->serverHost.SetTo(serverHost) || !this->metadataPath.SetTo(metadataPath))
	{
		return false;
	}
	sb->AppendC(UTF8STRC("https://"));
	sb->Append(serverHost);
	sb->Append(metadataPath);
	return true;
}

Bool Net::WebServer::SAMLHandler::GetSSOURL(NN<Text::StringBuilderUTF8> sb)
{
	NN<Text::String> serverHost;
	NN<Text::String> ssoPath;
	if (!this->serverHost.SetTo(serverHost) || !this->ssoPath.SetTo(ssoPath))
	{
		return false;
	}
	sb->AppendC(UTF8STRC("https://"));
	sb->Append(serverHost);
	sb->Append(ssoPath);
	return true;
}

void Net::WebServer::SAMLHandler::HandleRAWSAMLResponse(SAMLStrFunc hdlr, AnyType userObj)
{
	this->rawRespHdlr = hdlr;
	this->rawRespObj = userObj;
}

void Net::WebServer::SAMLHandler::HandleLoginRequest(SAMLLoginFunc hdlr, AnyType userObj)
{
	this->loginHdlr = hdlr;
	this->loginObj = userObj;
}

Optional<Crypto::Cert::X509PrivKey> Net::WebServer::SAMLHandler::GetKey()
{
	return this->signKey;
}

Text::CStringNN Net::WebServer::SAMLErrorGetName(SAMLError err)
{
	switch (err)
	{
	case SAMLError::None:
		return CSTR("None");
	case SAMLError::ServerHost:
		return CSTR("Server Host error");
	case SAMLError::MetadataPath:
		return CSTR("Metadata path error");
	case SAMLError::LogoutPath:
		return CSTR("Logout path error");
	case SAMLError::SSOPath:
		return CSTR("SSO path error");
	case SAMLError::SignCert:
		return CSTR("Signature Certificate Invalid");
	case SAMLError::SignKey:
		return CSTR("Signature Key Invalid");
	default:
		return CSTR("Unknown");
	}
}
