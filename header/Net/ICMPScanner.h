#ifndef _SM_NET_ICMPSCANNER
#define _SM_NET_ICMPSCANNER
#include "AnyType.h"
#include "Data/FastMap.h"
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
		NN<Net::SocketFactory> sockf;
		Sync::Mutex resultMut;
		Data::FastMap<UInt32, ScanResult*> results;
		Manage::HiResClock *clk;
		Socket *soc;
		Bool threadRunning;
		Bool threadToStop;

	private:
		static void __stdcall ICMPChecksum(UInt8 *buff, OSInt buffSize);
		static UInt32 __stdcall Ping1Thread(AnyType userObj);
		static UInt32 __stdcall Ping2Thread(AnyType userObj);

		void AppendMACs(UInt32 ip);
	public:
		ICMPScanner(NN<Net::SocketFactory> sockf);
		~ICMPScanner();

		Bool Scan(UInt32 ip);
		const Data::ReadingList<ScanResult*> *GetResults() const;
		void ClearResults();
	};
}
#endif
