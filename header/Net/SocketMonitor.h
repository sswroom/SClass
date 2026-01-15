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
		typedef void (CALLBACKFUNC RAWDataHdlr)(AnyType userData, UnsafeArray<const UInt8> packetData, UIntOS packetSize);

	private:
		NN<Net::SocketFactory> sockf;
		NN<Socket> soc;
		Data::CallbackStorage<RAWDataHdlr> hdlr;

		Sync::Thread **threads;
		UIntOS threadCnt;

	private:
		static void __stdcall DataThread(NN<Sync::Thread> thread);

	public:
		SocketMonitor(NN<Net::SocketFactory> sockf, NN<Socket> soc, RAWDataHdlr hdlr, AnyType userData, UIntOS workerCnt);
		~SocketMonitor();
	};
}
#endif
