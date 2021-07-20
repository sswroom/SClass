#ifndef _SM_IO_BTCAPTURER
#define _SM_IO_BTCAPTURER
#include "IO/ProgCtrl/BluetoothCtlProgCtrl.h"
#include "Sync/Event.h"

namespace IO
{
	class BTCapturer
	{
	private:
		struct ClassData;

		struct ClassData *clsData;
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
		Data::ArrayList<IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo*> *GetLogList(Sync::MutexUsage *mutUsage);
		void SetUpdateHandler(IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceHandler hdlr, void *userObj);
	};
}
#endif
