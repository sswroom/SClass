#ifndef _SM_WIN32_WINDOWSBTSCANNER
#define _SM_WIN32_WINDOWSBTSCANNER
#include "IO/BTScanner.h"
#include "Sync/Event.h"

#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>

namespace Win32
{
	class WindowsBTScanner : public IO::BTScanner
	{
	private:
		winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher watcher = nullptr;
		Data::UInt64Map<IO::BTScanLog::ScanRecord*> *devMap;
		Sync::Mutex *devMut;
		RecordHandler recHdlr;
		void *recHdlrObj;


		void ReceivedHandler(winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher const &sender,
			winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs const &args);
		void StoppedHandler(winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher const &sender,
			winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcherStoppedEventArgs const &args);

		IO::BTScanLog::ScanRecord *DeviceGet(UInt64 mac);
		void DeviceFree(IO::BTScanLog::ScanRecord *rec);
	public:
		WindowsBTScanner();
		virtual ~WindowsBTScanner();

		virtual void HandleRecordUpdate(RecordHandler hdlr, void *userObj);

		virtual Bool IsScanOn();
		virtual void ScanOn();
		virtual void ScanOff();
		virtual void Close();
		virtual Bool SetScanMode(ScanMode scanMode);

		virtual Data::UInt64Map<IO::BTScanLog::ScanRecord*> *GetRecordMap(Sync::MutexUsage *mutUsage);
	};
}
#endif
