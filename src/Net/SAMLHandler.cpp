#include "Stdafx.h"
#include "Crypto/Cert/X509PubKey.h"
#include "Data/Compress/Deflater.h"
#include "Data/Compress/Inflater.h"
#include "IO/MemoryReadingStream.h"
#include "IO/MemoryStream.h"
#include "Net/SAMLHandler.h"
#include "Net/SAMLUtil.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Parser/FileParser/X509Parser.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"
#include "Text/XMLDOM.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/FormEncoding.h"

// https://<host>/adfs/ls/idpinitiatedsignon.aspx
// https://<host>/FederationMetadata/2007-06/FederationMetadata.xml
// https://www.samltool.com/decrypt.php
// https://support.f5.com/csp/article/K51854802

void Net::SAMLHandler::SendRedirect(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN url, Text::CStringNN reqContent, Crypto::Hash::HashType hashType, Bool response)
{
	Text::StringBuilderUTF8 sb;
	if (this->BuildRedirectURL(sb, url, reqContent, hashType, response))
	{
		resp->RedirectURL(req, sb.ToCString(), 0);
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
	}
}

Bool Net::SAMLHandler::BuildRedirectURL(NN<Text::StringBuilderUTF8> sbOut, Text::CStringNN url, Text::CStringNN reqContent, Crypto::Hash::HashType hashType, Bool response)
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
		return false;
	}
	if (!Data::Compress::Deflater::CompressDirect(Data::ByteArray(buff, reqContent.leng + 16), buffSize, reqContent.ToByteArray(), Data::Compress::Deflater::CompLevel::BestCompression, false))
	{
		MemFreeArr(buff);
		printf("SAMLHandler: Error in compressing content\r\n");
		key.Delete();
		return false;
	}
	Text::TextBinEnc::Base64Enc b64;
	Text::TextBinEnc::FormEncoding uri;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;

	if (response)
	{
		sb.Append(CSTR("SAMLResponse="));
	}
	else
	{
		sb.Append(CSTR("SAMLRequest="));
	}
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
		key.Delete();

		sbOut->ClearStr();
		sbOut->Append(url);
		sbOut->AppendUTF8Char('?');
		sbOut->Append(sb);
		return true;
	}
	else
	{
		key.Delete();
		printf("SAMLHandler: Error in Signature\r\n");
		return false;
	}
}

Net::SAMLSignError Net::SAMLHandler::VerifyHTTPRedirect(NN<Net::SSLEngine> ssl, NN<SAMLIdpConfig> idp, NN<Net::WebServer::WebRequest> req)
{
	UTF8Char sbuff[4096];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> signature;
	NN<Text::String> sigAlg;
	NN<Crypto::Cert::X509Cert> signCert;

	if (!idp->GetSigningCert().SetTo(signCert))
	{
		return SAMLSignError::CertMissing;
	}
	if (!req->GetQueryValue(CSTR("Signature")).SetTo(signature))
	{
		return SAMLSignError::SignatureMissing;
	}
	if (!req->GetQueryValue(CSTR("SigAlg")).SetTo(sigAlg))
	{
		return SAMLSignError::SigAlgMissing;
	}
	Crypto::Hash::HashType hashType = Crypto::Hash::HashType::Unknown;
	if (sigAlg->Equals(CSTR("http://www.w3.org/2001/04/xmldsig-more#rsa-sha256")))
	{
		hashType = Crypto::Hash::HashType::SHA256;
	}
	else if (sigAlg->Equals(CSTR("http://www.w3.org/2001/04/xmldsig-more#rsa-sha384")))
	{
		hashType = Crypto::Hash::HashType::SHA384;
	}
	else if (sigAlg->Equals(CSTR("http://www.w3.org/2001/04/xmldsig-more#rsa-sha512")))
	{
		hashType = Crypto::Hash::HashType::SHA512;
	}
	else if (sigAlg->Equals(CSTR("http://www.w3.org/2000/09/xmldsig#rsa-sha1")))
	{
		hashType = Crypto::Hash::HashType::SHA1;
	}
	else
	{
		return SAMLSignError::SigAlgNotSupported;
	}
	if (!req->GetQueryString(sbuff, sizeof(sbuff) - 1).SetTo(sptr))
	{
		return SAMLSignError::QueryStringGetError;
	}
	UOSInt i = Text::StrIndexOfC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("&Signature="));
	if (i == INVALID_INDEX)
	{
		return SAMLSignError::QueryStringSearchError;
	}
	UOSInt j = Text::StrIndexOfC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("&SigAlg="));
	if (j != INVALID_INDEX && j > i)
	{
		MemCopyO(&sbuff[i], &sbuff[j], (UOSInt)(sptr - &sbuff[j]));
		i += (UOSInt)(sptr - &sbuff[j]);
	}
	Text::TextBinEnc::Base64Enc b64;
	UnsafeArray<UInt8> signBuff = MemAllocArr(UInt8, signature->leng);
	UOSInt signSize = b64.DecodeBin(signature->ToCString(), signBuff);
	NN<Crypto::Cert::X509Key> key;
	if (!signCert->GetNewPublicKey().SetTo(key))
	{
		MemFreeArr(signBuff);
		return SAMLSignError::KeyError;
	}
	if (!ssl->SignatureVerify(key, hashType, Data::ByteArrayR(sbuff, i), Data::ByteArrayR(signBuff, signSize)))
	{
		MemFreeArr(signBuff);
		key.Delete();
		return SAMLSignError::SignatureInvalid;
	}
	MemFreeArr(signBuff);
	key.Delete();
	return SAMLSignError::Valid;
}

void Net::SAMLHandler::ParseSAMLLogoutResponse(NN<SAMLLogoutResponse> saml, NN<IO::Stream> stm)
{
	NN<Text::XMLAttrib> attr;
	Text::CStringNN cs;
	NN<Text::String> s;
	UOSInt i;
	Text::XMLReader reader(this->encFact, stm, Text::XMLReader::PM_XML);
	if (reader.NextElementName2().SetTo(cs) && cs.Equals(CSTR("LogoutResponse")))
	{
		while (reader.NextElementName2().SetTo(cs))
		{
			if (cs.Equals(CSTR("Status")))
			{
				while (reader.NextElementName2().SetTo(cs))
				{
					if (cs.Equals(CSTR("StatusCode")))
					{
						i = reader.GetAttribCount();
						while (i-- > 0)
						{
							attr = reader.GetAttribNoCheck(i);
							if (attr->name.SetTo(s) && s->Equals(CSTR("Value")))
							{
								if (attr->value.SetTo(s))
								{
									saml->SetStatus(Net::SAMLStatusCodeFromString(s->ToCString()));
								}
							}
						}
					}
					reader.SkipElement();
				}
			}
			else
			{
				reader.SkipElement();
			}
		}
	}
	else
	{
		saml->SetError(Net::SAMLLogoutResponse::ProcessError::MessageInvalid);
		saml->SetErrorMessage(CSTR("XML element is not LogoutResponse"));
	}
}

void Net::SAMLHandler::ParseSAMLLogoutRequest(NN<SAMLLogoutRequest> saml, NN<IO::Stream> stm)
{
	NN<Text::XMLAttrib> attr;
	Text::CStringNN cs;
	NN<Text::String> s;
	UOSInt i;
	Text::XMLReader reader(this->encFact, stm, Text::XMLReader::PM_XML);
	if (reader.NextElementName2().SetTo(cs) && cs.Equals(CSTR("LogoutRequest")))
	{
		Text::StringBuilderUTF8 sb;
		i = reader.GetAttribCount();
		while (i-- > 0)
		{
			attr = reader.GetAttribNoCheck(i);
			if (attr->name.SetTo(s))
			{
				if (s->Equals(CSTR("ID")))
				{
					saml->SetID(OPTSTR_CSTR(attr->value));
				}
				else if (s->Equals(CSTR("Destination")))
				{
					if (attr->value.SetTo(s))
					{
						sb.ClearStr();
						sb.Append(CSTR("https://"));
						sb.AppendOpt(this->serverHost);
						sb.AppendOpt(this->logoutPath);
						if (!sb.Equals(s))
						{
							saml->SetError(Net::SAMLLogoutRequest::ProcessError::DestinationInvalid);
							sb.ClearStr();
							sb.Append(CSTR("Destination is not valid: "));
							sb.Append(s);
							saml->SetErrorMessage(sb.ToCString());
							return;
						}
					}
				}
				else if (s->Equals(CSTR("NotOnOrAfter")))
				{
					Data::Timestamp notOnOrAfter = Data::Timestamp::FromStr(Text::String::OrEmpty(attr->value)->ToCString(), 0);
					if (!notOnOrAfter.IsNull() && Data::Timestamp::UtcNow() >= notOnOrAfter)
					{
						saml->SetError(Net::SAMLLogoutRequest::ProcessError::TimeInvalid);
						saml->SetErrorMessage(CSTR("Current time is >= NotOnOrAfter"));
						return;
					}
				}
			}
		}
		while (reader.NextElementName2().SetTo(cs))
		{
			if (cs.Equals(CSTR("Issuer")))
			{
				sb.ClearStr();
				reader.ReadNodeText(sb);
				Sync::MutexUsage mutUsage(this->idpMut);
				NN<Net::SAMLIdpConfig> idp;
				if (this->idp.SetTo(idp))
				{
					if (!idp->GetEntityId()->Equals(sb.ToCString()))
					{
						saml->SetError(Net::SAMLLogoutRequest::ProcessError::IssuerInvalid);
						saml->SetErrorMessage(CSTR("Issuer is not same as IDP"));
						return;
					}
				}
			}
			else if (cs.Equals(CSTR("NameID")))
			{
				sb.ClearStr();
				reader.ReadNodeText(sb);
				saml->SetNameID(sb.ToCString());
			}
			else if (cs.Equals(CSTR("SessionIndex")))
			{
				sb.ClearStr();
				reader.ReadNodeText(sb);
				saml->AddSessionIndex(sb.ToCString());
			}
			else
			{
				reader.SkipElement();
			}
		}
	}
	else
	{
		saml->SetError(Net::SAMLLogoutRequest::ProcessError::MessageInvalid);
		saml->SetErrorMessage(CSTR("XML element is not LogoutRequest"));
	}
}

Net::SAMLHandler::SAMLHandler(NN<SAMLConfig> cfg, Optional<Net::SSLEngine> ssl)
{
	Text::CStringNN nns;
	this->ssl = ssl;
	this->serverHost = 0;
	this->metadataPath = 0;
	this->loginPath = 0;
	this->logoutPath = 0;
	this->ssoPath = 0;
	this->signCert = 0;
	this->signKey = 0;
	this->idp = 0;
	this->hashType = Crypto::Hash::HashType::SHA1;
	this->nAuthMethods = 5;
	this->authMethods = MemAllocArr(SAMLAuthMethod, 5);
	this->authMethods[0] = Net::SAMLAuthMethod::Unknown;
	this->authMethods[1] = Net::SAMLAuthMethod::Password;
	this->authMethods[2] = Net::SAMLAuthMethod::PasswordProtectedTransport;
	this->authMethods[3] = Net::SAMLAuthMethod::Kerberos;
	this->authMethods[4] = Net::SAMLAuthMethod::WindowsAuth;
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

Net::SAMLHandler::~SAMLHandler()
{
	OPTSTR_DEL(this->serverHost);
	OPTSTR_DEL(this->metadataPath);
	OPTSTR_DEL(this->loginPath);
	OPTSTR_DEL(this->logoutPath);
	OPTSTR_DEL(this->ssoPath);
	MemFreeArr(this->authMethods);
	this->signCert.Delete();
	this->signKey.Delete();
}

Net::SAMLInitError Net::SAMLHandler::GetInitError()
{
	return this->initErr;
}

Bool Net::SAMLHandler::GetLoginURL(NN<Text::StringBuilderUTF8> sb)
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

Bool Net::SAMLHandler::GetLogoutURL(NN<Text::StringBuilderUTF8> sb)
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

Bool Net::SAMLHandler::GetMetadataURL(NN<Text::StringBuilderUTF8> sb)
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

Bool Net::SAMLHandler::GetSSOURL(NN<Text::StringBuilderUTF8> sb)
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

Optional<Crypto::Cert::X509PrivKey> Net::SAMLHandler::GetKey()
{
	return this->signKey;
}

void Net::SAMLHandler::SetIdp(NN<Net::SAMLIdpConfig> idp)
{
	Sync::MutexUsage mutUsage(this->idpMut);
	this->idp = idp;
}

void Net::SAMLHandler::SetHashType(Crypto::Hash::HashType hashType)
{
	this->hashType = hashType;
}

void Net::SAMLHandler::SetAuthMethods(UnsafeArray<SAMLAuthMethod> authMethods, UOSInt nAuthMethods)
{
	if (this->nAuthMethods < nAuthMethods)
	{
		this->nAuthMethods = nAuthMethods;
		MemFreeArr(this->authMethods);
		this->authMethods = MemAllocArr(SAMLAuthMethod, this->nAuthMethods);
	}
	else
	{
		this->nAuthMethods = nAuthMethods;
	}
	MemCopyNO(this->authMethods.Ptr(), authMethods.Ptr(), sizeof(SAMLAuthMethod) * nAuthMethods);
}

Bool Net::SAMLHandler::GetLoginMessageURL(NN<Text::StringBuilderUTF8> sbOut)
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
		if (this->nAuthMethods == 1)
		{
			sb.Append(CSTR("<samlp:RequestedAuthnContext Comparison=\"exact\">"));
			sb.Append(CSTR("<saml:AuthnContextClassRef>"));
			sb.Append(Net::SAMLAuthMethodGetString(this->authMethods[0]));
			sb.Append(CSTR("</saml:AuthnContextClassRef>"));
			sb.Append(CSTR("</samlp:RequestedAuthnContext>"));
		}
		else
		{
			sb.Append(CSTR("<samlp:RequestedAuthnContext Comparison=\"minimum\">"));
			UOSInt i = 0;
			while (i < this->nAuthMethods)
			{
				sb.Append(CSTR("<saml:AuthnContextClassRef>"));
				sb.Append(Net::SAMLAuthMethodGetString(this->authMethods[i]));
				sb.Append(CSTR("</saml:AuthnContextClassRef>"));
				i++;
			}
			sb.Append(CSTR("</samlp:RequestedAuthnContext>"));
		}
		sb.Append(CSTR("</samlp:AuthnRequest>"));
		return BuildRedirectURL(sbOut, idp->GetSignOnLocation()->ToCString(), sb.ToCString(), this->hashType, false);
	}
	else
	{
		return false;
	}
}

Bool Net::SAMLHandler::GetLogoutMessageURL(NN<Text::StringBuilderUTF8> sbOut, Text::CString nameID, Text::CString sessionId)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> metadataPath;
	NN<Text::String> serverHost;
	NN<Net::SAMLIdpConfig> idp;
	Text::CStringNN s;
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
		if (nameID.SetTo(s) && s.leng > 0)
		{
			sb.Append(CSTR("<saml:NameID>"));
			sb.Append(s);
			sb.Append(CSTR("</saml:NameID>"));
		}
		else
		{
			sb.Append(CSTR("<saml:NameID/>"));
		}
		if (sessionId.SetTo(s))
		{
			sb.Append(CSTR("<samlp:SessionIndex>"));
			sb.Append(s);
			sb.Append(CSTR("</samlp:SessionIndex>"));
		}
		sb.Append(CSTR("</samlp:LogoutRequest>"));

		return BuildRedirectURL(sbOut, idp->GetLogoutLocation()->ToCString(), sb.ToCString(), this->hashType, false);
	}
	else
	{
		return false;
	}
}

Bool Net::SAMLHandler::GetMetadataXML(NN<Text::StringBuilderUTF8> sb)
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
		sb->AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
		sb->AppendC(UTF8STRC("<md:EntityDescriptor xmlns:md=\"urn:oasis:names:tc:SAML:2.0:metadata\" ID=\""));
		sptr = Text::StrConcatC(sbuff, UTF8STRC("https://"));
		sptr = serverHost->ConcatTo(sptr);
		sptr = metadataPath->ConcatTo(sptr);
		Text::StrReplace(sbuff, ':', '_');
		Text::StrReplace(sbuff, '/', '_');
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb->AppendC(UTF8STRC("\" entityID=\""));
		sptr = Text::StrConcatC(sbuff, UTF8STRC("https://"));
		sptr = serverHost->ConcatTo(sptr);
		sptr = metadataPath->ConcatTo(sptr);
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb->AppendC(UTF8STRC("\">"));
		sb->AppendC(UTF8STRC("<md:SPSSODescriptor AuthnRequestsSigned=\"true\" WantAssertionsSigned=\"true\" protocolSupportEnumeration=\"urn:oasis:names:tc:SAML:2.0:protocol\">"));
		sb->AppendC(UTF8STRC("<md:KeyDescriptor use=\"signing\">"));
		sb->AppendC(UTF8STRC("<ds:KeyInfo xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\">"));
		sb->AppendC(UTF8STRC("<ds:X509Data>"));
		sb->AppendC(UTF8STRC("<ds:X509Certificate>"));
		b64.EncodeBin(sb, cert->GetASN1Buff(), cert->GetASN1BuffSize(), Text::LineBreakType::CRLF, 76);
		sb->AppendC(UTF8STRC("</ds:X509Certificate>"));
		sb->AppendC(UTF8STRC("</ds:X509Data>"));
		sb->AppendC(UTF8STRC("</ds:KeyInfo>"));
		sb->AppendC(UTF8STRC("</md:KeyDescriptor>"));
		sb->AppendC(UTF8STRC("<md:KeyDescriptor use=\"encryption\">"));
		sb->AppendC(UTF8STRC("<ds:KeyInfo xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\">"));
		sb->AppendC(UTF8STRC("<ds:X509Data>"));
		sb->AppendC(UTF8STRC("<ds:X509Certificate>"));
		b64.EncodeBin(sb, cert->GetASN1Buff(), cert->GetASN1BuffSize(), Text::LineBreakType::CRLF, 76);
		sb->AppendC(UTF8STRC("</ds:X509Certificate>"));
		sb->AppendC(UTF8STRC("</ds:X509Data>"));
		sb->AppendC(UTF8STRC("</ds:KeyInfo>"));
		sb->AppendC(UTF8STRC("</md:KeyDescriptor>"));
		sb->AppendC(UTF8STRC("<md:SingleLogoutService Binding=\"urn:oasis:names:tc:SAML:2.0:bindings:HTTP-Redirect\" Location=\"https://"));
		sb->Append(serverHost);
		sb->Append(logoutPath);
		sb->AppendC(UTF8STRC("\"/>"));
		sb->AppendC(UTF8STRC("<md:SingleLogoutService Binding=\"urn:oasis:names:tc:SAML:2.0:bindings:HTTP-POST\" Location=\"https://"));
		sb->Append(serverHost);
		sb->Append(logoutPath);
		sb->AppendC(UTF8STRC("\"/>"));
		sb->AppendC(UTF8STRC("<md:NameIDFormat>"));
		sb->AppendC(UTF8STRC("urn:oasis:names:tc:SAML:1.1:nameid-format:emailAddress"));
		sb->AppendC(UTF8STRC("</md:NameIDFormat>"));
		sb->AppendC(UTF8STRC("<md:NameIDFormat>"));
		sb->AppendC(UTF8STRC("urn:oasis:names:tc:SAML:2.0:nameid-format:transient"));
		sb->AppendC(UTF8STRC("</md:NameIDFormat>"));
		sb->AppendC(UTF8STRC("<md:NameIDFormat>"));
		sb->AppendC(UTF8STRC("urn:oasis:names:tc:SAML:2.0:nameid-format:persistent"));
		sb->AppendC(UTF8STRC("</md:NameIDFormat>"));
		sb->AppendC(UTF8STRC("<md:NameIDFormat>"));
		sb->AppendC(UTF8STRC("urn:oasis:names:tc:SAML:1.1:nameid-format:unspecified"));
		sb->AppendC(UTF8STRC("</md:NameIDFormat>"));
		sb->AppendC(UTF8STRC("<md:NameIDFormat>"));
		sb->AppendC(UTF8STRC("urn:oasis:names:tc:SAML:1.1:nameid-format:X509SubjectName"));
		sb->AppendC(UTF8STRC("</md:NameIDFormat>"));
		sb->AppendC(UTF8STRC("<md:AssertionConsumerService Binding=\"urn:oasis:names:tc:SAML:2.0:bindings:HTTP-POST\" Location=\"https://"));
		sb->Append(serverHost);
		sb->Append(ssoPath);
		sb->AppendC(UTF8STRC("\" index=\"0\" isDefault=\"true\"/>"));
		sb->AppendC(UTF8STRC("<md:AssertionConsumerService Binding=\"urn:oasis:names:tc:SAML:2.0:bindings:HTTP-Artifact\" Location=\"https://"));
		sb->Append(serverHost);
		sb->Append(ssoPath);
		sb->AppendC(UTF8STRC("\" index=\"1\"/>"));
		sb->AppendC(UTF8STRC("</md:SPSSODescriptor>"));
		sb->AppendC(UTF8STRC("</md:EntityDescriptor>"));
		return true;
	}
	return false;
}

Bool Net::SAMLHandler::GetLogoutResponse(NN<Text::StringBuilderUTF8> sb, Text::CStringNN id, SAMLStatusCode status)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> metadataPath;
	NN<Text::String> serverHost;
	NN<SAMLIdpConfig> idp;
	if (this->serverHost.SetTo(serverHost) && this->metadataPath.SetTo(metadataPath) && this->idp.SetTo(idp))
	{
		Data::Timestamp currTime = Data::Timestamp::UtcNow();
		sb->Append(CSTR("<samlp:LogoutResponse"));
		sb->Append(CSTR(" ID=\"SAML_"));
		sb->AppendI64(currTime.GetLocalSecs());
		sb->AppendU32(currTime.inst.nanosec);
		sb->AppendUTF8Char('"');
		sb->Append(CSTR(" Version=\"2.0\""));
		sb->Append(CSTR(" IssueInstant=\""));
		sptr = Data::Timestamp(Data::TimeInstant(currTime.inst.sec, 0), 0).ToStringISO8601(sbuff);
		sb->AppendP(sbuff, sptr);
		sb->AppendUTF8Char('"');
		sb->Append(CSTR(" InResponseTo=\""));
		sb->Append(id);
		sb->AppendUTF8Char('"');
		sb->Append(CSTR(" Destination=\""));
		sb->Append(idp->GetLogoutLocation());
		sb->AppendUTF8Char('"');
		sb->Append(CSTR(" xmlns:samlp=\"urn:oasis:names:tc:SAML:2.0:protocol\" xmlns=\"urn:oasis:names:tc:SAML:2.0:assertion\">"));
		sb->Append(CSTR("<Issuer>https://"));
		sb->Append(serverHost);
		sb->Append(metadataPath);
		sb->Append(CSTR("</Issuer>"));
		sb->Append(CSTR("<samlp:Status>"));
    	sb->Append(CSTR("<samlp:StatusCode Value=\""));
		sb->Append(SAMLStatusCodeGetString(status));
		sb->Append(CSTR("\" />"));
		sb->Append(CSTR("</samlp:Status>"));
		sb->Append(CSTR("</samlp:LogoutResponse>"));
		return true;
	}
	return false;
}

Bool Net::SAMLHandler::DoLoginGet(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::StringBuilderUTF8 sb;
	if (this->GetLoginMessageURL(sb))
	{
		return resp->RedirectURL(req, sb.ToCString(), 0);
	}
	else
	{
		return resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
}

Bool Net::SAMLHandler::DoLogoutGet(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CString nameID, Text::CString sessionId)
{
	if (req->GetQueryValue(CSTR("SAMLResponse")).NotNull())
	{
		NN<Net::SAMLLogoutResponse> msg = this->DoLogoutResp(req, resp);
		NN<Text::String> s;
		Text::StringBuilderUTF8 sb;
		Text::StringBuilderUTF8 sb2;
		sb.ClearStr();
		sb.AppendC(UTF8STRC("<html><head><title>Logout Response</title></head><body>"));
		sb.AppendC(UTF8STRC("<h1>Result</h1>"));
		sb.AppendC(UTF8STRC("<font color=\"red\">Error:</font> "));
		sb.Append(Net::SAMLLogoutResponse::ProcessErrorGetName(msg->GetError()));
		sb.Append(CSTR("<br/>"));
		sb.AppendC(UTF8STRC("<font color=\"red\">Error Message:</font> "));
		sb.Append(msg->GetErrorMessage());
		sb.Append(CSTR("<br/>"));
		sb.Append(CSTR("<font color=\"red\">Status:</font> "));
		sb.Append(Net::SAMLStatusCodeGetName(msg->GetStatus()));
		sb.Append(CSTR("<br/>"));
		if (msg->GetRawResponse().SetTo(s))
		{
			sb.AppendC(UTF8STRC("<hr/>"));
			sb.AppendC(UTF8STRC("<h1>RAW Response</h1>"));
			IO::MemoryReadingStream mstm(s->ToByteArray());
			Text::XMLReader::XMLWellFormat(this->encFact, mstm, 0, sb2);
			s = Text::XML::ToNewHTMLTextXMLColor(sb2.v);
			sb.Append(s);
			s->Release();
		}
		sb.AppendC(UTF8STRC("</body></html>"));
		msg.Delete();
		resp->AddDefHeaders(req);
		resp->AddCacheControl(0);
		resp->AddContentType(CSTR("text/html"));
		return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("text/html"), sb.ToCString());
	}
	else if (req->GetQueryValue(CSTR("SAMLRequest")).NotNull())
	{
		NN<SAMLLogoutRequest> msg = this->DoLogoutReq(req, resp);
		NN<Text::String> s;
		NN<SAMLIdpConfig> idp;
		if (msg->GetError() == SAMLLogoutRequest::ProcessError::Success && msg->GetID().SetTo(s) && this->idp.SetTo(idp))
		{
			Text::StringBuilderUTF8 sb;
			this->GetLogoutResponse(sb, s->ToCString(), SAMLStatusCode::Success);
			msg.Delete();
			this->SendRedirect(req, resp, idp->GetLogoutLocation()->ToCString(), sb.ToCString(), this->hashType, true);
			return true;
		}
		else
		{
			NN<Text::String> s;
			Text::StringBuilderUTF8 sb;
			Text::StringBuilderUTF8 sb2;
			sb.ClearStr();
			sb.AppendC(UTF8STRC("<html><head><title>Logout Response</title></head><body>"));
			sb.AppendC(UTF8STRC("<h1>Result</h1>"));
			sb.AppendC(UTF8STRC("<font color=\"red\">Error:</font> "));
			sb.Append(Net::SAMLLogoutRequest::ProcessErrorGetName(msg->GetError()));
			sb.Append(CSTR("<br/>"));
			sb.AppendC(UTF8STRC("<font color=\"red\">Error Message:</font> "));
			sb.Append(msg->GetErrorMessage());
			sb.Append(CSTR("<br/>"));
			sb.Append(CSTR("<font color=\"red\">ID:</font> "));
			sb.AppendOpt(msg->GetID());
			sb.Append(CSTR("<br/>"));
			sb.Append(CSTR("<font color=\"red\">NameID:</font> "));
			sb.AppendOpt(msg->GetNameID());
			sb.Append(CSTR("<br/>"));
			sb.Append(CSTR("<font color=\"red\">SessionIndex:</font> "));
			NN<const Data::ArrayListStringNN> sessionIndex = msg->GetSessionIndex();
			UOSInt i = 0;
			UOSInt j = sessionIndex->GetCount();
			while (i < j)
			{
				if (i > 0) sb.Append(CSTR("<br/>"));
				sb.Append(sessionIndex->GetItemNoCheck(i));
				i++;
			}
			sb.Append(CSTR("<br/>"));
			if (msg->GetRawResponse().SetTo(s))
			{
				sb.AppendC(UTF8STRC("<hr/>"));
				sb.AppendC(UTF8STRC("<h1>RAW Response</h1>"));
				IO::MemoryReadingStream mstm(s->ToByteArray());
				Text::XMLReader::XMLWellFormat(this->encFact, mstm, 0, sb2);
				s = Text::XML::ToNewHTMLTextXMLColor(sb2.v);
				sb.Append(s);
				s->Release();
			}
			sb.AppendC(UTF8STRC("</body></html>"));
			msg.Delete();
			resp->AddDefHeaders(req);
			resp->AddCacheControl(0);
			resp->AddContentType(CSTR("text/html"));
			return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("text/html"), sb.ToCString());
		}
	}
	Text::StringBuilderUTF8 sb;
	if (this->GetLogoutMessageURL(sb, nameID, sessionId))
	{
		return resp->RedirectURL(req, sb.ToCString(), 0);
	}
	else
	{
		return resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
}

Bool Net::SAMLHandler::DoMetadataGet(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	Text::StringBuilderUTF8 sb;
	if (this->GetMetadataXML(sb))
	{
		resp->AddDefHeaders(req);
		resp->AddCacheControl(0);
		resp->AddContentType(CSTR("application/samlmetadata+xml"));
		return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/samlmetadata+xml"), sb.GetLength(), sb.ToString());
	}
	return false;
}

NN<Net::SAMLSSOResponse> Net::SAMLHandler::DoSSOPost(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	NN<Text::XMLAttrib> attr;
	NN<Text::String> s;
	NN<SAMLSSOResponse> saml;
	NN<Net::SAMLIdpConfig> idp;
	NN<Net::SSLEngine> ssl;
	Data::Timestamp ts;
	req->ParseHTTPForm();
	Sync::MutexUsage mutUsage(this->idpMut);
	if (!this->idp.SetTo(idp))
	{
		NEW_CLASSNN(saml, SAMLSSOResponse(SAMLSSOResponse::ResponseError::IDPMissing, CSTR("Idp Config missing")));
		return saml;
	}
	else if (!this->ssl.SetTo(ssl))
	{
		NEW_CLASSNN(saml, SAMLSSOResponse(SAMLSSOResponse::ResponseError::IDPMissing, CSTR("SSL Engine missing")));
		return saml;
	}
	else if (req->GetQueryValue(CSTR("SAMLResponse")).SetTo(s) || req->GetHTTPFormStr(CSTR("SAMLResponse")).SetTo(s))
	{
		Text::TextBinEnc::Base64Enc b64;
		UnsafeArray<UInt8> buff = MemAllocArr(UInt8, s->leng);
		UOSInt buffSize = b64.DecodeBin(s->ToCString(), buff);
		buff[buffSize] = 0;
		NN<Crypto::Cert::X509Key> key;
		NN<Crypto::Cert::X509PrivKey> privKey;
		if (!this->signKey.SetTo(privKey))
		{
			NEW_CLASSNN(saml, SAMLSSOResponse(SAMLSSOResponse::ResponseError::SignKeyMissing, CSTR("Sign Key not exists")));
			saml->SetRawResponse(Text::CStringNN(buff, buffSize));
			MemFreeArr(buff);
			return saml;
		}
		if (!privKey->CreateKey().SetTo(key))
		{
			NEW_CLASSNN(saml, SAMLSSOResponse(SAMLSSOResponse::ResponseError::SignKeyInvalid, CSTR("Sign Key is not Private Key")));
			saml->SetRawResponse(Text::CStringNN(buff, buffSize));
			MemFreeArr(buff);
			return saml;
		}
		Text::StringBuilderUTF8 sb;
		Text::StringBuilderUTF8 sbTmp;
		SAMLStatusCode status;
		if (Net::SAMLUtil::DecryptResponse(ssl, this->encFact, key, Text::CStringNN(buff, buffSize), sb, status))
		{
			key.Delete();
			NEW_CLASSNN(saml, SAMLSSOResponse(SAMLSSOResponse::ResponseError::Success, CSTR("Decrypted")));
			saml->SetRawResponse(Text::CStringNN(buff, buffSize));
			saml->SetDecResponse(sb.ToCString());
			saml->SetStatusCode(status);
			MemFreeArr(buff);
			IO::MemoryReadingStream mstm(sb.ToByteArray());
			Text::XMLReader reader(this->encFact, mstm, Text::XMLReader::PM_XML);
			if (reader.NextElementName().SetTo(s) && s->Equals(CSTR("Assertion")))
			{
				i = reader.GetAttribCount();
				while (i-- > 0)
				{
					attr = reader.GetAttribNoCheck(i);
					if (attr->name.SetTo(s) && s->Equals(CSTR("ID")))
					{
						saml->SetId(OPTSTR_CSTR(attr->value));
					}
				}
				while (reader.NextElementName().SetTo(s))
				{
					if (s->Equals(CSTR("Issuer")))
					{
						sbTmp.ClearStr();
						reader.ReadNodeText(sbTmp);
						saml->SetIssuer(sbTmp.ToCString());
					}
					else if (s->Equals(CSTR("Conditions")))
					{
						i = reader.GetAttribCount();
						while (i-- > 0)
						{
							attr = reader.GetAttribNoCheck(i);
							if (attr->name.SetTo(s) && s->Equals(CSTR("NotBefore")))
							{
								if (attr->value.SetTo(s) && !(ts = Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr())).IsNull())
								{
									saml->SetNotBefore(ts);
								}
								else
								{
									saml->SetError(SAMLSSOResponse::ResponseError::ResponseFormatError);
									sbTmp.ClearStr();
									sbTmp.Append(CSTR("NotBefore cannot be parsed into Timestamp: "))->AppendOpt(s);
									saml->SetErrorMessage(sbTmp.ToCString());
								}
							}
							else if (attr->name.SetTo(s) && s->Equals(CSTR("NotOnOrAfter")))
							{
								if (attr->value.SetTo(s) && !(ts = Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr())).IsNull())
								{
									saml->SetNotOnOrAfter(ts);
								}
								else
								{
									saml->SetError(SAMLSSOResponse::ResponseError::ResponseFormatError);
									sbTmp.ClearStr();
									sbTmp.Append(CSTR("NotOnOrAfter cannot be parsed into Timestamp: "))->AppendOpt(s);
									saml->SetErrorMessage(sbTmp.ToCString());
								}
							}
						}
						while (reader.NextElementName().SetTo(s))
						{
							if (s->Equals(CSTR("AudienceRestriction")))
							{
								while (reader.NextElementName().SetTo(s))
								{
									if (s->Equals(CSTR("Audience")))
									{
										sbTmp.ClearStr();
										reader.ReadNodeText(sbTmp);
										saml->SetAudience(sbTmp.ToCString());
									}
									else
									{
										reader.SkipElement();
									}
								}
							}
							else
							{
								reader.SkipElement();
							}
						}
					}
					else if (s->Equals(CSTR("AttributeStatement")))
					{
						while (reader.NextElementName().SetTo(s))
						{
							if (s->Equals(CSTR("Attribute")))
							{
								Optional<Text::String> attrName = 0;
								i = reader.GetAttribCount();
								while (i-- > 0)
								{
									attr = reader.GetAttribNoCheck(i);
									if (attr->name.SetTo(s) && s->Equals(CSTR("Name")))
									{
										OPTSTR_DEL(attrName);
										attrName = Text::String::CopyOrNull(attr->value);
									}
								}
								while (reader.NextElementName().SetTo(s))
								{
									if (s->Equals(CSTR("AttributeValue")))
									{
										sbTmp.ClearStr();
										reader.ReadNodeText(sbTmp);
										if (attrName.SetTo(s))
										{
											if (s->Equals(CSTR("http://schemas.xmlsoap.org/ws/2005/05/identity/claims/name")))
											{
												saml->SetName(sbTmp.ToCString());
											}
											else if (s->Equals(CSTR("http://schemas.xmlsoap.org/ws/2005/05/identity/claims/emailaddress")))
											{
												saml->SetEmailAddress(sbTmp.ToCString());
											}
											else if (s->Equals(CSTR("http://schemas.xmlsoap.org/claims/EmailAddress")))
											{
												saml->SetEmailAddress(sbTmp.ToCString());
											}
											else if (s->Equals(CSTR("http://schemas.xmlsoap.org/ws/2005/05/identity/claims/givenname")))
											{
												saml->SetGivenname(sbTmp.ToCString());
											}
											else if (s->Equals(CSTR("http://schemas.xmlsoap.org/ws/2005/05/identity/claims/surname")))
											{
												saml->SetSurname(sbTmp.ToCString());
											}
											else if (s->Equals(CSTR("http://schemas.xmlsoap.org/claims/Group")))
											{
												saml->AddGroup(sbTmp.ToCString());
											}
										}
									}
									else
									{
										reader.SkipElement();
									}
								}
								OPTSTR_DEL(attrName);
							}
							else
							{
								reader.SkipElement();
							}
						}
					}
					else if (s->Equals(CSTR("Subject")))
					{
						while (reader.NextElementName().SetTo(s))
						{
							if (s->Equals(CSTR("NameID")))
							{
								sbTmp.ClearStr();
								reader.ReadNodeText(sbTmp);
								saml->SetNameID(sbTmp.ToCString());
							}
							else
							{
								reader.SkipElement();
							}
						}
					}
					else if (s->Equals(CSTR("AuthnStatement")))
					{
						i = reader.GetAttribCount();
						while (i-- > 0)
						{
							attr = reader.GetAttribNoCheck(i);
							if (attr->name.SetTo(s) && s->Equals(CSTR("SessionIndex")))
							{
								saml->SetSessionIndex(OPTSTR_CSTR(attr->value));
							}
						}
						reader.SkipElement();
					}
					else
					{
						reader.SkipElement();
					}
				}
			}
			if (saml->GetError() == SAMLSSOResponse::ResponseError::Success && saml->GetIssuer().SetTo(s) && !idp->GetEntityId()->Equals(s))
			{
				saml->SetError(SAMLSSOResponse::ResponseError::UnexpectedIssuer);
				saml->SetErrorMessage(CSTR("Unexpected Issuer"));
			}
			if (saml->GetError() == SAMLSSOResponse::ResponseError::Success && saml->GetNotBefore().NotNull())
			{
				Data::Timestamp currTime = Data::Timestamp::Now();
				if (currTime < saml->GetNotBefore())
				{
					saml->SetError(SAMLSSOResponse::ResponseError::TimeOutOfRange);
					sbTmp.ClearStr();
					sbTmp.Append(CSTR("Current time cannot be before "));
					sptr = saml->GetNotBefore().ToString(sbuff);
					sbTmp.AppendP(sbuff, sptr);
					saml->SetErrorMessage(sbTmp.ToCString());
				}
			}
			if (saml->GetError() == SAMLSSOResponse::ResponseError::Success && saml->GetNotOnOrAfter().NotNull())
			{
				Data::Timestamp currTime = Data::Timestamp::Now();
				if (currTime >= saml->GetNotOnOrAfter())
				{
					saml->SetError(SAMLSSOResponse::ResponseError::TimeOutOfRange);
					sbTmp.ClearStr();
					sbTmp.Append(CSTR("Current time cannot be on or after "));
					sptr = saml->GetNotOnOrAfter().ToString(sbuff);
					sbTmp.AppendP(sbuff, sptr);
					saml->SetErrorMessage(sbTmp.ToCString());
				}
			}
			if (saml->GetError() == SAMLSSOResponse::ResponseError::Success && saml->GetAudience().SetTo(s))
			{
				sbTmp.ClearStr();
				sbTmp.Append(CSTR("https://"));
				sbTmp.AppendOpt(this->serverHost);
				sbTmp.AppendOpt(this->metadataPath);
				if (!s->Equals(sbTmp.ToCString()))
				{
					saml->SetError(SAMLSSOResponse::ResponseError::InvalidAudience);
					saml->SetErrorMessage(CSTR("Invalid Audience"));
				}
			}
			if (saml->GetError() == SAMLSSOResponse::ResponseError::Success && saml->GetNameID().IsNull() && saml->GetName().IsNull())
			{
				saml->SetError(SAMLSSOResponse::ResponseError::UsernameMissing);
				saml->SetErrorMessage(CSTR("User name not found"));
			}
			return saml;
		}
		else
		{
			key.Delete();
			NEW_CLASSNN(saml, SAMLSSOResponse(SAMLSSOResponse::ResponseError::DecryptFailed, CSTR("Failed in decrypting response message")));
			saml->SetRawResponse(Text::CStringNN(buff, buffSize));
			saml->SetStatusCode(status);
			MemFreeArr(buff);
			return saml;
		}
	}
	else
	{
		NEW_CLASSNN(saml, SAMLSSOResponse(SAMLSSOResponse::ResponseError::ResponseNotFound, CSTR("SAMLResponse not found")));
		return saml;
	}
}

NN<Net::SAMLLogoutResponse> Net::SAMLHandler::DoLogoutResp(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	NN<Net::SAMLIdpConfig> idp;
	NN<Text::String> samlResponse;
	NN<Net::SSLEngine> ssl;
	NN<SAMLLogoutResponse> saml;

	Sync::MutexUsage mutUsage(this->idpMut);
	if (!this->idp.SetTo(idp))
	{
		NEW_CLASSNN(saml, SAMLLogoutResponse(SAMLLogoutResponse::ProcessError::IDPMissing, CSTR("Idp Config missing")));
		return saml;
	}
	else if (!this->ssl.SetTo(ssl))
	{
		NEW_CLASSNN(saml, SAMLLogoutResponse(SAMLLogoutResponse::ProcessError::SSLMissing, CSTR("SSL Engine missing")));
		return saml;
	}
	switch (VerifyHTTPRedirect(ssl, idp, req))
	{
	case SAMLSignError::CertMissing:
		NEW_CLASSNN(saml, SAMLLogoutResponse(SAMLLogoutResponse::ProcessError::IDPMissing, CSTR("Idp Config missing signature cert")));
		return saml;
	case SAMLSignError::SignatureMissing:
		NEW_CLASSNN(saml, SAMLLogoutResponse(SAMLLogoutResponse::ProcessError::ParamMissing, CSTR("Signature param missing")));
		return saml;
	case SAMLSignError::SigAlgMissing:
		NEW_CLASSNN(saml, SAMLLogoutResponse(SAMLLogoutResponse::ProcessError::ParamMissing, CSTR("SigAlg param missing")));
		return saml;
	case SAMLSignError::SigAlgNotSupported:
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(CSTR("SigAlg not supported: "));
		sb.AppendOpt(req->GetQueryValue(CSTR("SigAlg")));
		NEW_CLASSNN(saml, SAMLLogoutResponse(SAMLLogoutResponse::ProcessError::SigAlgNotSupported, sb.ToCString()));
		return saml;
	}
	case SAMLSignError::QueryStringGetError:
		NEW_CLASSNN(saml, SAMLLogoutResponse(SAMLLogoutResponse::ProcessError::QueryStringError, CSTR("Error in getting query string")));
		return saml;
	case SAMLSignError::QueryStringSearchError:
		NEW_CLASSNN(saml, SAMLLogoutResponse(SAMLLogoutResponse::ProcessError::QueryStringError, CSTR("Error in searching for payload end")));
		return saml;
	case SAMLSignError::KeyError:
		NEW_CLASSNN(saml, SAMLLogoutResponse(SAMLLogoutResponse::ProcessError::KeyError, CSTR("Error in extracting public key from cert")));
		return saml;
	case SAMLSignError::SignatureInvalid:
		NEW_CLASSNN(saml, SAMLLogoutResponse(SAMLLogoutResponse::ProcessError::SignatureInvalid, CSTR("Signature Invalid")));
		return saml;
	case SAMLSignError::Valid:
		break;
	}

	if (!req->GetQueryValue(CSTR("SAMLResponse")).SetTo(samlResponse))
	{
		NEW_CLASSNN(saml, SAMLLogoutResponse(SAMLLogoutResponse::ProcessError::ParamMissing, CSTR("SAMLResponse param missing")));
		return saml;
	}
	Text::TextBinEnc::Base64Enc b64;
	UnsafeArray<UInt8> dataBuff = MemAllocArr(UInt8, samlResponse->leng);
	UOSInt dataSize = b64.DecodeBin(samlResponse->ToCString(), dataBuff);
	IO::MemoryStream mstm;
	{
		Data::Compress::Inflater inflater(mstm, false);
		inflater.WriteCont(dataBuff, dataSize);
	}
	NEW_CLASSNN(saml, SAMLLogoutResponse(SAMLLogoutResponse::ProcessError::Success, CSTR("Decompressed")));
	saml->SetRawResponse(Text::CStringNN(mstm.GetBuff(), (UOSInt)mstm.GetLength()));

	mstm.SeekFromBeginning(0);
	this->ParseSAMLLogoutResponse(saml, mstm);
	MemFreeArr(dataBuff);
	if (saml->GetStatus() != Net::SAMLStatusCode::Success)
	{
		saml->SetError(Net::SAMLLogoutResponse::ProcessError::StatusError);
	}
	return saml;
}

NN<Net::SAMLLogoutRequest> Net::SAMLHandler::DoLogoutReq(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	NN<Net::SAMLIdpConfig> idp;
	NN<Text::String> samlRequest;
	NN<Net::SSLEngine> ssl;
	NN<SAMLLogoutRequest> saml;

	Sync::MutexUsage mutUsage(this->idpMut);
	if (!this->idp.SetTo(idp))
	{
		NEW_CLASSNN(saml, SAMLLogoutRequest(SAMLLogoutRequest::ProcessError::IDPMissing, CSTR("Idp Config missing")));
		return saml;
	}
	else if (!this->ssl.SetTo(ssl))
	{
		NEW_CLASSNN(saml, SAMLLogoutRequest(SAMLLogoutRequest::ProcessError::SSLMissing, CSTR("SSL Engine missing")));
		return saml;
	}
	switch (VerifyHTTPRedirect(ssl, idp, req))
	{
	case SAMLSignError::CertMissing:
		NEW_CLASSNN(saml, SAMLLogoutRequest(SAMLLogoutRequest::ProcessError::IDPMissing, CSTR("Idp Config missing signature cert")));
		return saml;
	case SAMLSignError::SignatureMissing:
		NEW_CLASSNN(saml, SAMLLogoutRequest(SAMLLogoutRequest::ProcessError::ParamMissing, CSTR("Signature param missing")));
		return saml;
	case SAMLSignError::SigAlgMissing:
		NEW_CLASSNN(saml, SAMLLogoutRequest(SAMLLogoutRequest::ProcessError::ParamMissing, CSTR("SigAlg param missing")));
		return saml;
	case SAMLSignError::SigAlgNotSupported:
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(CSTR("SigAlg not supported: "));
		sb.AppendOpt(req->GetQueryValue(CSTR("SigAlg")));
		NEW_CLASSNN(saml, SAMLLogoutRequest(SAMLLogoutRequest::ProcessError::SigAlgNotSupported, sb.ToCString()));
		return saml;
	}
	case SAMLSignError::QueryStringGetError:
		NEW_CLASSNN(saml, SAMLLogoutRequest(SAMLLogoutRequest::ProcessError::QueryStringError, CSTR("Error in getting query string")));
		return saml;
	case SAMLSignError::QueryStringSearchError:
		NEW_CLASSNN(saml, SAMLLogoutRequest(SAMLLogoutRequest::ProcessError::QueryStringError, CSTR("Error in searching for payload end")));
		return saml;
	case SAMLSignError::KeyError:
		NEW_CLASSNN(saml, SAMLLogoutRequest(SAMLLogoutRequest::ProcessError::KeyError, CSTR("Error in extracting public key from cert")));
		return saml;
	case SAMLSignError::SignatureInvalid:
		NEW_CLASSNN(saml, SAMLLogoutRequest(SAMLLogoutRequest::ProcessError::SignatureInvalid, CSTR("Signature Invalid")));
		return saml;
	case SAMLSignError::Valid:
		break;
	}

	if (!req->GetQueryValue(CSTR("SAMLRequest")).SetTo(samlRequest))
	{
		NEW_CLASSNN(saml, SAMLLogoutRequest(SAMLLogoutRequest::ProcessError::ParamMissing, CSTR("SAMLRequest param missing")));
		return saml;
	}
	Text::TextBinEnc::Base64Enc b64;
	UnsafeArray<UInt8> dataBuff = MemAllocArr(UInt8, samlRequest->leng);
	UOSInt dataSize = b64.DecodeBin(samlRequest->ToCString(), dataBuff);
	IO::MemoryStream mstm;
	{
		Data::Compress::Inflater inflater(mstm, false);
		inflater.WriteCont(dataBuff, dataSize);
	}
	NEW_CLASSNN(saml, SAMLLogoutRequest(SAMLLogoutRequest::ProcessError::Success, CSTR("Decompressed")));
	saml->SetRawResponse(Text::CStringNN(mstm.GetBuff(), (UOSInt)mstm.GetLength()));
	mstm.SeekFromBeginning(0);
	this->ParseSAMLLogoutRequest(saml, mstm);
	MemFreeArr(dataBuff);
	return saml;
}

Text::CStringNN Net::SAMLInitErrorGetName(SAMLInitError err)
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
