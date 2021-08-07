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
		Data::UInt64Map<ScanRecord2*> *devMap;
		Sync::Mutex *devMut;
		RecordHandler recHdlr;
		void *recHdlrObj;


		void ReceivedHandler(winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher const &sender,
			winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs const &args);
		void StoppedHandler(winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher const &sender,
			winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcherStoppedEventArgs const &args);
		void DevCompleted(winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Devices::Bluetooth::BluetoothLEDevice> asyncInfo,
			winrt::Windows::Foundation::AsyncStatus asyncStatus);

		ScanRecord2 *DeviceGet(UInt64 mac);
		void DeviceFree(ScanRecord2 *rec);
	public:
		WindowsBTScanner();
		virtual ~WindowsBTScanner();

		virtual void HandleRecordUpdate(RecordHandler hdlr, void *userObj);

		virtual Bool IsScanOn();
		virtual void ScanOn();
		virtual void ScanOff();
		virtual void Close();
		virtual Bool SetScanMode(ScanMode scanMode);

		virtual Data::UInt64Map<ScanRecord2*> *GetRecordMap(Sync::MutexUsage *mutUsage);
	};
}
#endif
