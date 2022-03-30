#include "Stdafx.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Net/WebServer/SAMLHandler.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/Base64Enc.h"

#include <stdio.h>

Net::WebServer::SAMLHandler::~SAMLHandler()
{
	SDEL_STRING(this->serverHost);
	SDEL_STRING(this->metadataPath);
	SDEL_STRING(this->logoutPath);
	SDEL_STRING(this->ssoPath);
	SDEL_CLASS(this->signCert);
	SDEL_CLASS(this->signKey);
}

Bool Net::WebServer::SAMLHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, UOSInt subReqLen)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	if (this->initErr == SAMLError::None)
	{
		if (this->metadataPath->Equals(subReq, subReqLen))
		{
			Text::TextBinEnc::Base64Enc b64;
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
			sb.AppendC(UTF8STRC("<md:EntityDescriptor xmlns:md=\"urn:oasis:names:tc:SAML:2.0:metadata\" ID=\""));
			sptr = Text::StrConcatC(sbuff, UTF8STRC("https://"));
			sptr = this->serverHost->ConcatTo(sptr);
			sptr = this->metadataPath->ConcatTo(sptr);
			Text::StrReplace(sbuff, ':', '_');
			Text::StrReplace(sbuff, '/', '_');
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sb.AppendC(UTF8STRC("\" entityID=\""));
			sptr = Text::StrConcatC(sbuff, UTF8STRC("https://"));
			sptr = this->serverHost->ConcatTo(sptr);
			sptr = this->metadataPath->ConcatTo(sptr);
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sb.AppendC(UTF8STRC("\">"));
			sb.AppendC(UTF8STRC("<md:SPSSODescriptor AuthnRequestsSigned=\"true\" WantAssertionsSigned=\"true\" protocolSupportEnumeration=\"urn:oasis:names:tc:SAML:2.0:protocol\">"));
			sb.AppendC(UTF8STRC("<md:KeyDescriptor use=\"signing\">"));
			sb.AppendC(UTF8STRC("<ds:KeyInfo xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\">"));
			sb.AppendC(UTF8STRC("<ds:X509Data>"));
			sb.AppendC(UTF8STRC("<ds:X509Certificate>"));
			b64.EncodeBin(&sb, this->signCert->GetASN1Buff(), this->signCert->GetASN1BuffSize(), Text::LineBreakType::CRLF, 76);
			sb.AppendC(UTF8STRC("</ds:X509Certificate>"));
			sb.AppendC(UTF8STRC("</ds:X509Data>"));
			sb.AppendC(UTF8STRC("</ds:KeyInfo>"));
			sb.AppendC(UTF8STRC("</md:KeyDescriptor>"));
			sb.AppendC(UTF8STRC("<md:KeyDescriptor use=\"encryption\">"));
			sb.AppendC(UTF8STRC("<ds:KeyInfo xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\">"));
			sb.AppendC(UTF8STRC("<ds:X509Data>"));
			sb.AppendC(UTF8STRC("<ds:X509Certificate>"));
			b64.EncodeBin(&sb, this->signCert->GetASN1Buff(), this->signCert->GetASN1BuffSize(), Text::LineBreakType::CRLF, 76);
			sb.AppendC(UTF8STRC("</ds:X509Certificate>"));
			sb.AppendC(UTF8STRC("</ds:X509Data>"));
			sb.AppendC(UTF8STRC("</ds:KeyInfo>"));
			sb.AppendC(UTF8STRC("</md:KeyDescriptor>"));
			sb.AppendC(UTF8STRC("<md:SingleLogoutService Binding=\"urn:oasis:names:tc:SAML:2.0:bindings:HTTP-POST\" Location=\"https://"));
			sb.Append(this->serverHost);
			sb.Append(this->logoutPath);
			sb.AppendC(UTF8STRC("\"/>"));
			sb.AppendC(UTF8STRC("<md:SingleLogoutService Binding=\"urn:oasis:names:tc:SAML:2.0:bindings:HTTP-Redirect\" Location=\"https://"));
			sb.Append(this->serverHost);
			sb.Append(this->logoutPath);
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
			sb.Append(this->serverHost);
			sb.Append(this->ssoPath);
			sb.AppendC(UTF8STRC("\" index=\"0\" isDefault=\"true\"/>"));
			sb.AppendC(UTF8STRC("<md:AssertionConsumerService Binding=\"urn:oasis:names:tc:SAML:2.0:bindings:HTTP-Artifact\" Location=\"https://"));
			sb.Append(this->serverHost);
			sb.Append(this->ssoPath);
			sb.AppendC(UTF8STRC("\" index=\"1\"/>"));
			sb.AppendC(UTF8STRC("</md:SPSSODescriptor>"));
			sb.AppendC(UTF8STRC("</md:EntityDescriptor>"));
			resp->AddDefHeaders(req);
			resp->AddCacheControl(0);
			resp->AddContentType(UTF8STRC("application/samlmetadata+xml"));
			return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/samlmetadata+xml"), sb.GetLength(), sb.ToString());
		}
		else if (this->ssoPath->Equals(subReq, subReqLen))
		{
			Bool succ = false;
			if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_POST)
			{
				req->ParseHTTPForm();
				Text::String *s = req->GetHTTPFormStr(CSTR("SAMLResponse"));
				if (s)
				{
					Text::TextBinEnc::Base64Enc b64;
					UInt8 *buff = MemAlloc(UInt8, s->leng + 1);
					UOSInt buffSize;
					buffSize = b64.DecodeBin(s->v, s->leng, buff);
					buff[buffSize] = 0;
					printf("SAMLResponse: %s\r\n", buff);
					MemFree(buff);
				}
			}
			if (!succ)
			{
				resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			}
			return true;
		}
		else if (this->logoutPath->Equals(subReq, subReqLen))
		{
			resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
			return true;
		}
		//////////////////////////////////
	}

	if (this->DoRequest(req, resp, subReq, subReqLen))
	{
		return true;
	}
	if (this->defHdlr)
	{
		return this->defHdlr->ProcessRequest(req, resp, subReq, subReqLen);
	}
	return false;
}

Net::WebServer::SAMLHandler::SAMLHandler(SAMLConfig *cfg, Net::SSLEngine *ssl, WebStandardHandler *defHdlr)
{
	this->defHdlr = defHdlr;
	this->ssl = ssl;
	this->serverHost = 0;
	this->metadataPath = 0;
	this->logoutPath = 0;
	this->ssoPath = 0;
	this->signCert = 0;
	this->signKey = 0;
	if (cfg == 0)
	{
		this->initErr = SAMLError::ConfigNotFound;
		return;
	}
	if (cfg->serverHost.leng == 0)
	{
		this->initErr = SAMLError::ServerHost;
		return;
	}
	this->serverHost = Text::String::New(cfg->serverHost);
	if (cfg->metadataPath.leng == 0 || cfg->metadataPath.v[0] != '/')
	{
		this->initErr = SAMLError::MetadataPath;
		return;
	}
	this->metadataPath = Text::String::New(cfg->metadataPath);
	if (cfg->logoutPath.leng == 0 || cfg->logoutPath.v[0] != '/')
	{
		this->initErr = SAMLError::LogoutPath;
		return;
	}
	this->logoutPath = Text::String::New(cfg->logoutPath);
	if (cfg->ssoPath.leng == 0 || cfg->ssoPath.v[0] != '/')
	{
		this->initErr = SAMLError::SSOPath;
		return;
	}
	this->ssoPath = Text::String::New(cfg->ssoPath);
	Parser::FileParser::X509Parser parser;
	if (cfg->signCertPath.leng == 0)
	{
		this->initErr = SAMLError::SignCert;
		return;
	}
	Crypto::Cert::X509File *x509;
	x509 = Parser::FileParser::X509Parser::ToType(parser.ParseFilePath(cfg->signCertPath), Crypto::Cert::X509File::FileType::Cert);
	if (x509 == 0)
	{
		this->initErr = SAMLError::SignCert;
		return;
	}
	this->signCert = (Crypto::Cert::X509Cert*)x509;

	if (cfg->signKeyPath.leng == 0)
	{
		this->initErr = SAMLError::SignCert;
		return;
	}
	x509 = Parser::FileParser::X509Parser::ToType(parser.ParseFilePath(cfg->signKeyPath), Crypto::Cert::X509File::FileType::PrivateKey);
	if (x509 == 0)
	{
		this->initErr = SAMLError::SignKey;
		return;
	}
	this->signKey = (Crypto::Cert::X509PrivKey*)x509;

	this->initErr = SAMLError::None;
}

Net::WebServer::SAMLError Net::WebServer::SAMLHandler::GetInitError()
{
	return this->initErr;
}
