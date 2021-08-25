#ifndef _SM_NET_TRACEROUTE
#define _SM_NET_TRACEROUTE
#include "Net/SocketFactory.h"
#include "Sync/Event.h"

namespace Net
{
	class TraceRoute
	{
	private:
		Net::SocketFactory *sockf;
		Socket *socV4;
		Bool threadRunning;
		Bool threadToStop;

		UInt16 resId;
		UInt16 resSeq;
		Bool resFound;
		UInt32 resIP;
		Sync::Event *resEvt;

		static UInt32 __stdcall RecvThread(void *userObj);
		void ICMPChecksum(UInt8 *buff, UOSInt buffSize);
	public:
		TraceRoute(Net::SocketFactory *sockf, UInt32 ip);
		~TraceRoute();

		Bool IsError();
		Bool Tracev4(UInt32 ip, Data::ArrayList<UInt32> *ipList);
	};
}
#endif
