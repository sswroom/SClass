#include "Stdafx.h"
#include "Crypto/Token/JWSignature.h"
#include "Exporter/PEMExporter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Net/ACMEConn.h"
#include "Net/DefaultSSLEngine.h"
#include "Net/HTTPClient.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/JSON.h"
#include "Text/TextBinEnc/Base64Enc.h"

#include <stdio.h>
/*
https://datatracker.ietf.org/doc/html/rfc8555

Testing:
https://acme-staging-v02.api.letsencrypt.org/directory

Production:
https://acme-v02.api.letsencrypt.org/directory
*/

const UTF8Char *Net::ACMEConn::JWK(Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm *alg)
{
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	switch (key->GetKeyType())
	{
	case Crypto::Cert::X509Key::KeyType::RSA:
	case Crypto::Cert::X509Key::KeyType::RSAPublic:
		{
			UOSInt mSize;
			UOSInt eSize;
			const UInt8 *m = key->GetRSAModulus(&mSize);
			const UInt8 *e = key->GetRSAPublicExponent(&eSize);
			if (m == 0 || e == 0)
			{
				return 0;
			}
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"{\"kty\":\"RSA\",\"n\":\"");
			b64.EncodeBin(&sb, m, mSize);
			sb.Append((const UTF8Char*)"\",\"e\":\"");
			b64.EncodeBin(&sb, e, eSize);
			sb.Append((const UTF8Char*)"\"}");
			*alg = Crypto::Token::JWSignature::Algorithm::RS256;
			return Text::StrCopyNew(sb.ToString());
		}
	case Crypto::Cert::X509Key::KeyType::ECDSA:
		return 0;
	case Crypto::Cert::X509Key::KeyType::DSA:
	case Crypto::Cert::X509Key::KeyType::ED25519:
	case Crypto::Cert::X509Key::KeyType::Unknown:
	default:
		return 0;
	}
}

const UTF8Char *Net::ACMEConn::ProtectedJWK(const UTF8Char *nonce, const UTF8Char *url, Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm *alg)
{
	const UTF8Char *jwk = JWK(key, alg);
	if (jwk == 0)
	{
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"{\"alg\":\"");
	sb.Append(Crypto::Token::JWSignature::AlgorithmGetName(*alg));
	sb.Append((const UTF8Char*)"\",\"nonce\":\"");
	sb.Append(nonce);
	sb.Append((const UTF8Char*)"\",\"url\":\"");
	sb.Append(url);
	sb.Append((const UTF8Char*)"\",\"jwk\":");
	sb.Append(jwk);
	sb.Append((const UTF8Char*)"}");
	Text::StrDelNew(jwk);
	return Text::StrCopyNew(sb.ToString());
}

const UTF8Char *Net::ACMEConn::EncodeJWS(Net::SSLEngine *ssl, const UTF8Char *protStr, const UTF8Char *data, Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm alg)
{
	Text::StringBuilderUTF8 sb;
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	b64.EncodeBin(&sb, protStr, Text::StrCharCnt(protStr));
	sb.AppendChar('.', 1);
	b64.EncodeBin(&sb, data, Text::StrCharCnt(data));
	Crypto::Token::JWSignature *sign;
	NEW_CLASS(sign, Crypto::Token::JWSignature(ssl, alg, key->GetASN1Buff(), key->GetASN1BuffSize()));
	sign->CalcHash(sb.ToString(), sb.GetLength());

	sb.ClearStr();
	sb.Append((const UTF8Char*)"{\"protected\":\"");
	b64.EncodeBin(&sb, protStr, Text::StrCharCnt(protStr));
	sb.Append((const UTF8Char*)"\",\"payload\":\"");
	b64.EncodeBin(&sb, data, Text::StrCharCnt(data));
	sb.Append((const UTF8Char*)"\",\"signature\":\"");
	sign->GetHashB64(&sb);
	sb.Append((const UTF8Char*)"\"}");
	DEL_CLASS(sign);
	printf("Protected: %s\r\n", protStr);
	printf("Payload: %s\r\n", data);
	printf("JWS: %s\r\n", sb.ToString());
	return Text::StrCopyNew(sb.ToString());
}

Net::HTTPClient *Net::ACMEConn::ACMEPost(const UTF8Char *url, const Char *data)
{
	if (this->nonce == 0)
	{
		return 0;
	}
	const UTF8Char *protStr;
	Crypto::Token::JWSignature::Algorithm alg;
	protStr = ProtectedJWK(this->nonce, url, this->key, &alg);
	if (protStr == 0)
	{
		return 0;
	}
	const UTF8Char *jws;
	jws = EncodeJWS(ssl, protStr, (const UTF8Char*)data, this->key, alg);
	Text::StrDelNew(protStr);
	if (jws == 0)
	{
		return 0;
	}
	UOSInt jwsLen = Text::StrCharCnt(jws);
	Net::HTTPClient *cli = 0;
	cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, url, "POST", true);
	if (cli)
	{
		cli->AddContentType((const UTF8Char*)"application/jose+json");
		cli->AddContentLength(jwsLen);
		cli->Write(jws, jwsLen);
	}
	Text::StrDelNew(jws);

	Text::StringBuilderUTF8 sb;
	cli->GetRespStatus();
	if (cli->GetRespHeader((const UTF8Char*)"Replay-Nonce", &sb))
	{
		SDEL_TEXT(this->nonce);
		this->nonce = Text::StrCopyNew(sb.ToString());
	}
	return cli;
}

Net::ACMEConn::ACMEConn(Net::SocketFactory *sockf, const UTF8Char *serverHost, UInt16 port)
{
	UInt8 buff[2048];
	UOSInt recvSize;
	this->sockf = sockf;
	this->key = 0;
	this->ssl = Net::DefaultSSLEngine::Create(sockf, false);
	this->serverHost = Text::StrCopyNew(serverHost);
	this->port = port;
	this->urlNewNonce = 0;
	this->urlNewAccount = 0;
	this->urlNewOrder = 0;
	this->urlNewAuthz = 0;
	this->urlRevokeCert = 0;
	this->urlKeyChange = 0;
	this->urlTermOfService = 0;
	this->urlWebsite = 0;
	this->nonce = 0;
	this->accountId = 0;
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char *)"https://");
	sb.Append(serverHost);
	if (port != 0 && port != 443)
	{
		sb.AppendChar(':', 1);
		sb.AppendU16(port);
	}
	sb.Append((const UTF8Char*)"/directory");
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sb.ToString(), "GET", true);
	if (cli)
	{
		IO::MemoryStream *mstm;
		if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
		{
			NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"Net.ACEClient.mstm"));
			while (true)
			{
				recvSize = cli->Read(buff, 2048);
				if (recvSize <= 0)
				{
					break;
				}
				mstm->Write(buff, recvSize);
			}
			if (mstm->GetLength() > 32)
			{
				UInt8 *jsonBuff = mstm->GetBuff(&recvSize);
				const UTF8Char *csptr;
				Text::JSONBase *json = Text::JSONBase::ParseJSONStrLen(jsonBuff, recvSize);
				if (json)
				{
					if (json->GetType() == Text::JSONType::Object)
					{
						Text::JSONObject *o = (Text::JSONObject*)json;
						if ((csptr = o->GetObjectString((const UTF8Char*)"newNonce")) != 0)
						{
							this->urlNewNonce = Text::StrCopyNew(csptr);
						}
						if ((csptr = o->GetObjectString((const UTF8Char*)"newAccount")) != 0)
						{
							this->urlNewAccount = Text::StrCopyNew(csptr);
						}
						if ((csptr = o->GetObjectString((const UTF8Char*)"newOrder")) != 0)
						{
							this->urlNewOrder = Text::StrCopyNew(csptr);
						}
						if ((csptr = o->GetObjectString((const UTF8Char*)"newAuthz")) != 0)
						{
							this->urlNewAuthz = Text::StrCopyNew(csptr);
						}
						if ((csptr = o->GetObjectString((const UTF8Char*)"revokeCert")) != 0)
						{
							this->urlRevokeCert = Text::StrCopyNew(csptr);
						}
						if ((csptr = o->GetObjectString((const UTF8Char*)"keyChange")) != 0)
						{
							this->urlKeyChange = Text::StrCopyNew(csptr);
						}
						Text::JSONBase *metaBase = o->GetObjectValue((const UTF8Char*)"meta");
						if (metaBase && metaBase->GetType() == Text::JSONType::Object)
						{
							Text::JSONObject *metaObj = (Text::JSONObject*)metaBase;
							if ((csptr = metaObj->GetObjectString((const UTF8Char*)"termsOfService")) != 0)
							{
								this->urlTermOfService = Text::StrCopyNew(csptr);
							}
							if ((csptr = metaObj->GetObjectString((const UTF8Char*)"website")) != 0)
							{
								this->urlWebsite = Text::StrCopyNew(csptr);
							}
						}
					}
					json->EndUse();
				}
			}
			DEL_CLASS(mstm);
		}
		DEL_CLASS(cli);
	}
}

Net::ACMEConn::~ACMEConn()
{
	SDEL_CLASS(this->ssl);
	Text::StrDelNew(this->serverHost);
	SDEL_TEXT(this->urlNewNonce);
	SDEL_TEXT(this->urlNewAccount);
	SDEL_TEXT(this->urlNewOrder);
	SDEL_TEXT(this->urlNewAuthz);
	SDEL_TEXT(this->urlRevokeCert);
	SDEL_TEXT(this->urlKeyChange);
	SDEL_TEXT(this->urlTermOfService);
	SDEL_TEXT(this->urlWebsite);
	SDEL_TEXT(this->nonce);
	SDEL_TEXT(this->accountId);
	SDEL_CLASS(this->key);
}

Bool Net::ACMEConn::IsError()
{
	if (this->urlNewNonce == 0 ||
		this->urlNewAccount == 0 ||
		this->urlNewOrder == 0 ||
		this->urlRevokeCert == 0 ||
		this->urlKeyChange == 0)
	{
		return true;
	}
	return false;
}

const UTF8Char *Net::ACMEConn::GetTermOfService()
{
	return this->urlTermOfService;
}

const UTF8Char *Net::ACMEConn::GetWebsite()
{
	return this->urlWebsite;
}

const UTF8Char *Net::ACMEConn::GetAccountId()
{
	return this->accountId;
}

Bool Net::ACMEConn::NewNonce()
{
	if (this->urlNewNonce == 0)
	{
		return false;
	}
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, this->urlNewNonce, "GET", true);
	if (cli == 0)
	{
		return false;
	}
	Bool succ = false;
	if (cli->GetRespStatus() == Net::WebStatus::SC_NO_CONTENT)
	{
		Text::StringBuilderUTF8 sb;
		if (cli->GetRespHeader((const UTF8Char*)"Replay-Nonce", &sb))
		{
			SDEL_TEXT(this->nonce);
			this->nonce = Text::StrCopyNew(sb.ToString());
			succ = true;
		}
	}
	DEL_CLASS(cli);
	return succ;
}

Bool Net::ACMEConn::AccountNew()
{
	if (this->urlNewAccount == 0)
	{
		return false;
	}
	Net::HTTPClient *cli = this->ACMEPost(this->urlNewAccount, "{\"onlyReturnExisting\":true}");
	if (cli == 0)
	{
		return false;
	}
	Bool succ = false;
	if (cli->GetRespStatus() == Net::WebStatus::SC_BAD_REQUEST)
	{
		IO::MemoryStream mstm((const UTF8Char*)"Net.ACMEConn.AccountNew.mstm");
		cli->ReadToEnd(&mstm, 4096);
		DEL_CLASS(cli);
		UOSInt buffSize;
		UInt8 *buff = mstm.GetBuff(&buffSize);
		Text::JSONBase *base = Text::JSONBase::ParseJSONStrLen(buff, buffSize);
		if (base != 0)
		{
			if (base->GetType() == Text::JSONType::Object)
			{
				Text::JSONObject *o = (Text::JSONObject*)base;
				const UTF8Char *csptr = o->GetObjectString((const UTF8Char*)"type");
				if (csptr && Text::StrEquals(csptr, (const UTF8Char*)"urn:ietf:params:acme:error:accountDoesNotExist"))
				{
					Text::StringBuilderUTF8 sb;
					sb.Append((const UTF8Char*)"{\"termsOfServiceAgreed\":true");
					sb.AppendChar('}', 1);
					cli = this->ACMEPost(this->urlNewAccount, (const Char*)sb.ToString());
					if (cli)
					{
						mstm.Clear();
						cli->ReadToEnd(&mstm, 4096);
						sb.ClearStr();
						if (cli->GetRespStatus() == Net::WebStatus::SC_CREATED && cli->GetRespHeader((const UTF8Char*)"Location", &sb))
						{
							SDEL_TEXT(this->accountId);
							this->accountId = Text::StrCopyNew(sb.ToString());
							succ = true;
						}
						DEL_CLASS(cli);
					}
				}
			}
			base->EndUse();
		}
	}
	else
	{
		DEL_CLASS(cli);
		succ = false;
	}

	return succ;
}

Bool Net::ACMEConn::AccountRetr()
{
	if (this->urlNewAccount == 0)
	{
		return false;
	}
	Net::HTTPClient *cli = this->ACMEPost(this->urlNewAccount, "{\"onlyReturnExisting\":true}");
	if (cli == 0)
	{
		return false;
	}
	Bool succ = false;
	if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
	{
		Text::StringBuilderUTF8 sb;
		if (cli->GetRespHeader((const UTF8Char*)"Location", &sb))
		{
			SDEL_TEXT(this->accountId);
			this->accountId = Text::StrCopyNew(sb.ToString());
			succ = true;
		}
		DEL_CLASS(cli);
	}
	else
	{
		DEL_CLASS(cli);
		succ = false;
	}
	return succ;
}

Bool Net::ACMEConn::NewKey()
{
	Crypto::Cert::X509Key *key = this->ssl->GenerateRSAKey();
	if (key)
	{
		SDEL_CLASS(this->key);
		this->key = key;
		return true;
	}
	return false;
}

Bool Net::ACMEConn::SetKey(Crypto::Cert::X509Key *key)
{
	if (key && key->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSA)
	{
		SDEL_CLASS(this->key);
		this->key = (Crypto::Cert::X509Key*)key->Clone();
		return true;
	}
	return false;
}

Bool Net::ACMEConn::LoadKey(const UTF8Char *fileName)
{
	UInt8 keyPEM[4096];
	UOSInt keyPEMSize = IO::FileStream::LoadFile(fileName, keyPEM, 4096);
	if (keyPEMSize == 0)
	{
		return false;
	}
	Crypto::Cert::X509File *x509 = Parser::FileParser::X509Parser::ParseBuff(keyPEM, keyPEMSize, fileName);
	if (x509 == 0)
	{
		return false;
	}
	if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Key && ((Crypto::Cert::X509Key*)x509)->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSA)
	{
		SDEL_CLASS(this->key);
		this->key = (Crypto::Cert::X509Key*)x509;
		return true;
	}
	DEL_CLASS(x509);
	return false;
}

Bool Net::ACMEConn::SaveKey(const UTF8Char *fileName)
{
	if (this->key == 0)
	{
		return false;
	}
	return Exporter::PEMExporter::ExportFile(fileName, this->key);
}
