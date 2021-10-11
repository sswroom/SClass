#include "Stdafx.h"
#include "IO/DebugWriter.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"
#include "Win32/WindowsBTScanner.h"
#include "Win32/WinRTCore.h"
#include "Data/ByteTool.h"
#if _MSC_VER >= 1929
#include <winrt/Windows.Foundation.h>
#endif
#include <sdkddkver.h>
#include <windows.h>
#include <bluetoothapis.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::Advertisement;

void Win32::WindowsBTScanner::ReceivedHandler(winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher const& sender,
	winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs const& args)
{
	IO::BTScanLog::AddressType addrType;
	Int8 txPower;
#if _MSC_VER >= 1920
	if (args.BluetoothAddressType() == BluetoothAddressType::Public)
	{
		addrType = IO::BTScanLog::AT_PUBLIC;
	}
	else if (args.BluetoothAddressType() == BluetoothAddressType::Random)
	{
		addrType = IO::BTScanLog::AT_RANDOM;
	}
	else
	{
		addrType = IO::BTScanLog::AT_UNKNOWN;
	}
#if _MSC_VER >= 1929
	txPower = (Int8)args.TransmitPowerLevelInDBm().Value();
#else
	txPower = (Int8)args.TransmitPowerLevelInDBm();
#endif
#else
	addrType = IO::BTScanLog::AT_UNKNOWN;
	txPower = 0;
#endif

	IO::BTScanLog::ScanRecord3 *rec = this->DeviceGet(args.BluetoothAddress(), addrType);
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	rec->lastSeenTime = dt.ToTicks();
	rec->rssi = (Int8)args.RawSignalStrengthInDBm();
	rec->inRange = true;
	rec->addrType = addrType;
	rec->txPower = txPower;
	rec->advType = IO::BTScanLog::ADVT_UNKNOWN;
	rec->company = 0;

	hstring hstr = args.Advertisement().LocalName();
	const WChar *wptr = hstr.c_str();
	if (wptr[0] != 0)
	{
		const UTF8Char *sptr = Text::StrToUTF8New(wptr);
		if (rec->name == 0 || !Text::StrEquals(sptr, rec->name))
		{
			SDEL_TEXT(rec->name);
			rec->name = sptr;
			if (this->recHdlr)
				this->recHdlr(rec, IO::BTScanner::UT_NAME, this->recHdlrObj);
		}
		else
		{
			Text::StrDelNew(sptr);
		}
	}
	if (this->recHdlr)
		this->recHdlr(rec, IO::BTScanner::UT_RSSI, this->recHdlrObj);
}

void Win32::WindowsBTScanner::StoppedHandler(winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher const &sender,
	winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcherStoppedEventArgs const &args)
{
}
/*public: __cdecl winrt::Windows::Foundation::TypedEventHandler<
	struct winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher, struct winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs>::
		TypedEventHandler<struct winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher, struct winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs>
			<class Win32::WindowsBTScanner, void(__cdecl Win32::WindowsBTScanner::*)(struct winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher const&, struct winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs const&)>
				(class Win32::WindowsBTScanner*, void(__cdecl Win32::WindowsBTScanner::*)(struct winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher const&, struct winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs const&))*/
IO::BTScanLog::ScanRecord3 *Win32::WindowsBTScanner::DeviceGet(UInt64 mac, IO::BTScanLog::AddressType addrType)
{
	Sync::MutexUsage mutUsage(this->devMut);
	IO::BTScanLog::ScanRecord3 *rec;
	if (addrType == IO::BTScanLog::AT_RANDOM)
	{
		rec = this->randDevMap->Get(mac);
	}
	else
	{
		rec = this->pubDevMap->Get(mac);
	}
	if (rec)
	{
		return rec;
	}
	rec = MemAlloc(IO::BTScanLog::ScanRecord3, 1);
	MemClear(rec, sizeof(IO::BTScanLog::ScanRecord3));
	UInt8 buff[8];
	rec->macInt = mac;
	WriteMUInt64(buff, mac);
	rec->mac[0] = buff[2];
	rec->mac[1] = buff[3];
	rec->mac[2] = buff[4];
	rec->mac[3] = buff[5];
	rec->mac[4] = buff[6];
	rec->mac[5] = buff[7];
	rec->radioType = IO::BTScanLog::RT_LE;
	rec->company = 0;
	rec->addrType = addrType;
	if (addrType == IO::BTScanLog::AT_RANDOM)
	{
		this->randDevMap->Put(mac, rec);
	}
	else
	{
		this->pubDevMap->Put(mac, rec);
	}
	return rec;
}

void Win32::WindowsBTScanner::DeviceFree(IO::BTScanLog::ScanRecord3 *rec)
{
	SDEL_TEXT(rec->name);
	MemFree(rec);
}

#if true
struct LE_SCAN_REQUEST
{
	DWORD unknown1;
	DWORD scanType;
	DWORD unknown2;
	WORD scanInterval;
	WORD scanWindow;
	DWORD unknown3[2];
};
#else
struct LE_SCAN_REQUEST
{
	DWORD scanType;
	WORD scanInterval;
	WORD scanWindow;
};
#endif

UInt32 __stdcall Win32::WindowsBTScanner::ScanThread(void *userObj)
{
	Win32::WindowsBTScanner *me = (Win32::WindowsBTScanner*)userObj;
	BLUETOOTH_FIND_RADIO_PARAMS param;
	param.dwSize = sizeof(param);
	HANDLE handle;
	HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio(&param, &handle);
	if (hFind == 0)
	{
		printf("No bluetooth radio found: %X\r\n", GetLastError());
		return 0;
	}
	me->handle = handle;
	BluetoothFindRadioClose(hFind);
	me->threadRunning = true;
	LE_SCAN_REQUEST req;
	MemClear(&req, sizeof(req));
	req.scanType = 0;
	req.scanInterval = 29;
	req.scanWindow = 29;
	DWORD outSize;
	printf("Begin Scan request, size = %d, Handle = %x\r\n", (UInt32)sizeof(req), (UInt32)(UOSInt)handle);
	if (DeviceIoControl(handle, 0x41118c, &req, sizeof(req), 0, 0, &outSize, 0) == 0)
	{
		printf("Error: %x\r\n", GetLastError());
	}
	printf("End Scan request\r\n");
	me->threadRunning = false;
	CloseHandle(handle);
	return 0;
}

Bool Win32::WindowsBTScanner::BeginScan()
{
	BLUETOOTH_FIND_RADIO_PARAMS param;
	param.dwSize = sizeof(param);
	HANDLE handle;
	HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio(&param, &handle);
	this->handle = 0;
	if (hFind == 0)
	{
		printf("No bluetooth radio found: %X\r\n", GetLastError());
		return false;
	}
	this->handle = handle;
	LE_SCAN_REQUEST req;
	MemClear(&req, sizeof(req));
	req.scanType = 0;
	req.scanInterval = 29;
	req.scanWindow = 29;
	BluetoothFindRadioClose(hFind);
	printf("Begin Scan request, size = %d, Handle = %x\r\n", (UInt32)sizeof(req), (UInt32)(UOSInt)handle);
	MemClear(&this->overlapped, sizeof(OVERLAPPED));
	this->overlapped.hEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (DeviceIoControl(handle, 0x41118c, &req, sizeof(req), 0, 0, 0, &this->overlapped) == 0)
	{
		printf("Error: %x\r\n", GetLastError());
		CloseHandle(handle);
		this->handle = 0;
		CloseHandle(this->overlapped.hEvent);
		return false;
	}
	printf("End Scan request\r\n");
	return true;
}

void Win32::WindowsBTScanner::EndScan()
{
	if (this->handle)
	{
		CancelIo(this->handle);
		CloseHandle(handle);
		this->handle = 0;
		CloseHandle(this->overlapped.hEvent);
	}
}

Win32::WindowsBTScanner::WindowsBTScanner()
{
	Win32::WinRTCore::Init();
	this->recHdlr = 0;
	this->recHdlrObj = 0;
	this->threadRunning = false;
	this->handle = 0;
	NEW_CLASS(this->devMut, Sync::Mutex());
	NEW_CLASS(this->pubDevMap, Data::UInt64Map<IO::BTScanLog::ScanRecord3*>());
	NEW_CLASS(this->randDevMap, Data::UInt64Map<IO::BTScanLog::ScanRecord3*>());

	this->watcher = BluetoothLEAdvertisementWatcher();
	this->watcher.ScanningMode(BluetoothLEScanningMode::Active);
	this->watcher.Received({ this, &Win32::WindowsBTScanner::ReceivedHandler });
	this->watcher.Stopped({ this, &Win32::WindowsBTScanner::StoppedHandler });
}

Win32::WindowsBTScanner::~WindowsBTScanner()
{
	this->Close();
	Data::ArrayList<IO::BTScanLog::ScanRecord3*> *devList = this->pubDevMap->GetValues();
	LIST_FREE_FUNC(devList, DeviceFree);
	devList = this->randDevMap->GetValues();
	LIST_FREE_FUNC(devList, DeviceFree);
	DEL_CLASS(this->pubDevMap);
	DEL_CLASS(this->randDevMap);
	DEL_CLASS(this->devMut);
}

void Win32::WindowsBTScanner::HandleRecordUpdate(RecordHandler hdlr, void *userObj)
{
	this->recHdlrObj = userObj;
	this->recHdlr = hdlr;
}

Bool Win32::WindowsBTScanner::IsScanOn()
{
	return this->threadRunning || this->watcher.Status() == BluetoothLEAdvertisementWatcherStatus::Started;
}

void Win32::WindowsBTScanner::ScanOn()
{
	this->watcher.Start();
//	this->BeginScan();
	if (!this->threadRunning)
	{
		Sync::Thread::Create(ScanThread, this);
	}
}

void Win32::WindowsBTScanner::ScanOff()
{
	if (this->threadRunning)
	{
		CancelIo(this->handle);
//		CloseHandle(this->handle);
	}
//	this->EndScan();
	this->watcher.Stop();
}

void Win32::WindowsBTScanner::Close()
{
	if (this->IsScanOn())
	{
		this->ScanOff();
	}
}

Bool Win32::WindowsBTScanner::SetScanMode(ScanMode scanMode)
{
	switch (scanMode)
	{
	case SM_ACTIVE:
		this->watcher.ScanningMode(BluetoothLEScanningMode::Active);
		return true;
	case SM_PASSIVE:
		this->watcher.ScanningMode(BluetoothLEScanningMode::Passive);
		return true;
	}
	return false;
}

Data::UInt64Map<IO::BTScanLog::ScanRecord3*> *Win32::WindowsBTScanner::GetPublicMap(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->devMut);
	return this->pubDevMap;
}

Data::UInt64Map<IO::BTScanLog::ScanRecord3*> *Win32::WindowsBTScanner::GetRandomMap(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->devMut);
	return this->randDevMap;
}
