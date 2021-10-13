#include "Stdafx.h"
#include "Exporter/PEMExporter.h"
#include "IO/FileStream.h"
#include "Net/ACMEConn.h"
#include "Net/DefaultSSLEngine.h"
#include "Net/HTTPClient.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/JSON.h"


/*
https://datatracker.ietf.org/doc/html/rfc8555

Testing:
https://acme-staging-v02.api.letsencrypt.org/directory

Production:
https://acme-v02.api.letsencrypt.org/directory
*/

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
					if (json->GetJSType() == Text::JSONBase::JST_OBJECT)
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
						if (metaBase && metaBase->GetJSType() == Text::JSONBase::JST_OBJECT)
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
