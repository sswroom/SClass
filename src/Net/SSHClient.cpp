#include "Stdafx.h"
#include "Net/SSHClient.h"
#include "Sync/MutexUsage.h"
#include <stdio.h>

void __stdcall Net::SSHClient::EventThread(NotNullPtr<Sync::Thread> thread)
{
	NotNullPtr<Net::SSHClient> me = thread->GetUserObj().GetNN<Net::SSHClient>();
	NotNullPtr<Net::TCPClient> cli;
	if (me->conn->GetTCPClient().SetTo(cli))
	{
		while (!thread->IsStopping())
		{
			if (me->HasChannels())
			{
				if (cli->Wait(1000) && !thread->IsStopping())
				{
					Sync::MutexUsage mutUsage(me->mut);
					Data::ArrayIterator<NotNullPtr<SSHForwarder>> it = me->fwd.Iterator();
					while (it.HasNext())
					{
						it.Next()->DoEvents();
					}
				}
			}
			else
			{
				thread->Wait(1000);
			}
		}
	}
}

Net::SSHClient::SSHClient(NotNullPtr<Net::SSHConn> conn) : thread(EventThread, this, CSTR("SSHClient"))
{
	this->conn = conn;
	if (!this->conn->IsError())
		this->thread.Start();
}

Net::SSHClient::~SSHClient()
{
	this->thread.BeginStop();

	this->fwd.DeleteAll();
	this->conn.Delete();
}

Optional<Net::SSHForwarder> Net::SSHClient::CreateForward(UInt16 localPort, Text::CStringNN remoteHost, UInt16 remotePort)
{
	if (remotePort == 0)
		return 0;
	NotNullPtr<Net::SSHForwarder> fwd;
	NEW_CLASSNN(fwd, Net::SSHForwarder(this->conn, localPort, remoteHost, remotePort));
	if (fwd->IsError())
	{
		fwd.Delete();
		return 0;
	}
	this->fwd.Add(fwd);
	return fwd;
}

Bool Net::SSHClient::IsError() const
{
	return this->conn->IsError();
}

Bool Net::SSHClient::HasChannels() const
{
	Data::ArrayIterator<NotNullPtr<SSHForwarder>> it = this->fwd.Iterator();
	while (it.HasNext())
		if (it.Next()->HasChannels())
			return true;
	return false;
}
