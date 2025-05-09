#include "Stdafx.h"
#include "Data/Compress/Deflater.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Net/WebServer/SAMLHandler.h"
#include "Parser/FileParser/X509Parser.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/FormEncoding.h"

// https://<host>/adfs/ls/idpinitiatedsignon.aspx
// https://<host>/FederationMetadata/2007-06/FederationMetadata.xml
// https://www.samltool.com/decrypt.php
// https://support.f5.com/csp/article/K51854802

Net::WebServer::SAMLHandler::~SAMLHandler()
{
	OPTSTR_DEL(this->serverHost);
	OPTSTR_DEL(this->metadataPath);
	OPTSTR_DEL(this->loginPath);
	OPTSTR_DEL(this->logoutPath);
	OPTSTR_DEL(this->ssoPath);
	this->signCert.Delete();
	this->signKey.Delete();
}

Bool Net::WebServer::SAMLHandler::ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	NN<Crypto::Cert::X509Cert> cert;
	NN<Crypto::Cert::X509PrivKey> privKey;
	NN<Text::String> metadataPath;
	NN<Text::String> loginPath;
	NN<Text::String> logoutPath;
	NN<Text::String> ssoPath;
	NN<Text::String> serverHost;
	if (this->initErr == SAMLInitError::None && this->signCert.SetTo(cert) && this->signKey.SetTo(privKey) && this->serverHost.SetTo(serverHost))
	{
		if (this->metadataPath.SetTo(metadataPath) && metadataPath->Equals(subReq.v, subReq.leng))
		{
			if (!this->DoMetadataGet(req, resp))
			{
				resp->ResponseError(req, Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
			}
			return true;
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
		else if (this->loginPath.SetTo(loginPath) && loginPath->Equals(subReq.v, subReq.leng))
		{
			return this->DoLoginGet(req, resp);
		}
		else if (this->logoutPath.SetTo(logoutPath) && logoutPath->Equals(subReq.v, subReq.leng))
		{
			return this->DoLogoutGet(req, resp, CSTR("id"));
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

void Net::WebServer::SAMLHandler::SendRedirect(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN url, Text::CStringNN reqContent, Crypto::Hash::HashType hashType)
{
	UnsafeArray<UInt8> buff = MemAllocArr(UInt8, reqContent.leng + 16);
	UOSInt buffSize;
	UInt8 signBuff[512];
	UOSInt signSize;
	NN<Net::SSLEngine> ssl;
	NN<Crypto::Cert::X509PrivKey> privKey;
	NN<Crypto::Cert::X509Key> key;
	if (!this->signKey.SetTo(privKey) || !this->ssl.SetTo(ssl) || !privKey->CreateKey().SetTo(key))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return;
	}
	if (!Data::Compress::Deflater::CompressDirect(Data::ByteArray(buff, reqContent.leng + 16), buffSize, reqContent.ToByteArray(), Data::Compress::Deflater::CompLevel::BestCompression, false))
	{
		MemFreeArr(buff);
		printf("SAMLHandler: Error in compressing content\r\n");
		resp->ResponseError(req, Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
		key.Delete();
		return;
	}
	Text::TextBinEnc::Base64Enc b64;
	Text::TextBinEnc::FormEncoding uri;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;

	sb.Append(CSTR("SAMLRequest="));
	b64.EncodeBin(sb2, buff, buffSize);
	uri.EncodeBin(sb, sb2.v, sb2.leng);

	if (hashType == Crypto::Hash::HashType::SHA256)
	{
		sb.Append(CSTR("&SigAlg=http%3A%2F%2Fwww.w3.org%2F2001%2F04%2Fxmldsig-more%23rsa-sha256"));
	}
	else if (hashType == Crypto::Hash::HashType::SHA384)
	{
		sb.Append(CSTR("&SigAlg=http%3A%2F%2Fwww.w3.org%2F2001%2F04%2Fxmldsig-more%23rsa-sha384"));
	}
	else if (hashType == Crypto::Hash::HashType::SHA512)
	{
		sb.Append(CSTR("&SigAlg=http%3A%2F%2Fwww.w3.org%2F2001%2F04%2Fxmldsig-more%23rsa-sha512"));
	}
	else
	{
		hashType = Crypto::Hash::HashType::SHA1;
		sb.Append(CSTR("&SigAlg=http%3A%2F%2Fwww.w3.org%2F2000%2F09%2Fxmldsig%23rsa-sha1"));
	}
	MemFreeArr(buff);
	if (ssl->Signature(key, hashType, sb.ToByteArray(), signBuff, signSize))
	{
		sb.Append(CSTR("&Signature="));
		sb2.ClearStr();
		b64.EncodeBin(sb2, signBuff, signSize);
		uri.EncodeBin(sb, sb2.v, sb2.leng);

		sb2.ClearStr();
		sb2.Append(url);
		sb2.AppendUTF8Char('?');
		sb2.Append(sb);
		resp->RedirectURL(req, sb2.ToCString(), 0);
	}
	else
	{
		printf("SAMLHandler: Error in Signature\r\n");
		resp->ResponseError(req, Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
	}
	key.Delete();
}

Net::WebServer::SAMLHandler::SAMLHandler(NN<SAMLConfig> cfg, Optional<Net::SSLEngine> ssl, Optional<WebStandardHandler> defHdlr)
{
	Text::CStringNN nns;
	this->defHdlr = defHdlr;
	this->ssl = ssl;
	this->serverHost = 0;
	this->metadataPath = 0;
	this->loginPath = 0;
	this->logoutPath = 0;
	this->ssoPath = 0;
	this->signCert = 0;
	this->signKey = 0;
	this->rawRespHdlr = 0;
	this->rawRespObj = 0;
	this->loginHdlr = 0;
	this->loginObj = 0;
	this->idp = 0;
	this->hashType = Crypto::Hash::HashType::SHA1;
	if (!cfg->serverHost.SetTo(nns) || nns.leng == 0)
	{
		this->initErr = SAMLInitError::ServerHost;
		return;
	}
	this->serverHost = Text::String::New(nns);
	if (!cfg->metadataPath.SetTo(nns) || nns.leng == 0 || nns.v[0] != '/')
	{
		this->initErr = SAMLInitError::MetadataPath;
		return;
	}
	this->metadataPath = Text::String::New(nns);
	if (!cfg->loginPath.SetTo(nns) || nns.leng == 0 || nns.v[0] != '/')
	{
		this->initErr = SAMLInitError::LoginPath;
		return;
	}
	this->loginPath = Text::String::New(nns);
	if (!cfg->logoutPath.SetTo(nns) || nns.leng == 0 || nns.v[0] != '/')
	{
		this->initErr = SAMLInitError::LogoutPath;
		return;
	}
	this->logoutPath = Text::String::New(nns);
	if (!cfg->ssoPath.SetTo(nns) || nns.leng == 0 || nns.v[0] != '/')
	{
		this->initErr = SAMLInitError::SSOPath;
		return;
	}
	this->ssoPath = Text::String::New(nns);
	Parser::FileParser::X509Parser parser;
	if (cfg->signCertPath.leng == 0)
	{
		this->initErr = SAMLInitError::SignCert;
		return;
	}
	NN<IO::ParsedObject> pobj;
	if (!parser.ParseFilePath(cfg->signCertPath).SetTo(pobj))
	{
		this->initErr = SAMLInitError::SignCert;
		return;
	}
	NN<Crypto::Cert::X509File> x509;
	if (!Parser::FileParser::X509Parser::ToType(pobj, Crypto::Cert::X509File::FileType::Cert).SetTo(x509))
	{
		this->initErr = SAMLInitError::SignCert;
		return;
	}
	this->signCert = NN<Crypto::Cert::X509Cert>::ConvertFrom(x509);

	if (cfg->signKeyPath.leng == 0)
	{
		this->initErr = SAMLInitError::SignCert;
		return;
	}
	if (!parser.ParseFilePath(cfg->signKeyPath).SetTo(pobj))
	{
		this->initErr = SAMLInitError::SignKey;
		return;
	}
	if (!Parser::FileParser::X509Parser::ToType(pobj, Crypto::Cert::X509File::FileType::PrivateKey).SetTo(x509))
	{
		this->initErr = SAMLInitError::SignKey;
		return;
	}
	this->signKey = NN<Crypto::Cert::X509PrivKey>::ConvertFrom(x509);

	this->initErr = SAMLInitError::None;
}

Net::WebServer::SAMLInitError Net::WebServer::SAMLHandler::GetInitError()
{
	return this->initErr;
}

Bool Net::WebServer::SAMLHandler::GetLoginURL(NN<Text::StringBuilderUTF8> sb)
{
	NN<Text::String> serverHost;
	NN<Text::String> loginPath;
	if (!this->serverHost.SetTo(serverHost) || !this->loginPath.SetTo(loginPath))
	{
		return false;
	}
	sb->AppendC(UTF8STRC("https://"));
	sb->Append(serverHost);
	sb->Append(loginPath);
	return true;
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

void Net::WebServer::SAMLHandler::SetIdp(NN<Net::SAMLIdpConfig> idp)
{
	Sync::MutexUsage mutUsage(this->idpMut);
	this->idp = idp;
}

void Net::WebServer::SAMLHandler::SetHashType(Crypto::Hash::HashType hashType)
{
	this->hashType = hashType;
}

Bool Net::WebServer::SAMLHandler::DoLoginGet(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> metadataPath;
	NN<Text::String> ssoPath;
	NN<Text::String> serverHost;
	NN<Text::String> s;
	NN<Net::SAMLIdpConfig> idp;
	Sync::MutexUsage mutUsage(this->idpMut);
	if (this->idp.SetTo(idp) && this->serverHost.SetTo(serverHost) && this->metadataPath.SetTo(metadataPath) && this->ssoPath.SetTo(ssoPath))
	{
		Data::Timestamp currTime = Data::Timestamp::UtcNow();
		Text::StringBuilderUTF8 sb;
		sb.Append(CSTR("<samlp:AuthnRequest xmlns:samlp=\"urn:oasis:names:tc:SAML:2.0:protocol\" xmlns:saml=\"urn:oasis:names:tc:SAML:2.0:assertion\""));
		sb.Append(CSTR(" ID=\"SAML_"));
		sb.AppendI64(currTime.GetLocalSecs());
		sb.AppendU32(currTime.inst.nanosec);
		sb.AppendUTF8Char('"');
		sb.Append(CSTR(" Version=\"2.0\""));
		sb.Append(CSTR(" ProviderName="));
		s = Text::XML::ToNewAttrText(idp->GetServiceDispName()->v);
		sb.Append(s);
		s->Release();
		sb.Append(CSTR(" IssueInstant=\""));
		sptr = Data::Timestamp(Data::TimeInstant(currTime.inst.sec, 0), 0).ToStringISO8601(sbuff);
		sb.AppendP(sbuff, sptr);
		sb.AppendUTF8Char('"');
		sb.Append(CSTR(" Destination=\""));
		sb.Append(idp->GetSignOnLocation());
		sb.AppendUTF8Char('"');
		sb.Append(CSTR(" ProtocolBinding=\"urn:oasis:names:tc:SAML:2.0:bindings:HTTP-POST\""));
		sb.Append(CSTR(" AssertionConsumerServiceURL=\"https://"));
		sb.Append(serverHost);
		sb.Append(ssoPath);
		sb.Append(CSTR("\">"));
		sb.Append(CSTR("<saml:Issuer>https://"));
		sb.Append(serverHost);
		sb.Append(metadataPath);
		sb.Append(CSTR("</saml:Issuer>"));
		sb.Append(CSTR("<samlp:NameIDPolicy Format=\"urn:oasis:names:tc:SAML:1.1:nameid-format:unspecified\" AllowCreate=\"true\"/>"));
		sb.Append(CSTR("<samlp:RequestedAuthnContext Comparison=\"exact\">"));
		sb.Append(CSTR("<saml:AuthnContextClassRef>urn:oasis:names:tc:SAML:2.0:ac:classes:PasswordProtectedTransport</saml:AuthnContextClassRef>"));
		sb.Append(CSTR("</samlp:RequestedAuthnContext>"));
		sb.Append(CSTR("</samlp:AuthnRequest>"));

		this->SendRedirect(req, resp, idp->GetSignOnLocation()->ToCString(), sb.ToCString(), this->hashType);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
}

Bool Net::WebServer::SAMLHandler::DoLogoutGet(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN nameID)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> metadataPath;
	NN<Text::String> serverHost;
	NN<Net::SAMLIdpConfig> idp;
	Sync::MutexUsage mutUsage(this->idpMut);
	if (this->idp.SetTo(idp) && this->serverHost.SetTo(serverHost) && this->metadataPath.SetTo(metadataPath))
	{
		Data::Timestamp currTime = Data::Timestamp::UtcNow();
		Text::StringBuilderUTF8 sb;
		sb.Append(CSTR("<samlp:LogoutRequest xmlns:samlp=\"urn:oasis:names:tc:SAML:2.0:protocol\" xmlns:saml=\"urn:oasis:names:tc:SAML:2.0:assertion\""));
		sb.Append(CSTR(" ID=\"SAML_"));
		sb.AppendI64(currTime.GetLocalSecs());
		sb.AppendU32(currTime.inst.nanosec);
		sb.AppendUTF8Char('"');
		sb.Append(CSTR(" Version=\"2.0\""));
		sb.Append(CSTR(" IssueInstant=\""));
		sptr = Data::Timestamp(Data::TimeInstant(currTime.inst.sec, 0), 0).ToStringISO8601(sbuff);
		sb.AppendP(sbuff, sptr);
		sb.AppendUTF8Char('"');
		sb.Append(CSTR(" Destination=\""));
		sb.Append(idp->GetLogoutLocation());
		sb.AppendUTF8Char('"');
		sb.Append(CSTR(">"));
		sb.Append(CSTR("<saml:Issuer>https://"));
		sb.Append(serverHost);
		sb.Append(metadataPath);
		sb.Append(CSTR("</saml:Issuer>"));
		sb.Append(CSTR("<saml:NameID SPNameQualifier=\"https://"));
		sb.Append(serverHost);
		sb.Append(metadataPath);
		sb.AppendUTF8Char('"');
		sb.Append(CSTR(" Format=\"urn:oasis:names:tc:SAML:2.0:nameid-format:transient\">"));
		sb.Append(nameID);
		sb.Append(CSTR("</saml:NameID>"));
		sb.Append(CSTR("</samlp:LogoutRequest>"));

		this->SendRedirect(req, resp, idp->GetLogoutLocation()->ToCString(), sb.ToCString(), this->hashType);
		return true;
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
}

Bool Net::WebServer::SAMLHandler::DoMetadataGet(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<Crypto::Cert::X509Cert> cert;
	NN<Text::String> metadataPath;
	NN<Text::String> logoutPath;
	NN<Text::String> ssoPath;
	NN<Text::String> serverHost;
	if (this->logoutPath.SetTo(logoutPath) && this->ssoPath.SetTo(ssoPath) && this->metadataPath.SetTo(metadataPath) && this->signCert.SetTo(cert) && this->serverHost.SetTo(serverHost))
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
	return false;
}

Text::CStringNN Net::WebServer::SAMLInitErrorGetName(SAMLInitError err)
{
	switch (err)
	{
	case SAMLInitError::None:
		return CSTR("None");
	case SAMLInitError::ServerHost:
		return CSTR("Server Host error");
	case SAMLInitError::MetadataPath:
		return CSTR("Metadata path error");
	case SAMLInitError::LoginPath:
		return CSTR("Login path error");
	case SAMLInitError::LogoutPath:
		return CSTR("Logout path error");
	case SAMLInitError::SSOPath:
		return CSTR("SSO path error");
	case SAMLInitError::SignCert:
		return CSTR("Signature Certificate Invalid");
	case SAMLInitError::SignKey:
		return CSTR("Signature Key Invalid");
	default:
		return CSTR("Unknown");
	}
}
