#ifndef _SM_NET_IPSCANDETECTOR
#define _SM_NET_IPSCANDETECTOR
#include "AnyType.h"
#include "Data/FastMap.hpp"
#include "Data/FastMapNN.hpp"
#include "Net/SocketFactory.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Net
{
	class IPScanDetector
	{
	public:
		typedef void (CALLBACKFUNC IPScanHandler)(UnsafeArray<const UInt8> hwAddr, AnyType userData);

		typedef struct
		{
			IPScanDetector *me;
			Bool threadRunning;
			Sync::Event *evt;
			Bool toStop;
		} ThreadStat;

		struct AdapterStatus
		{
			UInt64 iMAC;
			Int64 lastDetectTime;
			OSInt detectCnt;
			Data::FastMap<UInt32, Int64> targetIPMap;
		};

	private:
		NN<Net::SocketFactory> sockf;
		Optional<Socket> soc;
		IPScanHandler hdlr;
		AnyType userData;
		Data::FastMapNN<UInt64, AdapterStatus> adapterMap;
		Sync::Mutex adapterMut;

		ThreadStat *threadStats;
		UOSInt threadCnt;
		Sync::Event *ctrlEvt;

	private:
		static UInt32 __stdcall DataThread(AnyType obj);

	public:
		IPScanDetector(NN<Net::SocketFactory> sockf, IPScanHandler hdlr, AnyType userData, UOSInt workerCnt);
		~IPScanDetector();

		Bool IsError();
	};
}
#endif
