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
		Sync::Event threadEvt;
		Bool autoStore;

		static UInt32 __stdcall CheckThread(void *userObj);
	public:
		BTCapturer(Bool autoStore);
		~BTCapturer();

		Bool IsError();
		Bool IsStarted();

		Bool Start();
		void Stop();

		void StoreStatus();
		NotNullPtr<const Data::ReadingList<IO::BTScanLog::ScanRecord3*>> GetPublicList(NotNullPtr<Sync::MutexUsage> mutUsage) const;
		NotNullPtr<const Data::ReadingList<IO::BTScanLog::ScanRecord3*>> GetRandomList(NotNullPtr<Sync::MutexUsage> mutUsage) const;
		void SetUpdateHandler(IO::BTScanner::RecordHandler hdlr, void *userObj);
	};
}
#endif
