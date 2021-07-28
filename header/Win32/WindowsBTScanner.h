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
		winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher watcher;
		Data::UInt64Map<ScanRecord*> *devMap;
		Sync::Mutex *devMut;
		RecordHandler recHdlr;
		void *recHdlrObj;


		void ReceivedHandler(winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher const &sender,
			winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs const &args);
		void StoppedHandler(winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher const &sender,
			winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcherStoppedEventArgs const &args);
		void DevCompleted(winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Devices::Bluetooth::BluetoothLEDevice> asyncInfo,
			winrt::Windows::Foundation::AsyncStatus asyncStatus);

		ScanRecord *DeviceGet(UInt64 mac);
		void DeviceFree(ScanRecord *rec);
	public:
		WindowsBTScanner();
		virtual ~WindowsBTScanner();

		virtual void HandleRecordUpdate(RecordHandler hdlr, void *userObj);

		virtual Bool IsScanOn();
		virtual void ScanOn();
		virtual void ScanOff();
		virtual void Close();

		virtual Data::UInt64Map<ScanRecord*> *GetRecordMap(Sync::MutexUsage *mutUsage);
	};
}
#endif
