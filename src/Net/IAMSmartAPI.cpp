#include "Stdafx.h"
#include "Crypto/Cert/X509PubKey.h"
#include "Crypto/Encrypt/AES256GCM.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA256.h"
#include "Data/RandomBytesGenerator.h"
#include "Net/HTTPClient.h"
#include "Net/IAMSmartAPI.h"
#include "Text/JSONBuilder.h"
#include "Text/StringTool.h"
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
	if (content.leng > 0)
	{
		hmac.Calc(content.v, content.leng);
		cli->AddContentType(CSTR("application/json"));
	}
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

Optional<Text::JSONBase> Net::IAMSmartAPI::PostEncReq(Text::CStringNN url, NN<CEKInfo> cek, Text::CStringNN jsonMsg)
{
	Data::RandomBytesGenerator byteGen;
	UnsafeArray<UInt8> msgBuff = MemAllocArr(UInt8, jsonMsg.leng + 32);
	WriteMInt32(msgBuff.Ptr(), 12);
	byteGen.NextBytes(msgBuff + 4, 12);
	Crypto::Encrypt::AES256GCM aes(cek->key, msgBuff + 4);
	UOSInt encLeng = aes.Encrypt(jsonMsg.v, jsonMsg.leng, msgBuff + 16);
	if (encLeng != jsonMsg.leng + 16)
	{
		MemFreeArr(msgBuff);
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	Text::TextBinEnc::Base64Enc b64;
	sb.Append(CSTR("{\"content\":\""));
	b64.EncodeBin(sb, msgBuff, jsonMsg.leng + 32);
	sb.Append(CSTR("\"}"));
	MemFreeArr(msgBuff);
#if defined(VERBOSE)
	printf("PostEncReq.Url: %s\r\n", url.v.Ptr());
	printf("PostEncReq.Req: %s\r\n", sb.v.Ptr());
#endif
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, url, Net::WebUtil::RequestMethod::HTTP_POST, false);
	this->InitHTTPClient(cli, sb.ToCString());
	IO::MemoryStream mstm;
	cli->ReadToEnd(mstm, 65536);
	
	mstm.Write(Data::ByteArrayR(U8STR(""), 1));
	Net::WebStatus::StatusCode code = cli->GetRespStatus();
	cli.Delete();
#if defined(VERBOSE)
	printf("PostEncReq.Status: %d\r\n", (Int32)code);
	printf("PostEncReq.Content: %s\r\n", mstm.GetBuff().Ptr());
#endif
	if (code != Net::WebStatus::SC_OK)
	{
#if defined(VERBOSE)
		printf("Status is not OK\r\n");
#endif
		return 0;
	}
	NN<Text::JSONBase> json;
	if (!Text::JSONBase::ParseJSONStr(Text::CStringNN(mstm.GetBuff(), (UOSInt)mstm.GetLength() - 1)).SetTo(json))
	{
#if defined(VERBOSE)
		printf("Response is not JSON\r\n");
#endif
		return 0;
	}
	NN<Text::String> s;
	if (!json->GetValueString(CSTR("message")).SetTo(s) || !s->Equals(CSTR("SUCCESS")))
	{
#if defined(VERBOSE)
		printf("Response is not success\r\n");
#endif
		json->EndUse();
		return 0;
	}
	if (!json->GetValueString(CSTR("content")).SetTo(s))
	{
#if defined(VERBOSE)
		printf("Response content not found\r\n");
#endif
		json->EndUse();
		return 0;
	}

	msgBuff = MemAllocArr(UInt8, s->leng);
	UOSInt msgLeng = b64.DecodeBin(s->ToCString(), msgBuff);
	json->EndUse();
	if (msgLeng < 32)
	{
#if defined(VERBOSE)
		printf("Response content too short\r\n");
#endif
		MemFreeArr(msgBuff);
		return 0;
	}
	if (ReadMUInt32(&msgBuff[0]) != 12)
	{
#if defined(VERBOSE)
		printf("Response content IV not found\r\n");
#endif
		MemFreeArr(msgBuff);
		return 0;
	}
	UnsafeArray<UInt8> decBuff = MemAllocArr(UInt8, msgLeng - 32 + 1);
	aes.SetIV(&msgBuff[4]);
	UOSInt decLeng = aes.Decrypt(&msgBuff[16], msgLeng - 16, decBuff);
	MemFreeArr(msgBuff);
	if (decLeng != msgLeng - 32)
	{
#if defined(VERBOSE)
		printf("Decrypted length not valid\r\n");
#endif
		MemFreeArr(decBuff);
		return 0;
	}
	decBuff[decLeng] = 0;
#if defined(VERBOSE)
	printf("PostEncReq.Dec Content = %s\r\n", decBuff.Ptr());
#endif
	Optional<Text::JSONBase> decJSON = Text::JSONBase::ParseJSONStr(Text::CStringNN(decBuff, decLeng));
	MemFreeArr(decBuff);
#if defined(VERBOSE)
	if (decJSON.IsNull())
	{
		printf("Decrypted content is not JSON\r\n");
	}
#endif
	return decJSON;
}

Optional<Text::String> Net::IAMSmartAPI::ParseAddress(NN<Text::JSONBase> json, Text::CStringNN path)
{
	NN<Text::JSONBase> addr;
	if (!json->GetValue(path).SetTo(addr))
		return 0;
	Text::StringBuilderUTF8 sb;
	NN<Text::String> s;
	if (addr->GetValue(CSTR("ChiPremisesAddress")).SetTo(json))
	{
		sb.AppendOpt(json->GetValueString(CSTR("Region")));
		sb.AppendOpt(json->GetValueString(CSTR("ChiDistrict.Sub-district")));
		sb.AppendOpt(json->GetValueString(CSTR("ChiStreet.StreetName")));
		sb.AppendOpt(json->GetValueString(CSTR("ChiStreet.BuildingNoFrom")));
		sb.Append(CSTR("號"));
		sb.AppendOpt(json->GetValueString(CSTR("BuildingName")));
		sb.AppendOpt(json->GetValueString(CSTR("ChiBlock.BlockNo")));
		sb.AppendOpt(json->GetValueString(CSTR("ChiBlock.BlockDescriptor")));
		sb.AppendOpt(json->GetValueString(CSTR("Chi3dAddress.ChiFloor.FloorNum")));
		sb.AppendOpt(json->GetValueString(CSTR("Chi3dAddress.ChiUnit.UnitNo")));
		sb.AppendOpt(json->GetValueString(CSTR("Chi3dAddress.ChiUnit.UnitDescriptor")));
		return Text::String::New(sb.ToCString());
	}
	else if (addr->GetValue(CSTR("EngPremisesAddress")).SetTo(json))
	{
		if (json->GetValue(CSTR("Eng3dAddress.EngUnit")).NotNull())
		{
			sb.AppendOpt(json->GetValueString(CSTR("Eng3dAddress.EngUnit.UnitDescriptor")));
			sb.AppendUTF8Char(' ');
			sb.AppendOpt(json->GetValueString(CSTR("Eng3dAddress.EngUnit.UnitNo")));
			sb.Append(CSTR(", "));
		}
		if (json->GetValueString(CSTR("Eng3dAddress.EngFloor.FloorNum")).SetTo(s))
		{
			sb.Append(s);
			sb.Append(CSTR("/F "));
		}
		if (json->GetValue(CSTR("EngBlock")).NotNull())
		{
			sb.AppendOpt(json->GetValueString(CSTR("EngBlock.BlockDescriptor")));
			sb.AppendUTF8Char(' ');
			sb.AppendOpt(json->GetValueString(CSTR("EngBlock.BlockNo")));
			sb.Append(CSTR(", "));
		}
		sb.AppendOpt(json->GetValueString(CSTR("BuildingName")));
		if (json->GetValue(CSTR("EngStreet")).NotNull())
		{
			if (sb.leng > 0)
				sb.Append(CSTR(", "));
			Bool found = false;
			if (json->GetValueString(CSTR("EngStreet.BuildingNoFrom")).SetTo(s))
			{
				if (found) sb.AppendUTF8Char(' ');
				sb.Append(s);
				found = true;
			}
			if (json->GetValueString(CSTR("EngStreet.StreetName")).SetTo(s))
			{
				if (found) sb.AppendUTF8Char(' ');
				sb.Append(s);
				found = true;
			}
		}
		if (json->GetValueString(CSTR("EngDistrict.Sub-district")).SetTo(s))
		{
			if (sb.leng > 0)
				sb.Append(CSTR(", "));
			sb.Append(s);
		}
		if (json->GetValueString(CSTR("Region")).SetTo(s))
		{
			if (s->Equals(CSTR("KLN")))
				sb.Append(CSTR(", KOWLOON"));
			else if (s->Equals(CSTR("HK")))
				sb.Append(CSTR(", HONG KONG"));
			else if (s->Equals(CSTR("NT")))
				sb.Append(CSTR(", NEW TERRITORIES"));
		}
		return Text::String::New(sb.ToCString());
	}
	else if (addr->GetValue(CSTR("FreeFormatAddress")).SetTo(json))
	{
		sb.AppendOpt(json->GetValueString(CSTR("AddressLine1")));
		sb.Append(CSTR(", "));
		sb.AppendOpt(json->GetValueString(CSTR("AddressLine2")));
		sb.Append(CSTR(", "));
		sb.AppendOpt(json->GetValueString(CSTR("AddressLine3")));
		return Text::String::New(sb.ToCString());
	}
	else if (addr->GetValue(CSTR("PostBoxAddress")).SetTo(json))
	{
		if (json->GetValue(CSTR("EngPostBox")).SetTo(addr))
		{
			sb.AppendOpt(addr->GetValueString(CSTR("PostOffice")));
			sb.Append(CSTR(" Box "));
			sb.AppendI32(addr->GetValueAsInt32(CSTR("PoBoxNo")));
			sb.Append(CSTR(", "));
			sb.AppendOpt(addr->GetValueString(CSTR("PostOfficeRegion")));
			return Text::String::New(sb.ToCString());
		}
		else if (json->GetValue(CSTR("ChiPostBox")).SetTo(addr))
		{
			sb.AppendOpt(addr->GetValueString(CSTR("PostOfficeRegion")));
			sb.AppendOpt(addr->GetValueString(CSTR("PostOffice")));
			sb.Append(CSTR("信箱"));
			sb.AppendI32(addr->GetValueAsInt32(CSTR("PoBoxNo")));
			sb.Append(CSTR("號"));
			return Text::String::New(sb.ToCString());
		}
	}
	return 0;
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

void Net::IAMSmartAPI::FreeCEK(NN<CEKInfo> cek)
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

void Net::IAMSmartAPI::FreeToken(NN<TokenInfo> token)
{
	token->accessToken->Release();
	token->openID->Release();
}

Bool Net::IAMSmartAPI::GetToken(Text::CStringNN code, NN<CEKInfo> cek, NN<TokenInfo> token)
{
	Text::JSONBuilder jsonMsg(Text::JSONBuilder::OT_OBJECT);
	jsonMsg.ObjectAddStr(CSTR("code"), code);
	jsonMsg.ObjectAddStr(CSTR("grantType"), CSTR("authorization_code"));
	Text::StringBuilderUTF8 sbURL;
	sbURL.Append(CSTR("https://"));
	sbURL.Append(this->domain);
	sbURL.Append(CSTR("/api/v1/auth/getToken"));
	NN<Text::JSONBase> json;
	if (!this->PostEncReq(sbURL.ToCString(), cek, jsonMsg.Build()).SetTo(json))
		return false;
	Int64 expiresIn;
	NN<Text::String> accessToken;
	NN<Text::String> openID;
	if (json->GetValueAsInt64(CSTR("issueAt"), token->issueAt) && json->GetValueAsInt64(CSTR("expiresIn"), expiresIn) && json->GetValueAsInt64(CSTR("lastModifiedDate"), token->lastModifiedDate) && json->GetValueString(CSTR("accessToken")).SetTo(accessToken) && json->GetValueString(CSTR("openID")).SetTo(openID))
	{
		token->accessToken = accessToken->Clone();
		token->openID = openID->Clone();
		token->expiresAt = token->issueAt + expiresIn;
		json->EndUse();
		return true;
	}
	json->EndUse();
	return false;
}

void Net::IAMSmartAPI::FreeProfiles(NN<ProfileInfo> profiles)
{
	OPTSTR_DEL(profiles->hkid);
	OPTSTR_DEL(profiles->prefix);
	OPTSTR_DEL(profiles->enName);
	OPTSTR_DEL(profiles->chName);
	OPTSTR_DEL(profiles->chNameVerfied);
	OPTSTR_DEL(profiles->homeTelNumber);
	OPTSTR_DEL(profiles->homeTelNumberICC);
	OPTSTR_DEL(profiles->officeTelNumber);
	OPTSTR_DEL(profiles->officeTelNumberICC);
	OPTSTR_DEL(profiles->mobileTelNumber);
	OPTSTR_DEL(profiles->mobileTelNumberICC);
	OPTSTR_DEL(profiles->emailAddress);
	OPTSTR_DEL(profiles->residentialAddress);
	OPTSTR_DEL(profiles->postalAddress);
}

Bool Net::IAMSmartAPI::GetProfiles(NN<TokenInfo> token, Text::CStringNN eMEFields, Text::CStringNN profileFields, NN<CEKInfo> cek, NN<ProfileInfo> profiles)
{
	Text::JSONBuilder jsonMsg(Text::JSONBuilder::OT_OBJECT);
	jsonMsg.ObjectAddStr(CSTR("accessToken"), token->accessToken);
	jsonMsg.ObjectAddStr(CSTR("openID"), token->openID);
	jsonMsg.ObjectAddArrayStr(CSTR("profileFields"), profileFields, ',');
	jsonMsg.ObjectAddArrayStr(CSTR("eMEFields"), eMEFields, ',');
	Text::StringBuilderUTF8 sbURL;
	sbURL.Append(CSTR("https://"));
	sbURL.Append(this->domain);
	sbURL.Append(CSTR("/api/v1/profiles"));
	NN<Text::JSONBase> json;
	if (!this->PostEncReq(sbURL.ToCString(), cek, jsonMsg.Build()).SetTo(json))
		return false;
	profiles->hkid = 0;
	profiles->hkidChk = 0;
	profiles->prefix = 0;
	profiles->enName = 0;
	profiles->chName = 0;
	profiles->chNameVerfied = 0;
	profiles->birthDate = 0;
	profiles->gender = 0;
	profiles->maritalStatus = 0;
	profiles->homeTelNumber = 0;
	profiles->homeTelNumberICC = 0;
	profiles->officeTelNumber = 0;
	profiles->officeTelNumberICC = 0;
	profiles->mobileTelNumber = 0;
	profiles->mobileTelNumberICC = 0;
	profiles->emailAddress = 0;
	profiles->residentialAddress = 0;
	profiles->postalAddress = 0;
	profiles->educationLevel = 0;
	NN<Text::String> s;
	profiles->hkid = json->GetValueNewString(CSTR("idNo.Identification"));
	if (json->GetValueString(CSTR("idNo.CheckDigit")).SetTo(s))
		profiles->hkidChk = s->v[0];
	profiles->prefix = json->GetValueNewString(CSTR("prefix"));
	profiles->enName = json->GetValueNewString(CSTR("enName.UnstructuredName"));
	profiles->chName = json->GetValueNewString(CSTR("chName.ChineseName"));
	profiles->chNameVerfied = json->GetValueNewString(CSTR("chNameVerified"));
	if (json->GetValueString(CSTR("birthDate")).SetTo(s))
		profiles->birthDate = s->ToUInt32();
	if (json->GetValueString(CSTR("gender")).SetTo(s))
		profiles->gender = s->v[0];
	if (json->GetValueString(CSTR("maritalStatus")).SetTo(s))
		profiles->maritalStatus = s->v[0];
	profiles->homeTelNumber = json->GetValueNewString(CSTR("homeTelNumber.SubscriberNumber"));
	profiles->homeTelNumberICC = json->GetValueNewString(CSTR("homeTelNumber.CountryCode"));
	profiles->officeTelNumber = json->GetValueNewString(CSTR("officeTelNumber.SubscriberNumber"));
	profiles->officeTelNumberICC = json->GetValueNewString(CSTR("officeTelNumber.CountryCode"));
	profiles->mobileTelNumber = json->GetValueNewString(CSTR("mobileNumber.SubscriberNumber"));
	profiles->mobileTelNumberICC = json->GetValueNewString(CSTR("mobileNumber.CountryCode"));
	profiles->emailAddress = json->GetValueNewString(CSTR("emailAddress"));
	profiles->residentialAddress = ParseAddress(json, CSTR("residentialAddress"));
	profiles->postalAddress = ParseAddress(json, CSTR("postalAddress"));
	if (json->GetValueString(CSTR("educationLevel")).SetTo(s))
		profiles->educationLevel = s->v[0];
	json->EndUse();
	return true;

}
