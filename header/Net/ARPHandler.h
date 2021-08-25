#ifndef _SM_NET_ARPHANDLER
#define _SM_NET_ARPHANDLER
#include "Net/SocketFactory.h"
#include "Sync/Event.h"

namespace Net
{
	class ARPHandler
	{
	public:
		typedef void (__stdcall *ARPResponseHdlr)(const UInt8 *hwAddr, UInt32 ipv4, void *userData);

		typedef struct
		{
			ARPHandler *me;
			Bool threadRunning;
			Sync::Event *evt;
			Bool toStop;
		} ThreadStat;

	private:
		Net::SocketFactory *sockf;
		const UTF8Char *ifName;
		UInt8 hwAddr[6];
		UInt32 ipAddr;
		Socket *soc;
		ARPResponseHdlr hdlr;
		void *userData;

		ThreadStat *threadStats;
		UOSInt threadCnt;
		Sync::Event *ctrlEvt;


	private:
		static UInt32 __stdcall DataThread(void *obj);

	public:
		ARPHandler(Net::SocketFactory *sockf, const UTF8Char *ifName, const UInt8 *hwAddr, UInt32 adapterIP, ARPResponseHdlr hdlr, void *userData, UOSInt workerCnt);
		~ARPHandler();

		Bool IsError();
		Bool MakeRequest(UInt32 targetIP);
	};
}
#endif
