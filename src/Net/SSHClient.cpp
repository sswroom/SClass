#include "Stdafx.h"
#include "Net/SSHClient.h"
#include "Sync/MutexUsage.h"
#include <stdio.h>

void __stdcall Net::SSHClient::EventThread(NN<Sync::Thread> thread)
{
	NN<Net::SSHClient> me = thread->GetUserObj().GetNN<Net::SSHClient>();
	NN<Net::TCPClient> cli;
	if (me->conn->GetTCPClient().SetTo(cli))
	{
		while (!thread->IsStopping())
		{
			if (me->HasChannels())
			{
				if (cli->Wait(1000) && !thread->IsStopping())
				{
					Sync::MutexUsage mutUsage(me->mut);
					Data::ArrayIterator<NN<SSHForwarder>> it = me->fwd.Iterator();
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

Net::SSHClient::SSHClient(NN<Net::SSHConn> conn) : thread(EventThread, this, CSTR("SSHClient"))
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
		return nullptr;
	NN<Net::SSHForwarder> fwd;
	NEW_CLASSNN(fwd, Net::SSHForwarder(this->conn, localPort, remoteHost, remotePort));
	if (fwd->IsError())
	{
		fwd.Delete();
		return nullptr;
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
	Data::ArrayIterator<NN<SSHForwarder>> it = this->fwd.Iterator();
	while (it.HasNext())
		if (it.Next()->HasChannels())
			return true;
	return false;
}
