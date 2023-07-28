#ifndef _SM_WIN32_WINDOWSBTSCANNER
#define _SM_WIN32_WINDOWSBTSCANNER
#include "IO/BTScanner.h"
#include "Sync/Event.h"

#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>
#include <windows.h>

namespace Win32
{
	class WindowsBTScanner : public IO::BTScanner
	{
	private:
		winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher watcher = nullptr;
		Data::FastMap<UInt64, IO::BTScanLog::ScanRecord3*> pubDevMap;
		Data::FastMap<UInt64, IO::BTScanLog::ScanRecord3*> randDevMap;
		Sync::Mutex devMut;
		RecordHandler recHdlr;
		void *recHdlrObj;
		void *handle;
		Bool threadRunning;
		OVERLAPPED overlapped;

		void __cdecl ReceivedHandler(winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher const &sender,
			winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs const &args);
		void StoppedHandler(winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher const &sender,
			winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcherStoppedEventArgs const &args);

		IO::BTScanLog::ScanRecord3 *DeviceGet(UInt64 mac, IO::BTScanLog::AddressType addrType);
		void DeviceFree(IO::BTScanLog::ScanRecord3 *rec);
		static UInt32 __stdcall ScanThread(void *userObj);
		Bool BeginScan();
		void EndScan();
	public:
		WindowsBTScanner();
		virtual ~WindowsBTScanner();

		virtual void HandleRecordUpdate(RecordHandler hdlr, void *userObj);

		virtual Bool IsScanOn();
		virtual void ScanOn();
		virtual void ScanOff();
		virtual void Close();
		virtual Bool SetScanMode(ScanMode scanMode);

		virtual Data::FastMap<UInt64, IO::BTScanLog::ScanRecord3*> *GetPublicMap(NotNullPtr<Sync::MutexUsage> mutUsage);
		virtual Data::FastMap<UInt64, IO::BTScanLog::ScanRecord3*> *GetRandomMap(NotNullPtr<Sync::MutexUsage> mutUsage);
	};
}
#endif
