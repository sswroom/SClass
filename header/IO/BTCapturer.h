#ifndef _SM_IO_BTCAPTURER
#define _SM_IO_BTCAPTURER
#include "IO/BTScanner.h"
#include "Sync/Event.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

namespace IO
{
	class BTCapturer
	{
	private:
		Optional<IO::BTScanner> bt;
		UnsafeArrayOpt<const UTF8Char> lastFileName;
		Sync::Thread thread;
		Bool autoStore;

		static void __stdcall CheckThread(NN<Sync::Thread> thread);
	public:
		BTCapturer(Bool autoStore);
		~BTCapturer();

		Bool IsError();
		Bool IsStarted();

		Bool Start();
		void Stop();

		void StoreStatus();
		NN<const Data::ReadingListNN<IO::BTScanLog::ScanRecord3>> GetPublicList(NN<Sync::MutexUsage> mutUsage) const;
		NN<const Data::ReadingListNN<IO::BTScanLog::ScanRecord3>> GetRandomList(NN<Sync::MutexUsage> mutUsage) const;
		void SetUpdateHandler(IO::BTScanner::RecordHandler hdlr, void *userObj);
	};
}
#endif
