#include "Stdafx.h"
#include "IO/FileStream.h"
#include "Net/Google/GoogleServiceAccount.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/JSON.h"
#include "Text/JSONBuilder.h"

Net::Google::GoogleServiceAccount::GoogleServiceAccount(NN<Text::String> projectId, NN<Text::String> privateKeyId, NN<Crypto::Cert::X509PrivKey> privateKey, NN<Text::String> clientEmail, NN<Text::String> clientId, NN<Text::String> authUri, NN<Text::String> tokenUri, NN<Text::String> authProviderX509CertUrl, NN<Text::String> clientX509CertUrl, NN<Text::String> universeDomain)
{
	this->projectId = projectId;
	this->privateKeyId = privateKeyId;
	this->privateKey = privateKey;
	this->clientEmail = clientEmail;
	this->clientId = clientId;
	this->authUri = authUri;
	this->tokenUri = tokenUri;
	this->authProviderX509CertUrl = authProviderX509CertUrl;
	this->clientX509CertUrl = clientX509CertUrl;
	this->universeDomain = universeDomain;
}

Net::Google::GoogleServiceAccount::~GoogleServiceAccount()
{
	this->projectId->Release();
	this->privateKeyId->Release();
	this->privateKey.Delete();
	this->clientEmail->Release();
	this->clientId->Release();
	this->authUri->Release();
	this->tokenUri->Release();
	this->authProviderX509CertUrl->Release();
	this->clientX509CertUrl->Release();
	this->universeDomain->Release();
}

Optional<Crypto::Token::JWToken> Net::Google::GoogleServiceAccount::ToJWT(Optional<Net::SSLEngine> ssl, Text::CStringNN scope) const
{
	Int64 currTime = Data::DateTimeUtil::GetCurrTimeMillis() / 1000;
	Text::JSONBuilder claim(Text::JSONBuilder::OT_OBJECT);
	claim.ObjectAddStr(CSTR("iss"), this->clientEmail);
	claim.ObjectAddStr(CSTR("scope"), scope);
	claim.ObjectAddStr(CSTR("aud"), this->tokenUri);
	claim.ObjectAddInt64(CSTR("exp"), currTime + 3600);
	claim.ObjectAddInt64(CSTR("iat"), currTime);
	return Crypto::Token::JWToken::GenerateRSA(Crypto::Token::JWSignature::Algorithm::RS256, claim.Build(), ssl, this->privateKeyId->ToCString(), this->privateKey);
}

Optional<Net::Google::GoogleServiceAccount> Net::Google::GoogleServiceAccount::FromFile(Text::CStringNN filePath)
{
	IO::FileStream fs(filePath, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
		return nullptr;
	UInt8 buff[8192];
	UOSInt readSize;
	Text::StringBuilderUTF8 sb;
	while ((readSize = fs.Read(Data::ByteArray(buff, sizeof(buff)))) != 0)
	{
		sb.AppendC(buff, readSize);
	}
	return FromData(sb.ToCString());
}

Optional<Net::Google::GoogleServiceAccount> Net::Google::GoogleServiceAccount::FromData(Text::CStringNN fileData)
{
	NN<Text::JSONBase> json;
	if (!Text::JSONBase::ParseJSONStr(fileData).SetTo(json))
		return nullptr;
	Optional<GoogleServiceAccount> ret = nullptr;
	NN<Text::String> type;
	NN<Text::String> projectId;
	NN<Text::String> privateKeyId;
	NN<Text::String> privateKey;
	NN<Text::String> clientEmail;
	NN<Text::String> clientId;
	NN<Text::String> authUri;
	NN<Text::String> tokenUri;
	NN<Text::String> authProviderX509CertUrl;
	NN<Text::String> clientX509CertUrl;
	NN<Text::String> universeDomain;
	if (json->GetValueString(CSTR("type")).SetTo(type) &&
		json->GetValueString(CSTR("project_id")).SetTo(projectId) &&
		json->GetValueString(CSTR("private_key_id")).SetTo(privateKeyId) &&
		json->GetValueString(CSTR("private_key")).SetTo(privateKey) &&
		json->GetValueString(CSTR("client_email")).SetTo(clientEmail) &&
		json->GetValueString(CSTR("client_id")).SetTo(clientId) &&
		json->GetValueString(CSTR("auth_uri")).SetTo(authUri) &&
		json->GetValueString(CSTR("token_uri")).SetTo(tokenUri) &&
		json->GetValueString(CSTR("auth_provider_x509_cert_url")).SetTo(authProviderX509CertUrl) &&
		json->GetValueString(CSTR("client_x509_cert_url")).SetTo(clientX509CertUrl) &&
		json->GetValueString(CSTR("universe_domain")).SetTo(universeDomain) &&
		type->Equals(CSTR("service_account")))
	{
		NN<Crypto::Cert::X509PrivKey> key;
		NN<Text::String> fileName = Text::String::New(CSTR("PrivateKey.key"));
		if (Optional<Crypto::Cert::X509PrivKey>::ConvertFrom(Parser::FileParser::X509Parser::ParseBuff(privateKey->ToByteArray(), fileName)).SetTo(key))
		{
			NEW_CLASSOPT(ret, GoogleServiceAccount(projectId->Clone(),
				privateKeyId->Clone(),
				key,
				clientEmail->Clone(),
				clientId->Clone(),
				authUri->Clone(),
				tokenUri->Clone(),
				authProviderX509CertUrl->Clone(),
				clientX509CertUrl->Clone(),
				universeDomain->Clone()));
		}
		fileName->Release();
	}
	json->EndUse();
	return ret;
}
