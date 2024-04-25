#ifndef _SM_NET_SSHCLIENT
#define _SM_NET_SSHCLIENT
#include "Data/ArrayListNN.h"
#include "Net/SSHConn.h"
#include "Net/SSHForwarder.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"

namespace Net
{
	class SSHClient
	{
	private:
		NN<Net::SSHConn> conn;
		Data::ArrayListNN<Net::SSHForwarder> fwd;
		Sync::Thread thread;
		Sync::Mutex mut;

		static void __stdcall EventThread(NN<Sync::Thread> thread);
	public:
		SSHClient(NN<Net::SSHConn> conn);
		virtual ~SSHClient();

		Optional<Net::SSHForwarder> CreateForward(UInt16 localPort, Text::CStringNN remoteHost, UInt16 remotePort);
		Bool IsError() const;
		Bool HasChannels() const;
	};
}
#endif
