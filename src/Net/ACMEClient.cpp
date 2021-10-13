#include "Stdafx.h"
#include "Net/ACMEClient.h"

Net::ACMEClient::ACMEClient(Net::SocketFactory *sockf, const UTF8Char *serverHost, UInt16 port)
{
	NEW_CLASS(this->acme, Net::ACMEConn(sockf, serverHost, port));
}

Net::ACMEClient::~ACMEClient()
{
	DEL_CLASS(this->acme);
}

Bool Net::ACMEClient::IsError()
{
	return this->acme->IsError();
}

const UTF8Char *Net::ACMEClient::GetTermOfService()
{
	return this->acme->GetTermOfService();
}

const UTF8Char *Net::ACMEClient::GetWebsite()
{
	return this->acme->GetWebsite();
}
