#include "Stdafx.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA256.h"
#include "Net/HTTPClient.h"
#include "Net/IAMSmartAPI.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/FormEncoding.h"
#include <stdio.h>

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

Bool Net::IAMSmartAPI::GetKey()
{
	Text::StringBuilderUTF8 sbURL;
	sbURL.Append(CSTR("https://"));
	sbURL.Append(this->domain);
	sbURL.Append(CSTR("/api/v1/security/getKey"));
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sbURL.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, false);
	this->InitHTTPClient(cli, CSTR(""));
	IO::MemoryStream mstm;
	cli->ReadToEnd(mstm, 65536);
	
	mstm.Write(Data::ByteArrayR(U8STR(""), 1));
	printf("Status: %d\r\n", (Int32)cli->GetRespStatus());
	printf("Content: %s\r\n", mstm.GetBuff().Ptr());
	cli.Delete();
	return false;
}
