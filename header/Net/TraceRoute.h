#ifndef _SM_NET_TRACEROUTE
#define _SM_NET_TRACEROUTE
#include "Net/SocketFactory.h"
#include "Sync/Event.h"
#include "Sync/Thread.h"

namespace Net
{
	class TraceRoute
	{
	private:
		NN<Net::SocketFactory> sockf;
		Socket *socV4;
		Sync::Thread thread;

		UInt16 resId;
		UInt16 resSeq;
		Bool resFound;
		UInt32 resIP;
		Sync::Event *resEvt;

		static void __stdcall RecvThread(NN<Sync::Thread> thread);
		void ICMPChecksum(UInt8 *buff, UOSInt buffSize);
	public:
		TraceRoute(NN<Net::SocketFactory> sockf, UInt32 ip);
		~TraceRoute();

		Bool IsError();
		Bool Tracev4(UInt32 ip, Data::ArrayList<UInt32> *ipList);
	};
}
#endif
