#include "Stdafx.h"
#include "IO/Path.h"
#include "Net/ACMEClient.h"

Net::ACMEClient::ACMEClient(Net::SocketFactory *sockf, const UTF8Char *serverHost, UInt16 port, const UTF8Char *keyFile)
{
	NEW_CLASS(this->acme, Net::ACMEConn(sockf, serverHost, port));
	this->keyReady = false;
	this->accReady = false;
	if (!this->acme->IsError())
	{
		if (IO::Path::GetPathType(keyFile) == IO::Path::PathType::File)
		{
			this->keyReady = this->acme->LoadKey(keyFile);
			if (this->keyReady)
			{
				this->accReady = this->acme->NewNonce() && this->acme->AccountRetr();
			}
		}
		else
		{
			this->keyReady = (this->acme->NewKey() && this->acme->SaveKey(keyFile));
			if (this->keyReady)
			{
				this->accReady = this->acme->NewNonce() && this->acme->AccountNew();
			}
		}
	}
}

Net::ACMEClient::~ACMEClient()
{
	DEL_CLASS(this->acme);
}

Bool Net::ACMEClient::IsError()
{
	return this->acme->IsError() || !this->keyReady || !this->accReady;
}

const UTF8Char *Net::ACMEClient::GetTermOfService()
{
	return this->acme->GetTermOfService();
}

const UTF8Char *Net::ACMEClient::GetWebsite()
{
	return this->acme->GetWebsite();
}

const UTF8Char *Net::ACMEClient::GetAccountId()
{
	return this->acme->GetAccountId();
}

Net::ACMEConn *Net::ACMEClient::GetConn()
{
	return this->acme;
}