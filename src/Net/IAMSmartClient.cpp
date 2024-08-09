#include "Stdafx.h"
#include "Net/IAMSmartClient.h"
#include "Parser/FileParser/X509Parser.h"

Bool Net::IAMSmartClient::PrepareCEK()
{
	Int64 currTime = Data::DateTimeUtil::GetCurrTimeMillis();
	if (currTime < cek.expiresAt)
	{
		return true;
	}
	NN<Crypto::Cert::X509PrivKey> key;
	if (!this->key.SetTo(key))
		return false;
	if (cek.issueAt != 0)
	{
		Net::IAMSmartAPI::FreeCEK(this->cek);
		this->cek.issueAt = 0;
	}
	if (!this->api.GetKey(key, cek))
		return false;
	return currTime < cek.expiresAt;
}

Net::IAMSmartClient::IAMSmartClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN domain, Text::CStringNN clientID, Text::CStringNN clientSecret, Text::CStringNN keyFile) : api(clif, ssl, domain, clientID, clientSecret)
{
	cek.issueAt = 0;
	cek.expiresAt = 0;
	this->key = 0;
	Parser::FileParser::X509Parser parser;
	NN<Crypto::Cert::X509File> file;
	if (Optional<Crypto::Cert::X509File>::ConvertFrom(parser.ParseFilePath(keyFile)).SetTo(file))
	{
		if (file->GetFileType() == Crypto::Cert::X509File::FileType::PrivateKey)
		{
			this->key = NN<Crypto::Cert::X509PrivKey>::ConvertFrom(file);
		}
		else if (file->GetFileType() == Crypto::Cert::X509File::FileType::Key)
		{
			this->key = Crypto::Cert::X509PrivKey::CreateFromKey(NN<Crypto::Cert::X509Key>::ConvertFrom(file));
			file.Delete();
		}
		else
		{
			file.Delete();
		}
	}
}

Net::IAMSmartClient::~IAMSmartClient()
{
	this->key.Delete();
}

Bool Net::IAMSmartClient::IsError() const
{
	return this->key.IsNull();
}

Bool Net::IAMSmartClient::GetToken(Text::CStringNN code, Bool directLogin, NN<IAMSmartAPI::TokenInfo> token)
{
	if (!this->PrepareCEK())
		return false;
	
	return this->api.GetToken(code, directLogin, this->cek, token);
}

Bool Net::IAMSmartClient::GetProfiles(NN<IAMSmartAPI::TokenInfo> token, Text::CStringNN eMEFields, Text::CStringNN profileFields, NN<IAMSmartAPI::ProfileInfo> profiles)
{
	if (!this->PrepareCEK())
		return false;
	
	return this->api.GetProfiles(token, eMEFields, profileFields, this->cek, profiles);
}
