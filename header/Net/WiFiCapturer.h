#ifndef _SM_NET_WIFICAPTURER
#define _SM_NET_WIFICAPTURER
#include "Net/WiFiLogFile.h"
#include "Sync/MutexUsage.h"

namespace Net
{
	class WiFiCapturer
	{
	private:
		Net::WirelessLAN *wlan;
		Bool threadRunning;
		Bool threadToStop;
		Net::WiFiLogFile *wifiLog;
		Sync::Mutex *logMut;
		const UTF8Char *lastFileName;
		Net::WirelessLAN::Interface *interf;
		Int64 lastScanTimeTicks;

		static UInt32 __stdcall ScanThread(void *userObj);
	public:
		WiFiCapturer();
		~WiFiCapturer();

		Bool IsError();
		Bool IsStarted();
		Int64 GetLastScanTimeTicks();

		Bool Start();
		void Stop();

		void StoreStatus();
		Data::ArrayList<Net::WiFiLogFile::LogFileEntry*> *GetLogList(Sync::MutexUsage *mutUsage);
	};
}
#endif
