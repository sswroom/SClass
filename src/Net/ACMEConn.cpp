#include "Stdafx.h"
#include "Crypto/Hash/SHA256.h"
#include "Crypto/Token/JWSignature.h"
#include "Exporter/PEMExporter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Net/ACMEConn.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngineFactory.h"
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

Optional<Text::String> Net::ACMEConn::JWK(NN<Crypto::Cert::X509Key> key, OutParam<Crypto::Token::JWSignature::Algorithm> alg)
{
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	switch (key->GetKeyType())
	{
	case Crypto::Cert::X509Key::KeyType::RSA:
	case Crypto::Cert::X509Key::KeyType::RSAPublic:
		{
			UOSInt mSize;
			UOSInt eSize;
			UnsafeArray<const UInt8> m;
			UnsafeArray<const UInt8> e;
			if (!key->GetRSAModulus(mSize).SetTo(m) || !key->GetRSAPublicExponent(eSize).SetTo(e))
			{
				return 0;
			}
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("{\"e\":\""));
			b64.EncodeBin(sb, e.Ptr(), eSize);
			sb.AppendC(UTF8STRC("\",\"kty\":\"RSA\",\"n\":\""));
			b64.EncodeBin(sb, m.Ptr(), mSize);
			sb.AppendC(UTF8STRC("\"}"));
			alg.Set(Crypto::Token::JWSignature::Algorithm::RS256);
			return Text::String::New(sb.ToCString());
		}
	case Crypto::Cert::X509Key::KeyType::ECDSA:
	case Crypto::Cert::X509Key::KeyType::ECPublic:
		return 0;
	case Crypto::Cert::X509Key::KeyType::DSA:
	case Crypto::Cert::X509Key::KeyType::ED25519:
	case Crypto::Cert::X509Key::KeyType::Unknown:
	default:
		return 0;
	}
}

Text::String *Net::ACMEConn::ProtectedJWK(Text::String *nonce, NN<Text::String> url, NN<Crypto::Cert::X509Key> key, OutParam<Crypto::Token::JWSignature::Algorithm> alg, Text::String *accountId)
{
	NN<Text::String> jwk;
	Crypto::Token::JWSignature::Algorithm palg;
	if (!JWK(key, palg).SetTo(jwk))
	{
		return 0;
	}
	alg.Set(palg);
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("{\"alg\":\""));
	sb.Append(Crypto::Token::JWSignature::AlgorithmGetName(palg));
	sb.AppendC(UTF8STRC("\",\"nonce\":\""));
	sb.Append(nonce);
	sb.AppendC(UTF8STRC("\",\"url\":\""));
	sb.Append(url);
	if (accountId)
	{
		sb.AppendC(UTF8STRC("\",\"kid\":\""));
		sb.Append(accountId);
		sb.AppendUTF8Char('\"');
	}
	else
	{
		sb.AppendC(UTF8STRC("\",\"jwk\":"));
		sb.Append(jwk);
	}
	jwk->Release();
	sb.AppendC(UTF8STRC("}"));
	return Text::String::New(sb.ToCString()).Ptr();
}

NN<Text::String> Net::ACMEConn::EncodeJWS(Optional<Net::SSLEngine> ssl, Text::CStringNN protStr, Text::CStringNN data, NN<Crypto::Cert::X509Key> key, Crypto::Token::JWSignature::Algorithm alg)
{
	Text::StringBuilderUTF8 sb;
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	b64.EncodeBin(sb, protStr.v, protStr.leng);
	sb.AppendUTF8Char('.');
	b64.EncodeBin(sb, data.v, data.leng);
	Crypto::Token::JWSignature *sign;
	NEW_CLASS(sign, Crypto::Token::JWSignature(ssl, alg, key->GetASN1Buff(), key->GetASN1BuffSize(), key->GetKeyType()));
	sign->CalcHash(sb.ToString(), sb.GetLength());

	sb.ClearStr();
	sb.AppendC(UTF8STRC("{\"protected\":\""));
	b64.EncodeBin(sb, protStr.v, protStr.leng);
	sb.AppendC(UTF8STRC("\",\"payload\":\""));
	b64.EncodeBin(sb, data.v, data.leng);
	sb.AppendC(UTF8STRC("\",\"signature\":\""));
	sign->GetHashB64(sb);
	sb.AppendC(UTF8STRC("\"}"));
	DEL_CLASS(sign);
	printf("Protected: %s\r\n", protStr.v.Ptr());
	printf("Payload: %s\r\n", data.v.Ptr());
	printf("JWS: %s\r\n", sb.ToPtr());
	return Text::String::New(sb.ToString(), sb.GetLength());
}

Bool Net::ACMEConn::KeyHash(NN<Crypto::Cert::X509Key> key, NN<Text::StringBuilderUTF8> sb)
{
	Crypto::Token::JWSignature::Algorithm alg;
	NN<Text::String> jwk;
	if (!JWK(key, alg).SetTo(jwk))
	{
		return false;
	}
	Crypto::Hash::SHA256 sha256;
	UInt8 hashVal[32];
	sha256.Calc(jwk->v, jwk->leng);
	sha256.GetValue(hashVal);
	jwk->Release();
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	b64.EncodeBin(sb, hashVal, 32);
	return true;
}

Net::HTTPClient *Net::ACMEConn::ACMEPost(NN<Text::String> url, Text::CStringNN data)
{
	NN<Crypto::Cert::X509Key> key;
	if (this->nonce == 0 || !this->key.SetTo(key))
	{
		return 0;
	}
	Text::String *protStr;
	Crypto::Token::JWSignature::Algorithm alg;
	protStr = ProtectedJWK(this->nonce, url, key, alg, this->accountId);
	if (protStr == 0)
	{
		return 0;
	}
	NN<Text::String> jws;
	jws = EncodeJWS(ssl, protStr->ToCString(), data, key, alg);
	protStr->Release();
	UOSInt jwsLen = jws->leng;
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, url->ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, true);
	cli->AddContentType(CSTR("application/jose+json"));
	cli->AddContentLength(jwsLen);
	cli->Write(jws->v, jwsLen);
	jws->Release();

	Text::StringBuilderUTF8 sb;
	cli->GetRespStatus();
	if (cli->GetRespHeader(CSTR("Replay-Nonce"), sb))
	{
		SDEL_STRING(this->nonce);
		this->nonce = Text::String::New(sb.ToCString()).Ptr();
	}
	return cli.Ptr();
}

Net::ACMEConn::Order *Net::ACMEConn::OrderParse(const UInt8 *buff, UOSInt buffSize)
{
	Text::JSONBase *json = Text::JSONBase::ParseJSONBytes(buff, buffSize);
	if (json)
	{
		NN<Text::String> s;
		Order *order = 0;
		if (json->GetType() == Text::JSONType::Object)
		{
			Text::JSONObject *o = (Text::JSONObject*)json;
			order = MemAlloc(Order, 1);
			MemClear(order, sizeof(Order));
			order->status = ACMEStatusFromString(o->GetObjectString(CSTR("status")));
			if (o->GetObjectString(CSTR("expires")).SetTo(s))
			{
				Data::DateTime dt;
				dt.SetValue(s->ToCString());
				order->expires = dt.ToTicks();
			}
			order->finalizeURL = o->GetObjectNewString(CSTR("finalize"));
			Text::JSONBase *auth = o->GetObjectValue(CSTR("authorizations"));
			if (auth && auth->GetType() == Text::JSONType::Array)
			{
				Text::JSONArray *authArr = (Text::JSONArray*)auth;
				NEW_CLASS(order->authURLs, Data::ArrayListStringNN());
				UOSInt i = 0;
				UOSInt j = authArr->GetArrayLength();
				while (i < j)
				{
					auth = authArr->GetArrayValue(i);
					if (auth && auth->GetType() == Text::JSONType::String)
					{
						order->authURLs->Add(((Text::JSONString*)auth)->GetValue()->Clone());
					}
					i++;
				}
			}
		}
		json->EndUse();
		return order;
	}
	return 0;
}

Net::ACMEConn::Challenge *Net::ACMEConn::ChallengeJSON(Text::JSONBase *json)
{
	NN<Text::String> type;
	NN<Text::String> status;
	NN<Text::String> url;
	NN<Text::String> token;

	if (json->GetValueString(CSTR("type")).SetTo(type) &&
		json->GetValueString(CSTR("status")).SetTo(status) &&
		json->GetValueString(CSTR("url")).SetTo(url) &&
		json->GetValueString(CSTR("token")).SetTo(token))
	{
		Challenge *chall = MemAlloc(Challenge, 1);
		chall->status = ACMEStatusFromString(status);
		chall->type = AuthorizeTypeFromString(type);
		chall->url = url->Clone();
		chall->token = token->Clone();
		return chall;
	}
	return 0;
}

Net::ACMEConn::Challenge *Net::ACMEConn::ChallengeParse(const UInt8 *buff, UOSInt buffSize)
{
	Text::JSONBase *json = Text::JSONBase::ParseJSONBytes(buff, buffSize);
	Challenge *chall = 0;
	if (json)
	{
		chall = ChallengeJSON(json);
		json->EndUse();
	}
	return chall;
}

Net::ACMEConn::ACMEConn(NN<Net::SocketFactory> sockf, Text::CStringNN serverHost, UInt16 port)
{
	UInt8 buff[2048];
	UOSInt recvSize;
	this->sockf = sockf;
	this->key = 0;
	this->ssl = Net::SSLEngineFactory::Create(sockf, false);
	this->serverHost = Text::String::New(serverHost);
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
	sb.AppendC(UTF8STRC("https://"));
	sb.Append(this->serverHost);
	if (port != 0 && port != 443)
	{
		sb.AppendUTF8Char(':');
		sb.AppendU16(port);
	}
	sb.AppendC(UTF8STRC("/directory"));
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
	{
		IO::MemoryStream mstm;
		while (true)
		{
			recvSize = cli->Read(BYTEARR(buff));
			if (recvSize <= 0)
			{
				break;
			}
			mstm.Write(buff, recvSize);
		}
		if (mstm.GetLength() > 32)
		{
			UInt8 *jsonBuff = mstm.GetBuff(recvSize);
			NN<Text::String> s;
			Text::JSONBase *json = Text::JSONBase::ParseJSONBytes(jsonBuff, recvSize);
			if (json)
			{
				if (json->GetType() == Text::JSONType::Object)
				{
					Text::JSONObject *o = (Text::JSONObject*)json;
					if (o->GetObjectString(CSTR("newNonce")).SetTo(s))
					{
						this->urlNewNonce = s->Clone().Ptr();
					}
					if (o->GetObjectString(CSTR("newAccount")).SetTo(s))
					{
						this->urlNewAccount = s->Clone().Ptr();
					}
					if (o->GetObjectString(CSTR("newOrder")).SetTo(s))
					{
						this->urlNewOrder = s->Clone().Ptr();
					}
					if (o->GetObjectString(CSTR("newAuthz")).SetTo(s))
					{
						this->urlNewAuthz = s->Clone().Ptr();
					}
					if (o->GetObjectString(CSTR("revokeCert")).SetTo(s))
					{
						this->urlRevokeCert = s->Clone().Ptr();
					}
					if (o->GetObjectString(CSTR("keyChange")).SetTo(s))
					{
						this->urlKeyChange = s->Clone().Ptr();
					}
					Text::JSONBase *metaBase = o->GetObjectValue(CSTR("meta"));
					if (metaBase && metaBase->GetType() == Text::JSONType::Object)
					{
						Text::JSONObject *metaObj = (Text::JSONObject*)metaBase;
						if (metaObj->GetObjectString(CSTR("termsOfService")).SetTo(s))
						{
							this->urlTermOfService = s->Clone().Ptr();
						}
						if (metaObj->GetObjectString(CSTR("website")).SetTo(s))
						{
							this->urlWebsite = s->Clone().Ptr();
						}
					}
				}
				json->EndUse();
			}
		}
	}
	cli.Delete();
}

Net::ACMEConn::~ACMEConn()
{
	this->ssl.Delete();
	this->serverHost->Release();
	SDEL_STRING(this->urlNewNonce);
	SDEL_STRING(this->urlNewAccount);
	SDEL_STRING(this->urlNewOrder);
	SDEL_STRING(this->urlNewAuthz);
	SDEL_STRING(this->urlRevokeCert);
	SDEL_STRING(this->urlKeyChange);
	SDEL_STRING(this->urlTermOfService);
	SDEL_STRING(this->urlWebsite);
	SDEL_STRING(this->nonce);
	SDEL_STRING(this->accountId);
	this->key.Delete();
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

Text::String *Net::ACMEConn::GetTermOfService()
{
	return this->urlTermOfService;
}

Text::String *Net::ACMEConn::GetWebsite()
{
	return this->urlWebsite;
}

Text::String *Net::ACMEConn::GetAccountId()
{
	return this->accountId;
}

Bool Net::ACMEConn::NewNonce()
{
	if (this->urlNewNonce == 0)
	{
		return false;
	}
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, this->urlNewNonce->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli->IsError())
	{
		cli.Delete();
		return false;
	}
	Bool succ = false;
	if (cli->GetRespStatus() == Net::WebStatus::SC_NO_CONTENT)
	{
		Text::StringBuilderUTF8 sb;
		if (cli->GetRespHeader(CSTR("Replay-Nonce"), sb))
		{
			SDEL_STRING(this->nonce);
			this->nonce = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
			succ = true;
		}
	}
	cli.Delete();
	return succ;
}

Bool Net::ACMEConn::AccountNew()
{
	NN<Text::String> url;
	if (!url.Set(this->urlNewAccount))
	{
		return false;
	}
	Net::HTTPClient *cli = this->ACMEPost(url, CSTR("{\"onlyReturnExisting\":true}"));
	if (cli == 0)
	{
		return false;
	}
	Bool succ = false;
	if (cli->GetRespStatus() == Net::WebStatus::SC_BAD_REQUEST)
	{
		IO::MemoryStream mstm;
		cli->ReadToEnd(mstm, 4096);
		DEL_CLASS(cli);
		UOSInt buffSize;
		UInt8 *buff = mstm.GetBuff(buffSize);
		Text::JSONBase *base = Text::JSONBase::ParseJSONBytes(buff, buffSize);
		if (base != 0)
		{
			if (base->GetType() == Text::JSONType::Object)
			{
				Text::JSONObject *o = (Text::JSONObject*)base;
				NN<Text::String> s;
				if (o->GetObjectString(CSTR("type")).SetTo(s) && s->Equals(UTF8STRC("urn:ietf:params:acme:error:accountDoesNotExist")))
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("{\"termsOfServiceAgreed\":true"));
					sb.AppendUTF8Char('}');
					cli = this->ACMEPost(url, sb.ToCString());
					if (cli)
					{
						mstm.Clear();
						cli->ReadToEnd(mstm, 4096);
						sb.ClearStr();
						if (cli->GetRespStatus() == Net::WebStatus::SC_CREATED && cli->GetRespHeader(CSTR("Location"), sb))
						{
							SDEL_STRING(this->accountId);
							this->accountId = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
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
	NN<Text::String> url;
	if (!url.Set(this->urlNewAccount))
	{
		return false;
	}
	Net::HTTPClient *cli = this->ACMEPost(url, CSTR("{\"onlyReturnExisting\":true}"));
	if (cli == 0)
	{
		return false;
	}
	Bool succ = false;
	if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
	{
		Text::StringBuilderUTF8 sb;
		if (cli->GetRespHeader(CSTR("Location"), sb))
		{
			SDEL_STRING(this->accountId);
			this->accountId = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
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

Net::ACMEConn::Order *Net::ACMEConn::OrderNew(const UTF8Char *domainNames, UOSInt namesLen)
{
	NN<Text::String> url;
	if (!url.Set(this->urlNewOrder))
	{
		return 0;
	}
	Text::StringBuilderUTF8 sbNames;
	Text::StringBuilderUTF8 sb;
	Net::SocketUtil::AddressInfo addr;
	UOSInt i;
	Text::PString sarr[2];
	Bool found = false;
	sb.AppendC(UTF8STRC("{\"identifiers\":["));
	sbNames.AppendC(domainNames, namesLen);
	sarr[1] = sbNames;
	i = 2;
	while (i == 2)
	{
		i = Text::StrSplitP(sarr, 2, sarr[1], ',');
		if (found)
			sb.AppendUTF8Char(',');
		sb.AppendC(UTF8STRC("{\"type\":\""));
		if (Net::SocketUtil::SetAddrInfo(addr, sarr[0].ToCString()))
		{
			sb.AppendC(UTF8STRC("ip"));
		}
		else
		{
			sb.AppendC(UTF8STRC("dns"));
		}
		sb.AppendC(UTF8STRC("\",\"value\":\""));
		sb.AppendC(sarr[0].v, sarr[0].leng);
		sb.AppendC(UTF8STRC("\"}"));
	}
	sb.AppendC(UTF8STRC("]}"));

	Net::HTTPClient *cli = this->ACMEPost(url, sb.ToCString());
	if (cli->GetRespStatus() == 201)
	{
		Text::StringBuilderUTF8 sb;
		IO::MemoryStream mstm;
		cli->ReadToEnd(mstm, 2048);
		cli->GetRespHeader(CSTR("Location"), sb);
		DEL_CLASS(cli);

		const UInt8 *replyBuff = mstm.GetBuff(i);
		Order *order = this->OrderParse(replyBuff, i);
		if (order && sb.GetLength() > 0)
		{
			order->orderURL = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
		}
		return order;
	}
	else
	{
		DEL_CLASS(cli);
		return 0;
	}
}

Net::ACMEConn::Challenge *Net::ACMEConn::OrderAuthorize(NN<Text::String> authorizeURL, AuthorizeType authType)
{
	Net::HTTPClient *cli = this->ACMEPost(authorizeURL, CSTR(""));
	if (cli)
	{
		cli->GetRespStatus();
		IO::MemoryStream mstm;
		cli->ReadToEnd(mstm, 2048);
		DEL_CLASS(cli);

		Text::CStringNN sAuthType;
		if (!AuthorizeTypeGetName(authType).SetTo(sAuthType) || Text::StrEqualsICaseC(sAuthType.v, sAuthType.leng, UTF8STRC("UNKNOWN")))
		{
			return 0;
		}
		UOSInt i;
		UOSInt j;
		NN<Text::String> s;
		const UInt8 *authBuff = mstm.GetBuff(i);
		Text::JSONBase *json = Text::JSONBase::ParseJSONBytes(authBuff, i);
		if (json == 0)
		{
			return 0;
		}
		if (json->GetType() != Text::JSONType::Object)
		{
			json->EndUse();
			return 0;
		}
		Challenge *ret = 0;
		Text::JSONObject *authObj = (Text::JSONObject*)json;
		json = authObj->GetObjectValue(CSTR("challenges"));
		if (json && json->GetType() == Text::JSONType::Array)
		{
			Text::JSONArray *challArr = (Text::JSONArray*)json;
			i = 0;
			j = challArr->GetArrayLength();
			while (i < j)
			{
				json = challArr->GetArrayValue(i);
				if (json)
				{
					if (json->GetValueString(CSTR("type")).SetTo(s) && s->EqualsICase(sAuthType.v, sAuthType.leng))
					{
						ret = ChallengeJSON(json);
						break;
					}
				}
				i++;
			}
		}
		authObj->EndUse();
		return ret;
	}
	return 0;
}

Net::ACMEConn::Order *Net::ACMEConn::OrderGetStatus(const UTF8Char *orderURL)
{
	return 0;
}

Net::ACMEConn::Order *Net::ACMEConn::OrderFinalize(const UTF8Char *finalizeURL, Crypto::Cert::X509CertReq *csr)
{
	return 0;
}

void Net::ACMEConn::OrderFree(Order *order)
{
	SDEL_STRING(order->orderURL);
	if (order->authURLs)
	{
		order->authURLs->FreeAll();
		DEL_CLASS(order->authURLs);
	}
	OPTSTR_DEL(order->finalizeURL);
	SDEL_STRING(order->certificateURL);
	MemFree(order);
}

Net::ACMEConn::Challenge *Net::ACMEConn::ChallengeBegin(NN<Text::String> challURL)
{
	Net::HTTPClient *cli = this->ACMEPost(challURL, CSTR("{}"));
	if (cli)
	{
		if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
		{
			IO::MemoryStream mstm;
			cli->ReadToEnd(mstm, 2048);
			DEL_CLASS(cli);
			UOSInt i;
			const UInt8 *buff = mstm.GetBuff(i);
			return ChallengeParse(buff, i);
		}
		else
		{
			DEL_CLASS(cli);
			return 0;
		}
	}
	return 0;
}

Net::ACMEConn::Challenge *Net::ACMEConn::ChallengeGetStatus(NN<Text::String> challURL)
{
	Net::HTTPClient *cli = this->ACMEPost(challURL, CSTR(""));
	if (cli)
	{
		if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
		{
			IO::MemoryStream mstm;
			cli->ReadToEnd(mstm, 2048);
			DEL_CLASS(cli);
			UOSInt i;
			const UInt8 *buff = mstm.GetBuff(i);
			return ChallengeParse(buff, i);
		}
		else
		{
			DEL_CLASS(cli);
			return 0;
		}
	}
	return 0;
}

void Net::ACMEConn::ChallengeFree(Challenge *chall)
{
	chall->token->Release();
	chall->url->Release();
	MemFree(chall);
}

Bool Net::ACMEConn::NewKey()
{
	NN<Net::SSLEngine> ssl;
	if (this->ssl.SetTo(ssl))
	{
		NN<Crypto::Cert::X509Key> key;
		if (ssl->GenerateRSAKey().SetTo(key))
		{
			this->key.Delete();
			this->key = key;
			return true;
		}
	}
	return false;
}

Bool Net::ACMEConn::SetKey(Crypto::Cert::X509Key *key)
{
	if (key && key->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSA)
	{
		this->key.Delete();
		this->key =	NN<Crypto::Cert::X509Key>::ConvertFrom(key->Clone());
		return true;
	}
	return false;
}

Bool Net::ACMEConn::LoadKey(Text::CStringNN fileName)
{
	UInt8 keyPEM[4096];
	UOSInt keyPEMSize = IO::FileStream::LoadFile(fileName, keyPEM, 4096);
	if (keyPEMSize == 0)
	{
		return false;
	}
	NN<Text::String> s = Text::String::New(fileName.v, fileName.leng);
	NN<Crypto::Cert::X509File> x509;
	if (!Parser::FileParser::X509Parser::ParseBuff(Data::ByteArrayR(keyPEM, keyPEMSize), s).SetTo(x509))
	{
		s->Release();
		return false;
	}
	s->Release();
	if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Key && NN<Crypto::Cert::X509Key>::ConvertFrom(x509)->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSA)
	{
		this->key.Delete();
		this->key = NN<Crypto::Cert::X509Key>::ConvertFrom(x509);
		return true;
	}
	x509.Delete();
	return false;
}

Bool Net::ACMEConn::SaveKey(Text::CStringNN fileName)
{
	NN<Crypto::Cert::X509Key> key;
	if (!this->key.SetTo(key))
	{
		return false;
	}
	return Exporter::PEMExporter::ExportFile(fileName, key);
}

Net::ACMEConn::ACMEStatus Net::ACMEConn::ACMEStatusFromString(Optional<Text::String> status)
{
	NN<Text::String> s;
	if (!status.SetTo(s))
	{
		return ACMEStatus::Unknown;
	}
	if (s->EqualsICase(UTF8STRC("pending")))
	{
		return ACMEStatus::Pending;
	}
	else if (s->EqualsICase(UTF8STRC("ready")))
	{
		return ACMEStatus::Ready;
	}
	else if (s->EqualsICase(UTF8STRC("processing")))
	{
		return ACMEStatus::Processing;
	}
	else if (s->EqualsICase(UTF8STRC("valid")))
	{
		return ACMEStatus::Processing;
	}
	else if (s->EqualsICase(UTF8STRC("invalid")))
	{
		return ACMEStatus::Processing;
	}
	return ACMEStatus::Unknown;
}

Text::CString Net::ACMEConn::AuthorizeTypeGetName(AuthorizeType authType)
{
	switch (authType)
	{
	case AuthorizeType::HTTP_01:
		return CSTR("http-01");
	case AuthorizeType::DNS_01:
		return CSTR("dns-01");
	case AuthorizeType::TLS_ALPN_01:
		return CSTR("tls-alpn-01");
	case AuthorizeType::Unknown:
	default:
		return CSTR("Unknown");
	}
}

Net::ACMEConn::AuthorizeType Net::ACMEConn::AuthorizeTypeFromString(NN<Text::String> s)
{
	if (s->EqualsICase(UTF8STRC("http-01")))
	{
		return AuthorizeType::HTTP_01;
	}
	else if (s->EqualsICase(UTF8STRC("dns-01")))
	{
		return AuthorizeType::DNS_01;
	}
	else if (s->EqualsICase(UTF8STRC("tls-alpn-01")))
	{
		return AuthorizeType::TLS_ALPN_01;
	}
	else
	{
		return AuthorizeType::Unknown;
	}
}
