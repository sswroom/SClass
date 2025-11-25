#include "Stdafx.h"
#if _MSC_VER >= 1929
#include <winrt/Windows.Foundation.h>
#endif
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>
#include <sdkddkver.h>
#include <windows.h>
#include <bluetoothapis.h>

#include "IO/DebugWriter.h"
#include "IO/OS.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"
#include "Win32/WindowsBTScanner.h"
#include "Win32/WinRTCore.h"
#include "Core/ByteTool_C.h"

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
	if (IO::OS::GetBuildNumber() >= 19041)
	{
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
	}
	else
	{
		addrType = IO::BTScanLog::AT_UNKNOWN;
	}
#if _MSC_VER >= 1929
	if (IO::OS::GetBuildNumber() >= 19041 && args.TransmitPowerLevelInDBm())
	{
		txPower = (Int8)args.TransmitPowerLevelInDBm().Value();
	}
	else
	{
		txPower = 0;
	}
#else
	txPower = (Int8)args.TransmitPowerLevelInDBm();
#endif
#else
	addrType = IO::BTScanLog::AT_UNKNOWN;
	txPower = 0;
#endif

	NN<IO::BTScanLog::ScanRecord3> rec = this->DeviceGet(args.BluetoothAddress() << 16, addrType);
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
		NN<Text::String> s = Text::String::NewNotNull(wptr);
		NN<Text::String> name;
		if (!rec->name.SetTo(name) || !Text::StrEqualsC(s->v, s->leng, name->v, name->leng))
		{
			OPTSTR_DEL(rec->name);
			rec->name = s;
			if (this->recHdlr)
				this->recHdlr(rec, IO::BTScanner::UT_NAME, this->recHdlrObj);
		}
		else
		{
			s->Release();
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
NN<IO::BTScanLog::ScanRecord3> Win32::WindowsBTScanner::DeviceGet(UInt64 mac64, IO::BTScanLog::AddressType addrType)
{
	Sync::MutexUsage mutUsage(this->devMut);
	NN<IO::BTScanLog::ScanRecord3> rec;
	Optional<IO::BTScanLog::ScanRecord3> optrec;
	if (addrType == IO::BTScanLog::AT_RANDOM)
	{
		optrec = this->randDevMap.Get(mac64);
	}
	else
	{
		optrec = this->pubDevMap.Get(mac64);
	}
	if (optrec.SetTo(rec))
	{
		return rec;
	}
	rec = MemAllocNN(IO::BTScanLog::ScanRecord3);
	rec.ZeroContent();
	UInt8 buff[8];
	rec->mac64Int = mac64;
	WriteMUInt64(buff, mac64);
	rec->mac[0] = buff[0];
	rec->mac[1] = buff[1];
	rec->mac[2] = buff[2];
	rec->mac[3] = buff[3];
	rec->mac[4] = buff[4];
	rec->mac[5] = buff[5];
	rec->radioType = IO::BTScanLog::RT_LE;
	rec->company = 0;
	rec->addrType = addrType;
	if (addrType == IO::BTScanLog::AT_RANDOM)
	{
		this->randDevMap.Put(mac64, rec);
	}
	else
	{
		this->pubDevMap.Put(mac64, rec);
	}
	return rec;
}

void Win32::WindowsBTScanner::DeviceFree(NN<IO::BTScanLog::ScanRecord3> rec)
{
	OPTSTR_DEL(rec->name);
	MemFreeNN(rec);
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

UInt32 __stdcall Win32::WindowsBTScanner::ScanThread(AnyType userObj)
{
	NN<Win32::WindowsBTScanner> me = userObj.GetNN<Win32::WindowsBTScanner>();
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

	this->watcher = BluetoothLEAdvertisementWatcher();
	this->watcher.ScanningMode(BluetoothLEScanningMode::Active);
	this->watcher.Received({ this, &Win32::WindowsBTScanner::ReceivedHandler });
	this->watcher.Stopped({ this, &Win32::WindowsBTScanner::StoppedHandler });
}

Win32::WindowsBTScanner::~WindowsBTScanner()
{
	this->Close();
	this->pubDevMap.FreeAll(DeviceFree);
	this->randDevMap.FreeAll(DeviceFree);
}

void Win32::WindowsBTScanner::HandleRecordUpdate(RecordHandler hdlr, AnyType userObj)
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
		Sync::ThreadUtil::Create(ScanThread, this);
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

NN<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> Win32::WindowsBTScanner::GetPublicMap(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->devMut);
	return this->pubDevMap;
}

NN<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> Win32::WindowsBTScanner::GetRandomMap(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->devMut);
	return this->randDevMap;
}
