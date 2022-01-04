#include "Stdafx.h"
#include "Crypto/Hash/SHA256.h"
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
		sb.AppendChar('\"', 1);
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

Text::String *Net::ACMEConn::EncodeJWS(Net::SSLEngine *ssl, const UTF8Char *protStr, const UTF8Char *data, Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm alg)
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
	sb.AppendC(UTF8STRC("{\"protected\":\""));
	b64.EncodeBin(&sb, protStr, Text::StrCharCnt(protStr));
	sb.AppendC(UTF8STRC("\",\"payload\":\""));
	b64.EncodeBin(&sb, data, Text::StrCharCnt(data));
	sb.AppendC(UTF8STRC("\",\"signature\":\""));
	sign->GetHashB64(&sb);
	sb.AppendC(UTF8STRC("\"}"));
	DEL_CLASS(sign);
	printf("Protected: %s\r\n", protStr);
	printf("Payload: %s\r\n", data);
	printf("JWS: %s\r\n", sb.ToString());
	return Text::String::NewNotNull(sb.ToString());
}

Bool Net::ACMEConn::KeyHash(Crypto::Cert::X509Key *key, Text::StringBuilderUTF *sb)
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

Net::HTTPClient *Net::ACMEConn::ACMEPost(Text::String *url, const Char *data)
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
	jws = EncodeJWS(ssl, protStr->v, (const UTF8Char*)data, this->key, alg);
	protStr->Release();
	if (jws == 0)
	{
		return 0;
	}
	UOSInt jwsLen = jws->leng;
	Net::HTTPClient *cli = 0;
	cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, url->v, "POST", true);
	if (cli)
	{
		cli->AddContentType((const UTF8Char*)"application/jose+json");
		cli->AddContentLength(jwsLen);
		cli->Write(jws->v, jwsLen);
	}
	jws->Release();

	Text::StringBuilderUTF8 sb;
	cli->GetRespStatus();
	if (cli->GetRespHeader((const UTF8Char*)"Replay-Nonce", &sb))
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
			order->status = ACMEStatusFromString(o->GetObjectString((const UTF8Char*)"status"));
			s = o->GetObjectString((const UTF8Char*)"expires");
			if (s)
			{
				Data::DateTime dt;
				dt.SetValue(s->v);
				order->expires = dt.ToTicks();
			}
			s = o->GetObjectString((const UTF8Char*)"finalize");
			order->finalizeURL = SCOPY_STRING(s);
			Text::JSONBase *auth = o->GetObjectValue((const UTF8Char*)"authorizations");
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
	Text::String *type = json->GetString("type");
	Text::String *status = json->GetString("status");
	Text::String *url = json->GetString("url");
	Text::String *token = json->GetString("token");

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
	this->ssl = Net::DefaultSSLEngine::Create(sockf, false);
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
	sb.Append(serverHost);
	if (port != 0 && port != 443)
	{
		sb.AppendChar(':', 1);
		sb.AppendU16(port);
	}
	sb.AppendC(UTF8STRC("/directory"));
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
				Text::String *s;
				Text::JSONBase *json = Text::JSONBase::ParseJSONStrLen(jsonBuff, recvSize);
				if (json)
				{
					if (json->GetType() == Text::JSONType::Object)
					{
						Text::JSONObject *o = (Text::JSONObject*)json;
						if ((s = o->GetObjectString((const UTF8Char*)"newNonce")) != 0)
						{
							this->urlNewNonce = s->Clone();
						}
						if ((s = o->GetObjectString((const UTF8Char*)"newAccount")) != 0)
						{
							this->urlNewAccount = s->Clone();
						}
						if ((s = o->GetObjectString((const UTF8Char*)"newOrder")) != 0)
						{
							this->urlNewOrder = s->Clone();
						}
						if ((s = o->GetObjectString((const UTF8Char*)"newAuthz")) != 0)
						{
							this->urlNewAuthz = s->Clone();
						}
						if ((s = o->GetObjectString((const UTF8Char*)"revokeCert")) != 0)
						{
							this->urlRevokeCert = s->Clone();
						}
						if ((s = o->GetObjectString((const UTF8Char*)"keyChange")) != 0)
						{
							this->urlKeyChange = s->Clone();
						}
						Text::JSONBase *metaBase = o->GetObjectValue((const UTF8Char*)"meta");
						if (metaBase && metaBase->GetType() == Text::JSONType::Object)
						{
							Text::JSONObject *metaObj = (Text::JSONObject*)metaBase;
							if ((s = metaObj->GetObjectString((const UTF8Char*)"termsOfService")) != 0)
							{
								this->urlTermOfService = s->Clone();
							}
							if ((s = metaObj->GetObjectString((const UTF8Char*)"website")) != 0)
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
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, this->urlNewNonce->v, "GET", true);
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
				Text::String *s = o->GetObjectString((const UTF8Char*)"type");
				if (s && s->Equals((const UTF8Char*)"urn:ietf:params:acme:error:accountDoesNotExist"))
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("{\"termsOfServiceAgreed\":true"));
					sb.AppendChar('}', 1);
					cli = this->ACMEPost(this->urlNewAccount, (const Char*)sb.ToString());
					if (cli)
					{
						mstm.Clear();
						cli->ReadToEnd(&mstm, 4096);
						sb.ClearStr();
						if (cli->GetRespStatus() == Net::WebStatus::SC_CREATED && cli->GetRespHeader((const UTF8Char*)"Location", &sb))
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

Net::ACMEConn::Order *Net::ACMEConn::OrderNew(const UTF8Char *domainNames)
{
	Text::StringBuilderUTF8 sbNames;
	Text::StringBuilderUTF8 sb;
	Net::SocketUtil::AddressInfo addr;
	UOSInt i;
	UTF8Char *sarr[2];
	Bool found = false;
	sb.AppendC(UTF8STRC("{\"identifiers\":["));
	sbNames.Append(domainNames);
	sarr[1] = sbNames.ToString();
	i = 2;
	while (i == 2)
	{
		i = Text::StrSplit(sarr, 2, sarr[1], ',');
		if (found)
			sb.AppendChar(',', 1);
		sb.AppendC(UTF8STRC("{\"type\":\""));
		if (Net::SocketUtil::GetIPAddr(sarr[0], &addr))
		{
			sb.AppendC(UTF8STRC("ip"));
		}
		else
		{
			sb.AppendC(UTF8STRC("dns"));
		}
		sb.AppendC(UTF8STRC("\",\"value\":\""));
		sb.Append(sarr[0]);
		sb.AppendC(UTF8STRC("\"}"));
	}
	sb.AppendC(UTF8STRC("]}"));

	Net::HTTPClient *cli = this->ACMEPost(this->urlNewOrder, (const Char*)sb.ToString());
	if (cli->GetRespStatus() == 201)
	{
		Text::StringBuilderUTF8 sb;
		IO::MemoryStream mstm((const UTF8Char*)"Net.ACMEConn.OrderNew.mstm");
		cli->ReadToEnd(&mstm, 2048);
		cli->GetRespHeader((const UTF8Char*)"Location", &sb);
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
	Net::HTTPClient *cli = this->ACMEPost(authorizeURL, "");
	if (cli)
	{
		cli->GetRespStatus();
		IO::MemoryStream mstm((const UTF8Char*)"Net.ACMEConn.OrderAuthorize.mstm");
		cli->ReadToEnd(&mstm, 2048);
		DEL_CLASS(cli);

		const UTF8Char *sAuthType = AuthorizeTypeGetName(authType);
		if (sAuthType == 0 || Text::StrEqualsICase(sAuthType, (const UTF8Char*)"UNKNOWN"))
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
		json = authObj->GetObjectValue((const UTF8Char*)"challenges");
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
					s = json->GetString("type");
					if (s && s->EqualsICase(sAuthType))
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
	Net::HTTPClient *cli = this->ACMEPost(challURL, "{}");
	if (cli)
	{
		if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
		{
			IO::MemoryStream mstm((const UTF8Char*)"Net.ACMEConn.ChallengeBegin.mstm");
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
	Net::HTTPClient *cli = this->ACMEPost(challURL, "");
	if (cli)
	{
		if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
		{
			IO::MemoryStream mstm((const UTF8Char*)"Net.ACMEConn.ChallengeGetStatus.mstm");
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

Bool Net::ACMEConn::LoadKey(const UTF8Char *fileName)
{
	UInt8 keyPEM[4096];
	UOSInt keyPEMSize = IO::FileStream::LoadFile(fileName, keyPEM, 4096);
	if (keyPEMSize == 0)
	{
		return false;
	}
	Text::String *s = Text::String::NewNotNull(fileName);
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

Bool Net::ACMEConn::SaveKey(const UTF8Char *fileName)
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
	if (status->EqualsICase((const UTF8Char*)"pending"))
	{
		return ACMEStatus::Pending;
	}
	else if (status->EqualsICase((const UTF8Char*)"ready"))
	{
		return ACMEStatus::Ready;
	}
	else if (status->EqualsICase((const UTF8Char*)"processing"))
	{
		return ACMEStatus::Processing;
	}
	else if (status->EqualsICase((const UTF8Char*)"valid"))
	{
		return ACMEStatus::Processing;
	}
	else if (status->EqualsICase((const UTF8Char*)"invalid"))
	{
		return ACMEStatus::Processing;
	}
	return ACMEStatus::Unknown;
}

const UTF8Char *Net::ACMEConn::AuthorizeTypeGetName(AuthorizeType authType)
{
	switch (authType)
	{
	case AuthorizeType::HTTP_01:
		return (const UTF8Char*)"http-01";
	case AuthorizeType::DNS_01:
		return (const UTF8Char*)"dns-01";
	case AuthorizeType::TLS_ALPN_01:
		return (const UTF8Char*)"tls-alpn-01";
	case AuthorizeType::Unknown:
	default:
		return (const UTF8Char*)"Unknown";
	}
}

Net::ACMEConn::AuthorizeType Net::ACMEConn::AuthorizeTypeFromString(Text::String *s)
{
	if (s == 0)
	{
		return AuthorizeType::Unknown;
	}
	if (s->EqualsICase((const UTF8Char*)"http-01"))
	{
		return AuthorizeType::HTTP_01;
	}
	else if (s->EqualsICase((const UTF8Char*)"dns-01"))
	{
		return AuthorizeType::DNS_01;
	}
	else if (s->EqualsICase((const UTF8Char*)"tls-alpn-01"))
	{
		return AuthorizeType::TLS_ALPN_01;
	}
	else
	{
		return AuthorizeType::Unknown;
	}
}
