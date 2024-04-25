#include "Stdafx.h"
#include "Net/SSHManager.h"

Net::SSHManager::SSHManager(NN<Net::SocketFactory> sockf)
{
	this->sockf = sockf;
	this->error = 1;
}

Net::SSHManager::~SSHManager()
{
}

Bool Net::SSHManager::IsError() const
{
	return true;
}

Optional<Net::SSHClient> Net::SSHManager::CreateClient(Text::CStringNN host, UInt16 port, Text::CStringNN userName, Text::CStringNN password)
{
	return 0;
}

Optional<Net::SSHConn> Net::SSHManager::CreateConn(Text::CStringNN host, UInt16 port, Data::Duration timeout)
{
	return 0;
}

Text::CStringNN Net::SSHManager::ErrorGetName(Int32 errorCode)
{
	switch (errorCode)
	{
	case 0:
		return CSTR("Success");
	default:
		return CSTR("Unknown");
	}
}
