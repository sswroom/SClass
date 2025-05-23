#include "Stdafx.h"
#include "IO/Path.h"
#include "Net/ACMEClient.h"

Net::ACMEClient::ACMEClient(NN<Net::TCPClientFactory> clif, Text::CStringNN serverHost, UInt16 port, Text::CStringNN keyFile)
{
	NEW_CLASSNN(this->acme, Net::ACMEConn(clif, serverHost, port));
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
	this->acme.Delete();
}

Bool Net::ACMEClient::IsError()
{
	return this->acme->IsError() || !this->keyReady || !this->accReady;
}

Optional<Text::String> Net::ACMEClient::GetTermOfService()
{
	return this->acme->GetTermOfService();
}

Optional<Text::String> Net::ACMEClient::GetWebsite()
{
	return this->acme->GetWebsite();
}

Optional<Text::String> Net::ACMEClient::GetAccountId()
{
	return this->acme->GetAccountId();
}

NN<Net::ACMEConn> Net::ACMEClient::GetConn()
{
	return this->acme;
}
