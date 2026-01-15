#ifndef _SM_NET_ARPHANDLER
#define _SM_NET_ARPHANDLER
#include "AnyType.h"
#include "Net/SocketFactory.h"
#include "Sync/Event.h"

namespace Net
{
	class ARPHandler
	{
	public:
		typedef void (CALLBACKFUNC ARPResponseHdlr)(UnsafeArray<const UInt8> hwAddr, UInt32 ipv4, AnyType userData);

		typedef struct
		{
			ARPHandler *me;
			Bool threadRunning;
			Sync::Event *evt;
			Bool toStop;
		} ThreadStat;

	private:
		NN<Net::SocketFactory> sockf;
		UnsafeArray<const UTF8Char> ifName;
		UInt8 hwAddr[6];
		UInt32 ipAddr;
		Optional<Socket> soc;
		ARPResponseHdlr hdlr;
		AnyType userData;

		ThreadStat *threadStats;
		UIntOS threadCnt;
		Sync::Event *ctrlEvt;


	private:
		static UInt32 __stdcall DataThread(AnyType obj);

	public:
		ARPHandler(NN<Net::SocketFactory> sockf, UnsafeArray<const UTF8Char> ifName, const UInt8 *hwAddr, UInt32 adapterIP, ARPResponseHdlr hdlr, AnyType userData, UIntOS workerCnt);
		~ARPHandler();

		Bool IsError();
		Bool MakeRequest(UInt32 targetIP);
	};
}
#endif
