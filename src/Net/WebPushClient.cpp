#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/WebPushClient.h"
#include "Text/TextBinEnc/Base64Enc.h"

Net::WebPushClient::WebPushClient(NN<Net::TCPClientFactory> clif, NN<Net::SSLEngine> ssl)
{
	this->clif = clif;
	this->ssl = ssl;
}

Net::WebPushClient::~WebPushClient()
{
}

Bool Net::WebPushClient::Push(Text::CStringNN endPoint, NN<Crypto::Cert::X509Key> key, Text::CStringNN email, UIntOS ttl) const
{
	if (key->GetKeyType() != Crypto::Cert::X509File::KeyType::ECDSA)
	{
		return false;
	}
	if (key->GetECName() != Crypto::Cert::X509File::ECName::secp256r1)
	{
		return false;
	}
	UnsafeArray<const UInt8> keyBuff;
	UIntOS keyLeng;
	if (!key->GetECPublic(keyLeng).SetTo(keyBuff))
	{
		return false;
	}
	Text::CStringNN header = CSTR("{\"typ\":\"JWT\",\"alg\":\"ES256\"}");
	Text::StringBuilderUTF8 sbClaim;
	UIntOS i;
	UInt8 signData[256];
	UIntOS signLen;
	UInt8 signRAW[256];
	UIntOS signRAWLen;
	sbClaim.Append(CSTR("{\"aud\":\""));
	i = endPoint.IndexOf('/', 8);
	sbClaim.AppendC(endPoint.v, i);
	sbClaim.Append(CSTR("\",\"exp\":"));
	sbClaim.AppendI64(Data::DateTimeUtil::GetCurrTimeMillis() / 1000 + 43200);
	sbClaim.Append(CSTR(",\"sub\":\"mailto:"));
	sbClaim.Append(email);
	sbClaim.Append(CSTR("\"}"));
	Text::StringBuilderUTF8 sb;
	Text::TextBinEnc::Base64Enc b64t(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, false);
	sb.Append(CSTR("WebPush "));
	b64t.EncodeBin(sb, header.v, header.leng);
	sb.AppendUTF8Char('.');
	b64t.EncodeBin(sb, sbClaim.v, sbClaim.leng);
	if (!this->ssl->Signature(key, Crypto::Hash::HashType::SHA256, sb.ToCString().Substring(8).ToByteArray(), signData, signLen))
	{
		return false;
	}
	sb.AppendUTF8Char('.');
	signRAWLen = Crypto::Cert::X509File::ECSignData2RAW(signRAW, Data::ByteArrayR(signData, signLen));
	b64t.EncodeBin(sb, signRAW, signRAWLen);
	Text::StringBuilderUTF8 sbKey;
	sbKey.Append(CSTR("p256ecdsa="));
	b64t.EncodeBin(sbKey, keyBuff, keyLeng);
	printf("WebPushClient.Push: Authorization: %s\r\n", sb.v.Ptr());
	printf("WebPushClient.Push: Crypto-Key: %s\r\n", sbKey.v.Ptr());
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, endPoint, Net::WebUtil::RequestMethod::HTTP_POST, false);
	cli->AddContentLength(0);
	cli->AddHeaderC(CSTR("Authorization"), sb.ToCString());
	cli->AddHeaderC(CSTR("Crypto-Key"), sbKey.ToCString());
	sb.ClearStr();
	sb.AppendUIntOS(ttl);
	cli->AddHeaderC(CSTR("TTL"), sb.ToCString());
	cli->WriteCont(signData, 0);
	sb.ClearStr();
	cli->ReadAllContent(sb, 16384, 1048576);
	Net::WebStatus::StatusCode sc = cli->GetRespStatus();
	printf("WebPushClient.Push: Status = %d\r\n", (UInt32)sc);
	printf("WebPushClient.Push: %s\r\n", sb.v.Ptr());
	cli.Delete();
	return ((Int32)sc / 100) == 2;
}
