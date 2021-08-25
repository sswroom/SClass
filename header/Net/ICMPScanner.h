#ifndef _SM_NET_ICMPSCANNER
#define _SM_NET_ICMPSCANNER
#include "Data/UInt32Map.h"
#include "Manage/HiResClock.h"
#include "Net/SocketFactory.h"
#include "Sync/Mutex.h"

namespace Net
{
	class ICMPScanner
	{
	public:
		typedef struct
		{
			UInt32 ip;
			UInt8 mac[6];
			Double respTime;
		} ScanResult;

	private:
		Net::SocketFactory *sockf;
		Sync::Mutex *resultMut;
		Data::UInt32Map<ScanResult*> *results;
		Manage::HiResClock *clk;
		Socket *soc;
		Bool threadRunning;
		Bool threadToStop;

	private:
		static void __stdcall ICMPChecksum(UInt8 *buff, OSInt buffSize);
		static UInt32 __stdcall Ping1Thread(void *userObj);
		static UInt32 __stdcall Ping2Thread(void *userObj);

		void AppendMACs(UInt32 ip);
	public:
		ICMPScanner(Net::SocketFactory *sockf);
		~ICMPScanner();

		Bool Scan(UInt32 ip);
		Data::ArrayList<ScanResult*> *GetResults();
		void ClearResults();
	};
}
#endif
