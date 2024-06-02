#ifndef _SM_NET_WIFICAPTURER
#define _SM_NET_WIFICAPTURER
#include "AnyType.h"
#include "Data/ArrayListNN.h"
#include "Net/WiFiLogFile.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

namespace Net
{
	class WiFiCapturer
	{
	public:
		typedef void (__stdcall *UpdateHandler)(NN<Net::WirelessLAN::BSSInfo> bss, const Data::Timestamp &scanTime, AnyType userObj);
	private:
		Net::WirelessLAN wlan;
		Sync::Thread thread;
		Net::WiFiLogFile wifiLog;
		Sync::Mutex logMut;
		UnsafeArrayOpt<const UTF8Char> lastFileName;
		Optional<Net::WirelessLAN::Interface> interf;
		Data::Timestamp lastScanTime;
		UpdateHandler hdlr;
		AnyType hdlrObj;

		static void __stdcall ScanThread(NN<Sync::Thread> thread);
	public:
		WiFiCapturer();
		~WiFiCapturer();

		Bool IsError();
		Bool IsStarted();
		Data::Timestamp GetLastScanTime();

		Bool Start();
		void Stop();

		void StoreStatus();
		NN<Data::ArrayListNN<Net::WiFiLogFile::LogFileEntry>> GetLogList(NN<Sync::MutexUsage> mutUsage);
		void SetUpdateHandler(UpdateHandler hdlr, AnyType hdlrObj);
	};
}
#endif
