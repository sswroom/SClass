#ifndef _SM_NET_WIFICAPTURER
#define _SM_NET_WIFICAPTURER
#include "Net/WiFiLogFile.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

namespace Net
{
	class WiFiCapturer
	{
	public:
		typedef void (__stdcall *UpdateHandler)(Net::WirelessLAN::BSSInfo *bss, const Data::Timestamp &scanTime, void *userObj);
	private:
		Net::WirelessLAN wlan;
		Sync::Thread thread;
		Net::WiFiLogFile wifiLog;
		Sync::Mutex logMut;
		const UTF8Char *lastFileName;
		Net::WirelessLAN::Interface *interf;
		Data::Timestamp lastScanTime;
		UpdateHandler hdlr;
		void *hdlrObj;

		static void __stdcall ScanThread(NotNullPtr<Sync::Thread> thread);
	public:
		WiFiCapturer();
		~WiFiCapturer();

		Bool IsError();
		Bool IsStarted();
		Data::Timestamp GetLastScanTime();

		Bool Start();
		void Stop();

		void StoreStatus();
		NotNullPtr<Data::ArrayList<Net::WiFiLogFile::LogFileEntry*>> GetLogList(NotNullPtr<Sync::MutexUsage> mutUsage);
		void SetUpdateHandler(UpdateHandler hdlr, void *hdlrObj);
	};
}
#endif
