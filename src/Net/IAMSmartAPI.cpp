#include "Stdafx.h"
#include "Crypto/Cert/X509PubKey.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA256.h"
#include "Net/HTTPClient.h"
#include "Net/IAMSmartAPI.h"
#include "Text/JSON.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/FormEncoding.h"

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

void Net::IAMSmartAPI::InitHTTPClient(NN<Net::HTTPClient> cli, Text::CStringNN content)
{
	UInt8 buff[64];
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Text::CStringNN signatureMethod = CSTR("HmacSHA256");
	Int64 timestamp = Data::DateTimeUtil::GetCurrTimeMillis();
	Crypto::Hash::SHA256 sha256;
	Crypto::Hash::HMAC hmac(sha256, this->clientSecret->v, this->clientSecret->leng);
	hmac.Calc(this->clientID->v, this->clientID->leng);
	hmac.Calc(signatureMethod.v, signatureMethod.leng);
	cli->AddHeaderC(CSTR("clientID"), this->clientID->ToCString());
	cli->AddHeaderC(CSTR("signatureMethod"), signatureMethod);
	sptr = Text::StrInt64(sbuff, timestamp);
	hmac.Calc(sbuff, (UOSInt)(sptr - sbuff));
	cli->AddHeaderC(CSTR("timestamp"), CSTRP(sbuff, sptr));
	this->rand.NextBytes(buff, 18);
	sptr = Text::StrHexBytes(sbuff, buff, 18, 0);
	hmac.Calc(sbuff, (UOSInt)(sptr - sbuff));
	cli->AddHeaderC(CSTR("nonce"), CSTRP(sbuff, sptr));
	hmac.GetValue(buff);
	Text::StringBuilderUTF8 sbSignature;
	Text::TextBinEnc::Base64Enc b64;
	sptr = b64.EncodeBin(sbuff, buff, hmac.GetResultSize());
	Text::TextBinEnc::FormEncoding::FormEncode(sbSignature, CSTRP(sbuff, sptr));
	cli->AddHeaderC(CSTR("signature"), sbSignature.ToCString());
	cli->AddContentLength(content.leng);
	if (content.leng > 0)
	{
		cli->Write(content.ToByteArray());
	}
}

Net::IAMSmartAPI::IAMSmartAPI(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN domain, Text::CStringNN clientID, Text::CStringNN clientSecret)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->domain = Text::String::New(domain);
	this->clientID = Text::String::New(clientID);
	this->clientSecret = Text::String::New(clientSecret);
}

Net::IAMSmartAPI::~IAMSmartAPI()
{
	this->domain->Release();
	this->clientID->Release();
	this->clientSecret->Release();
}

void Net::IAMSmartAPI::FreeCEK(NN<CEKInfo> cek) const
{
	MemFreeArr(cek->key);
}

Bool Net::IAMSmartAPI::GetKey(NN<Crypto::Cert::X509PrivKey> privKey, NN<CEKInfo> cek)
{
	NN<Net::SSLEngine> ssl;
	if (!this->ssl.SetTo(ssl))
	{
#if defined(VERBOSE)
		printf("SSLEngine not found\r\n");
#endif
		return false;
	}
	Text::StringBuilderUTF8 sbURL;
	sbURL.Append(CSTR("https://"));
	sbURL.Append(this->domain);
	sbURL.Append(CSTR("/api/v1/security/getKey"));
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sbURL.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
	this->InitHTTPClient(cli, CSTR(""));
	IO::MemoryStream mstm;
	cli->ReadToEnd(mstm, 65536);
	
	mstm.Write(Data::ByteArrayR(U8STR(""), 1));
	Net::WebStatus::StatusCode code = cli->GetRespStatus();
	cli.Delete();
#if defined(VERBOSE)
	printf("GetKey.Status: %d\r\n", (Int32)code);
	printf("GetKey.Content: %s\r\n", mstm.GetBuff().Ptr());
#endif
	if (code != Net::WebStatus::SC_OK)
	{
#if defined(VERBOSE)
		printf("Status is not OK\r\n");
#endif
		return false;
	}
	NN<Text::JSONBase> json;
	if (!Text::JSONBase::ParseJSONStr(Text::CStringNN(mstm.GetBuff(), (UOSInt)mstm.GetLength() - 1)).SetTo(json))
	{
#if defined(VERBOSE)
		printf("Response is not JSON\r\n");
#endif
		return false;
	}
	NN<Text::String> secretKey;
	NN<Text::String> pubKey;
	Int64 issueAt;
	Int64 expiresIn;
	if (!json->GetValueString(CSTR("content.secretKey")).SetTo(secretKey) ||
		!json->GetValueString(CSTR("content.pubKey")).SetTo(pubKey) ||
		!json->GetValueAsInt64(CSTR("content.issueAt"), issueAt) ||
		!json->GetValueAsInt64(CSTR("content.expiresIn"), expiresIn))
	{
#if defined(VERBOSE)
		printf("Response content is not valid\r\n");
#endif
		json->EndUse();
		return false;
	}

	UnsafeArray<UInt8> pubKeyBuff = MemAllocArr(UInt8, pubKey->leng);
	UOSInt pubKeyLen;
	Text::TextBinEnc::Base64Enc b64;
	pubKeyLen = b64.DecodeBin(pubKey->ToCString(), pubKeyBuff);
	if (!Crypto::Cert::X509File::IsPublicKeyInfo(pubKeyBuff, pubKeyBuff + pubKeyLen, "1"))
	{
#if defined(VERBOSE)
		printf("PubKey is not valid\r\n");
#endif
		MemFreeArr(pubKeyBuff);
		json->EndUse();
		return false;
	}
	NN<Crypto::Cert::X509PubKey> pk;
	NEW_CLASSNN(pk, Crypto::Cert::X509PubKey(CSTR("PublicKey.key"), Data::ByteArrayR(pubKeyBuff, pubKeyLen)));
	MemFreeArr(pubKeyBuff);

	UInt8 privKeyId[20];
	UInt8 pubKeyId[20];
	if (!pk->GetKeyId(BYTEARR(pubKeyId)) || !privKey->GetKeyId(BYTEARR(privKeyId)))
	{
#if defined(VERBOSE)
		printf("Error in getting key id\r\n");
#endif
		pk.Delete();
		json->EndUse();
		return false;
	}
	pk.Delete();
	if (!Text::StrEqualsC(pubKeyId, 20, privKeyId, 20))
	{
#if defined(VERBOSE)
		printf("PubKey and PrivKey is not the same\r\n");
#endif
		json->EndUse();
		return false;
	}
	UnsafeArray<UInt8> cekBuff1 = MemAllocArr(UInt8, secretKey->leng);
	UInt8 cekBuff2[256];
	UOSInt cekLeng;
	cekLeng = b64.DecodeBin(secretKey->ToCString(), cekBuff1);
	if (cekLeng != 256)
	{
		MemFreeArr(cekBuff1);
#if defined(VERBOSE)
		printf("secretKey length is not valid\r\n");
#endif
		json->EndUse();
		return false;
	}
	json->EndUse();
	NN<Crypto::Cert::X509Key> prkey;
	if (!privKey->CreateKey().SetTo(prkey))
	{
		MemFreeArr(cekBuff1);
#if defined(VERBOSE)
		printf("Error in converting PrivKey to Key\r\n");
#endif
		return false;
	}
	cekLeng = ssl->Decrypt(prkey, cekBuff2, Data::ByteArrayR(cekBuff1, cekLeng), Crypto::Encrypt::RSACipher::Padding::PKCS1);
	prkey.Delete();
	MemFreeArr(cekBuff1);
	if (cekLeng != 32)
	{
#if defined(VERBOSE)
		printf("Decrypted key length is not valid\r\n");
#endif
		return false;
	}
#if defined(VERBOSE)
	sbURL.ClearStr();
	sbURL.AppendHexBuff(cekBuff2, cekLeng, ' ', Text::LineBreakType::CRLF);
	printf("CEK = %s\r\n", sbURL.ToPtr());
	sbURL.ClearStr();
	sbURL.AppendTSNoZone(Data::Timestamp(issueAt, Data::DateTimeUtil::GetLocalTzQhr()));
	printf("IssueAt = %s\r\n", sbURL.ToPtr());
	sbURL.ClearStr();
	sbURL.AppendTSNoZone(Data::Timestamp(issueAt + expiresIn, Data::DateTimeUtil::GetLocalTzQhr()));
	printf("ExpiresAt = %s\r\n", sbURL.ToPtr());
#endif
	cek->key = MemAllocArr(UInt8, cekLeng);
	MemCopyNO(cek->key.Ptr(), cekBuff2, cekLeng);
	cek->keyLen = cekLeng;
	cek->issueAt = issueAt;
	cek->expiresAt = issueAt + expiresIn;
	return true;
}

Bool Net::IAMSmartAPI::RevokeKey()
{
	Text::StringBuilderUTF8 sbURL;
	sbURL.Append(CSTR("https://"));
	sbURL.Append(this->domain);
	sbURL.Append(CSTR("/api/v1/security/revokeKey"));
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sbURL.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
	this->InitHTTPClient(cli, CSTR(""));
	IO::MemoryStream mstm;
	cli->ReadToEnd(mstm, 65536);
	
	mstm.Write(Data::ByteArrayR(U8STR(""), 1));
	Net::WebStatus::StatusCode code = cli->GetRespStatus();
	cli.Delete();
#if defined(VERBOSE)
	printf("RevokeKey.Status: %d\r\n", (Int32)code);
	printf("RevokeKey.Content: %s\r\n", mstm.GetBuff().Ptr());
#endif
	if (code != Net::WebStatus::SC_OK)
	{
#if defined(VERBOSE)
		printf("Status is not OK\r\n");
#endif
		return false;
	}
	NN<Text::JSONBase> json;
	if (!Text::JSONBase::ParseJSONStr(Text::CStringNN(mstm.GetBuff(), (UOSInt)mstm.GetLength() - 1)).SetTo(json))
	{
#if defined(VERBOSE)
		printf("Response is not JSON\r\n");
#endif
		return false;
	}
	NN<Text::String> s;
	Bool succ = json->GetValueString(CSTR("message")).SetTo(s) && s->Equals(CSTR("SUCCESS"));
	json->EndUse();
	return succ;
}
