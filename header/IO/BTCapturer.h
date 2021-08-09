#ifndef _SM_IO_BTCAPTURER
#define _SM_IO_BTCAPTURER
#include "IO/BTScanner.h"
#include "Sync/Event.h"
#include "Sync/MutexUsage.h"

namespace IO
{
	class BTCapturer
	{
	private:
		IO::BTScanner *bt;
		const UTF8Char *lastFileName;
		Bool threadRunning;
		Bool threadToStop;
		Sync::Event *threadEvt;

		static UInt32 __stdcall CheckThread(void *userObj);
	public:
		BTCapturer();
		~BTCapturer();

		Bool IsError();
		Bool IsStarted();

		Bool Start();
		void Stop();

		void StoreStatus();
		Data::ArrayList<IO::BTScanLog::ScanRecord2*> *GetPublicList(Sync::MutexUsage *mutUsage);
		Data::ArrayList<IO::BTScanLog::ScanRecord2*> *GetRandomList(Sync::MutexUsage *mutUsage);
		void SetUpdateHandler(IO::BTScanner::RecordHandler hdlr, void *userObj);
	};
}
#endif
