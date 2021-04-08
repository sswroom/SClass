#ifndef _SM_NET_IPSCANDETECTOR
#define _SM_NET_IPSCANDETECTOR
#include "Data/Int32Map.h"
#include "Data/Int64Map.h"
#include "Net/SocketFactory.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Net
{
	class IPScanDetector
	{
	public:
		typedef void (__stdcall *IPScanHandler)(const UInt8 *hwAddr, void *userData);

		typedef struct
		{
			IPScanDetector *me;
			Bool threadRunning;
			Sync::Event *evt;
			Bool toStop;
		} ThreadStat;

		typedef struct
		{
			Int64 iMAC;
			Int64 lastDetectTime;
			OSInt detectCnt;
			Data::Int32Map<Int64> *targetIPMap;
		} AdapterStatus;

	private:
		Net::SocketFactory *sockf;
		UInt32 *soc;
		IPScanHandler hdlr;
		void *userData;
		Data::Int64Map<AdapterStatus*> *adapterMap;
		Sync::Mutex *adapterMut;

		ThreadStat *threadStats;
		OSInt threadCnt;
		Sync::Event *ctrlEvt;

	private:
		static UInt32 __stdcall DataThread(void *obj);

	public:
		IPScanDetector(Net::SocketFactory *sockf, IPScanHandler hdlr, void *userData, OSInt workerCnt);
		~IPScanDetector();

		Bool IsError();
	};
}
#endif
