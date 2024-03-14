#ifndef _SM_NET_SOCKETMONITOR
#define _SM_NET_SOCKETMONITOR
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "Data/DateTime.h"
#include "Sync/Event.h"
#include "Net/SocketFactory.h"
#include "Sync/Thread.h"

namespace Net
{
	class SocketMonitor
	{
	public:
		typedef void (__stdcall *RAWDataHdlr)(AnyType userData, const UInt8 *packetData, UOSInt packetSize);

	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Socket *soc;
		Data::CallbackStorage<RAWDataHdlr> hdlr;

		Sync::Thread **threads;
		UOSInt threadCnt;

	private:
		static void __stdcall DataThread(NotNullPtr<Sync::Thread> thread);

	public:
		SocketMonitor(NotNullPtr<Net::SocketFactory> sockf, Socket *soc, RAWDataHdlr hdlr, AnyType userData, UOSInt workerCnt);
		~SocketMonitor();
	};
}
#endif
