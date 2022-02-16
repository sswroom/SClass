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

Text::String *Net::ACMEConn::JWK(Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm *alg)
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
			sb.AppendC(UTF8STRC("{\"e\":\""));
			b64.EncodeBin(&sb, e, eSize);
			sb.AppendC(UTF8STRC("\",\"kty\":\"RSA\",\"n\":\""));
			b64.EncodeBin(&sb, m, mSize);
			sb.AppendC(UTF8STRC("\"}"));
			*alg = Crypto::Token::JWSignature::Algorithm::RS256;
			return Text::String::NewNotNull(sb.ToString());
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

Text::String *Net::ACMEConn::ProtectedJWK(Text::String *nonce, Text::String *url, Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm *alg, Text::String *accountId)
{
	Text::String *jwk = JWK(key, alg);
	if (jwk == 0)
	{
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("{\"alg\":\""));
	sb.Append(Crypto::Token::JWSignature::AlgorithmGetName(*alg));
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
	return Text::String::NewNotNull(sb.ToString());
}

Text::String *Net::ACMEConn::EncodeJWS(Net::SSLEngine *ssl, Text::CString protStr, Text::CString data, Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm alg)
{
	Text::StringBuilderUTF8 sb;
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	b64.EncodeBin(&sb, protStr.v, protStr.leng);
	sb.AppendUTF8Char('.');
	b64.EncodeBin(&sb, data.v, data.leng);
	Crypto::Token::JWSignature *sign;
	NEW_CLASS(sign, Crypto::Token::JWSignature(ssl, alg, key->GetASN1Buff(), key->GetASN1BuffSize()));
	sign->CalcHash(sb.ToString(), sb.GetLength());

	sb.ClearStr();
	sb.AppendC(UTF8STRC("{\"protected\":\""));
	b64.EncodeBin(&sb, protStr.v, protStr.leng);
	sb.AppendC(UTF8STRC("\",\"payload\":\""));
	b64.EncodeBin(&sb, data.v, data.leng);
	sb.AppendC(UTF8STRC("\",\"signature\":\""));
	sign->GetHashB64(&sb);
	sb.AppendC(UTF8STRC("\"}"));
	DEL_CLASS(sign);
	printf("Protected: %s\r\n", protStr.v);
	printf("Payload: %s\r\n", data.v);
	printf("JWS: %s\r\n", sb.ToString());
	return Text::String::New(sb.ToString(), sb.GetLength());
}

Bool Net::ACMEConn::KeyHash(Crypto::Cert::X509Key *key, Text::StringBuilderUTF8 *sb)
{
	Crypto::Token::JWSignature::Algorithm alg;
	Text::String *jwk = JWK(key, &alg);
	if (jwk == 0)
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

Net::HTTPClient *Net::ACMEConn::ACMEPost(Text::String *url, Text::CString data)
{
	if (this->nonce == 0)
	{
		return 0;
	}
	Text::String *protStr;
	Crypto::Token::JWSignature::Algorithm alg;
	protStr = ProtectedJWK(this->nonce, url, this->key, &alg, this->accountId);
	if (protStr == 0)
	{
		return 0;
	}
	Text::String *jws;
	jws = EncodeJWS(ssl, protStr->ToCString(), data, this->key, alg);
	protStr->Release();
	if (jws == 0)
	{
		return 0;
	}
	UOSInt jwsLen = jws->leng;
	Net::HTTPClient *cli = 0;
	cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, url->v, Net::WebUtil::RequestMethod::HTTP_POST, true);
	if (cli)
	{
		cli->AddContentType(CSTR("application/jose+json"));
		cli->AddContentLength(jwsLen);
		cli->Write(jws->v, jwsLen);
	}
	jws->Release();

	Text::StringBuilderUTF8 sb;
	cli->GetRespStatus();
	if (cli->GetRespHeader(CSTR("Replay-Nonce"), &sb))
	{
		SDEL_STRING(this->nonce);
		this->nonce = Text::String::New(sb.ToString(), sb.GetLength());
	}
	return cli;
}

Net::ACMEConn::Order *Net::ACMEConn::OrderParse(const UInt8 *buff, UOSInt buffSize)
{
	Text::JSONBase *json = Text::JSONBase::ParseJSONStrLen(buff, buffSize);
	if (json)
	{
		Text::String *s;
		Order *order = 0;
		if (json->GetType() == Text::JSONType::Object)
		{
			Text::JSONObject *o = (Text::JSONObject*)json;
			order = MemAlloc(Order, 1);
			MemClear(order, sizeof(Order));
			order->status = ACMEStatusFromString(o->GetObjectString(UTF8STRC("status")));
			s = o->GetObjectString(UTF8STRC("expires"));
			if (s)
			{
				Data::DateTime dt;
				dt.SetValue(s->v, s->leng);
				order->expires = dt.ToTicks();
			}
			s = o->GetObjectString(UTF8STRC("finalize"));
			order->finalizeURL = SCOPY_STRING(s);
			Text::JSONBase *auth = o->GetObjectValue(UTF8STRC("authorizations"));
			if (auth && auth->GetType() == Text::JSONType::Array)
			{
				Text::JSONArray *authArr = (Text::JSONArray*)auth;
				NEW_CLASS(order->authURLs, Data::ArrayList<Text::String*>());
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
	Text::String *type = json->GetString(UTF8STRC("type"));
	Text::String *status = json->GetString(UTF8STRC("status"));
	Text::String *url = json->GetString(UTF8STRC("url"));
	Text::String *token = json->GetString(UTF8STRC("token"));

	if (type && status && url && token)
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
	Text::JSONBase *json = Text::JSONBase::ParseJSONStrLen(buff, buffSize);
	Challenge *chall = 0;
	if (json)
	{
		chall = ChallengeJSON(json);
		json->EndUse();
	}
	return chall;
}

Net::ACMEConn::ACMEConn(Net::SocketFactory *sockf, const UTF8Char *serverHost, UInt16 port)
{
	UInt8 buff[2048];
	UOSInt recvSize;
	this->sockf = sockf;
	this->key = 0;
	this->ssl = Net::SSLEngineFactory::Create(sockf, false);
	this->serverHost = Text::String::NewNotNull(serverHost);
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
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sb.ToString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli)
	{
		IO::MemoryStream *mstm;
		if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
		{
			NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("Net.ACEClient.mstm")));
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
				Text::String *s;
				Text::JSONBase *json = Text::JSONBase::ParseJSONStrLen(jsonBuff, recvSize);
				if (json)
				{
					if (json->GetType() == Text::JSONType::Object)
					{
						Text::JSONObject *o = (Text::JSONObject*)json;
						if ((s = o->GetObjectString(UTF8STRC("newNonce"))) != 0)
						{
							this->urlNewNonce = s->Clone();
						}
						if ((s = o->GetObjectString(UTF8STRC("newAccount"))) != 0)
						{
							this->urlNewAccount = s->Clone();
						}
						if ((s = o->GetObjectString(UTF8STRC("newOrder"))) != 0)
						{
							this->urlNewOrder = s->Clone();
						}
						if ((s = o->GetObjectString(UTF8STRC("newAuthz"))) != 0)
						{
							this->urlNewAuthz = s->Clone();
						}
						if ((s = o->GetObjectString(UTF8STRC("revokeCert"))) != 0)
						{
							this->urlRevokeCert = s->Clone();
						}
						if ((s = o->GetObjectString(UTF8STRC("keyChange"))) != 0)
						{
							this->urlKeyChange = s->Clone();
						}
						Text::JSONBase *metaBase = o->GetObjectValue(UTF8STRC("meta"));
						if (metaBase && metaBase->GetType() == Text::JSONType::Object)
						{
							Text::JSONObject *metaObj = (Text::JSONObject*)metaBase;
							if ((s = metaObj->GetObjectString(UTF8STRC("termsOfService"))) != 0)
							{
								this->urlTermOfService = s->Clone();
							}
							if ((s = metaObj->GetObjectString(UTF8STRC("website"))) != 0)
							{
								this->urlWebsite = s->Clone();
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
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, this->urlNewNonce->v, Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli == 0)
	{
		return false;
	}
	Bool succ = false;
	if (cli->GetRespStatus() == Net::WebStatus::SC_NO_CONTENT)
	{
		Text::StringBuilderUTF8 sb;
		if (cli->GetRespHeader(CSTR("Replay-Nonce"), &sb))
		{
			SDEL_STRING(this->nonce);
			this->nonce = Text::String::New(sb.ToString(), sb.GetLength());
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
	Net::HTTPClient *cli = this->ACMEPost(this->urlNewAccount, CSTR("{\"onlyReturnExisting\":true}"));
	if (cli == 0)
	{
		return false;
	}
	Bool succ = false;
	if (cli->GetRespStatus() == Net::WebStatus::SC_BAD_REQUEST)
	{
		IO::MemoryStream mstm(UTF8STRC("Net.ACMEConn.AccountNew.mstm"));
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
				Text::String *s = o->GetObjectString(UTF8STRC("type"));
				if (s && s->Equals(UTF8STRC("urn:ietf:params:acme:error:accountDoesNotExist")))
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("{\"termsOfServiceAgreed\":true"));
					sb.AppendUTF8Char('}');
					cli = this->ACMEPost(this->urlNewAccount, sb.ToCString());
					if (cli)
					{
						mstm.Clear();
						cli->ReadToEnd(&mstm, 4096);
						sb.ClearStr();
						if (cli->GetRespStatus() == Net::WebStatus::SC_CREATED && cli->GetRespHeader(CSTR("Location"), &sb))
						{
							SDEL_STRING(this->accountId);
							this->accountId = Text::String::New(sb.ToString(), sb.GetLength());
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
	Net::HTTPClient *cli = this->ACMEPost(this->urlNewAccount, CSTR("{\"onlyReturnExisting\":true}"));
	if (cli == 0)
	{
		return false;
	}
	Bool succ = false;
	if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
	{
		Text::StringBuilderUTF8 sb;
		if (cli->GetRespHeader(CSTR("Location"), &sb))
		{
			SDEL_STRING(this->accountId);
			this->accountId = Text::String::New(sb.ToString(), sb.GetLength());
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
	Text::StringBuilderUTF8 sbNames;
	Text::StringBuilderUTF8 sb;
	Net::SocketUtil::AddressInfo addr;
	UOSInt i;
	Text::PString sarr[2];
	Bool found = false;
	sb.AppendC(UTF8STRC("{\"identifiers\":["));
	sbNames.AppendC(domainNames, namesLen);
	sarr[1].v = sbNames.ToString();
	sarr[1].leng = sbNames.GetLength();
	i = 2;
	while (i == 2)
	{
		i = Text::StrSplitP(sarr, 2, sarr[1], ',');
		if (found)
			sb.AppendUTF8Char(',');
		sb.AppendC(UTF8STRC("{\"type\":\""));
		if (Net::SocketUtil::GetIPAddr(sarr[0].v, sarr[0].leng, &addr))
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

	Net::HTTPClient *cli = this->ACMEPost(this->urlNewOrder, sb.ToCString());
	if (cli->GetRespStatus() == 201)
	{
		Text::StringBuilderUTF8 sb;
		IO::MemoryStream mstm(UTF8STRC("Net.ACMEConn.OrderNew.mstm"));
		cli->ReadToEnd(&mstm, 2048);
		cli->GetRespHeader(CSTR("Location"), &sb);
		DEL_CLASS(cli);

		const UInt8 *replyBuff = mstm.GetBuff(&i);
		Order *order = this->OrderParse(replyBuff, i);
		if (order && sb.GetLength() > 0)
		{
			order->orderURL = Text::String::New(sb.ToString(), sb.GetLength());
		}
		return order;
	}
	else
	{
		DEL_CLASS(cli);
		return 0;
	}
}

Net::ACMEConn::Challenge *Net::ACMEConn::OrderAuthorize(Text::String *authorizeURL, AuthorizeType authType)
{
	Net::HTTPClient *cli = this->ACMEPost(authorizeURL, CSTR(""));
	if (cli)
	{
		cli->GetRespStatus();
		IO::MemoryStream mstm(UTF8STRC("Net.ACMEConn.OrderAuthorize.mstm"));
		cli->ReadToEnd(&mstm, 2048);
		DEL_CLASS(cli);

		Text::CString sAuthType = AuthorizeTypeGetName(authType);
		if (sAuthType.v == 0 || Text::StrEqualsICaseC(sAuthType.v, sAuthType.leng, UTF8STRC("UNKNOWN")))
		{
			return 0;
		}
		UOSInt i;
		UOSInt j;
		Text::String *s;
		const UInt8 *authBuff = mstm.GetBuff(&i);
		Text::JSONBase *json = Text::JSONBase::ParseJSONStrLen(authBuff, i);
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
		json = authObj->GetObjectValue(UTF8STRC("challenges"));
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
					s = json->GetString(UTF8STRC("type"));
					if (s && s->EqualsICase(sAuthType.v, sAuthType.leng))
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
		LIST_FREE_STRING(order->authURLs);
		DEL_CLASS(order->authURLs);
	}
	SDEL_STRING(order->finalizeURL);
	SDEL_STRING(order->certificateURL);
	MemFree(order);
}

Net::ACMEConn::Challenge *Net::ACMEConn::ChallengeBegin(Text::String *challURL)
{
	Net::HTTPClient *cli = this->ACMEPost(challURL, CSTR("{}"));
	if (cli)
	{
		if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
		{
			IO::MemoryStream mstm(UTF8STRC("Net.ACMEConn.ChallengeBegin.mstm"));
			cli->ReadToEnd(&mstm, 2048);
			DEL_CLASS(cli);
			UOSInt i;
			const UInt8 *buff = mstm.GetBuff(&i);
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

Net::ACMEConn::Challenge *Net::ACMEConn::ChallengeGetStatus(Text::String *challURL)
{
	Net::HTTPClient *cli = this->ACMEPost(challURL, CSTR(""));
	if (cli)
	{
		if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
		{
			IO::MemoryStream mstm(UTF8STRC("Net.ACMEConn.ChallengeGetStatus.mstm"));
			cli->ReadToEnd(&mstm, 2048);
			DEL_CLASS(cli);
			UOSInt i;
			const UInt8 *buff = mstm.GetBuff(&i);
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
	SDEL_STRING(chall->token);
	SDEL_STRING(chall->url);
	MemFree(chall);
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

Bool Net::ACMEConn::LoadKey(Text::CString fileName)
{
	UInt8 keyPEM[4096];
	UOSInt keyPEMSize = IO::FileStream::LoadFile(fileName, keyPEM, 4096);
	if (keyPEMSize == 0)
	{
		return false;
	}
	Text::String *s = Text::String::New(fileName.v, fileName.leng);
	Crypto::Cert::X509File *x509 = Parser::FileParser::X509Parser::ParseBuff(keyPEM, keyPEMSize, s);
	s->Release();
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

Bool Net::ACMEConn::SaveKey(Text::CString fileName)
{
	if (this->key == 0)
	{
		return false;
	}
	return Exporter::PEMExporter::ExportFile(fileName, this->key);
}

Net::ACMEConn::ACMEStatus Net::ACMEConn::ACMEStatusFromString(Text::String* status)
{
	if (status == 0)
	{
		return ACMEStatus::Unknown;
	}
	if (status->EqualsICase(UTF8STRC("pending")))
	{
		return ACMEStatus::Pending;
	}
	else if (status->EqualsICase(UTF8STRC("ready")))
	{
		return ACMEStatus::Ready;
	}
	else if (status->EqualsICase(UTF8STRC("processing")))
	{
		return ACMEStatus::Processing;
	}
	else if (status->EqualsICase(UTF8STRC("valid")))
	{
		return ACMEStatus::Processing;
	}
	else if (status->EqualsICase(UTF8STRC("invalid")))
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
		return {UTF8STRC("http-01")};
	case AuthorizeType::DNS_01:
		return {UTF8STRC("dns-01")};
	case AuthorizeType::TLS_ALPN_01:
		return {UTF8STRC("tls-alpn-01")};
	case AuthorizeType::Unknown:
	default:
		return {UTF8STRC("Unknown")};
	}
}

Net::ACMEConn::AuthorizeType Net::ACMEConn::AuthorizeTypeFromString(Text::String *s)
{
	if (s == 0)
	{
		return AuthorizeType::Unknown;
	}
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
