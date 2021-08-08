#include "Stdafx.h"
#include "IO/DebugWriter.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"
#include "Win32/WindowsBTScanner.h"
#include "Win32/WinRTCore.h"
#include "Data/ByteTool.h"
#include <sdkddkver.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::Advertisement;

void Win32::WindowsBTScanner::ReceivedHandler(winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher const &sender,
	winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs const &args)
{
	IO::BTScanLog::ScanRecord *rec = this->DeviceGet(args.BluetoothAddress());
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	rec->lastSeenTime = dt.ToTicks();
	rec->rssi = (Int8)args.RawSignalStrengthInDBm();
	rec->inRange = true;
#if _MSC_VER >= 1920
	if (args.BluetoothAddressType() == BluetoothLEAdvertisementAddressType::Public)
	{
		rec->addrType = IO::BTScanLog::AT_PUBLIC;
	}
	else if (args.BluetoothAddressType() == BluetoothLEAdvertisementAddressType::Random)
	{
		rec->addrType = IO::BTScanLog::AT_RANDOM;
	}
	else
	{
		rec->addrType = IO::BTScanLog::AT_UNKNOWN;
	}
	rec->txPower = args.TransmitPowerLevelInDBm();
#else
	rec->addrType = IO::BTScanLog::AT_UNKNOWN;
	rec->txPower = 0;
#endif
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

IO::BTScanLog::ScanRecord *Win32::WindowsBTScanner::DeviceGet(UInt64 mac)
{
	Sync::MutexUsage mutUsage(this->devMut);
	IO::BTScanLog::ScanRecord *rec = this->devMap->Get(mac);
	if (rec)
	{
		return rec;
	}
	rec = MemAlloc(IO::BTScanLog::ScanRecord, 1);
	MemClear(rec, sizeof(IO::BTScanLog::ScanRecord));
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
	this->devMap->Put(mac, rec);
	return rec;
}

void Win32::WindowsBTScanner::DeviceFree(IO::BTScanLog::ScanRecord *rec)
{
	SDEL_TEXT(rec->name);
	MemFree(rec);
}

Win32::WindowsBTScanner::WindowsBTScanner()
{
	Win32::WinRTCore::Init();
	this->recHdlr = 0;
	this->recHdlrObj = 0;
	NEW_CLASS(this->devMut, Sync::Mutex());
	NEW_CLASS(this->devMap, Data::UInt64Map<IO::BTScanLog::ScanRecord*>());
	
	this->watcher = BluetoothLEAdvertisementWatcher();
	this->watcher.ScanningMode(BluetoothLEScanningMode::Active);
	this->watcher.Received({ this, &Win32::WindowsBTScanner::ReceivedHandler });
	this->watcher.Stopped({ this, &Win32::WindowsBTScanner::StoppedHandler });
}

Win32::WindowsBTScanner::~WindowsBTScanner()
{
	this->Close();
	Data::ArrayList<IO::BTScanLog::ScanRecord*> *devList = this->devMap->GetValues();
	LIST_FREE_FUNC(devList, DeviceFree);
	DEL_CLASS(this->devMap);
	DEL_CLASS(this->devMut);
}

void Win32::WindowsBTScanner::HandleRecordUpdate(RecordHandler hdlr, void *userObj)
{
	this->recHdlrObj = userObj;
	this->recHdlr = hdlr;
}

Bool Win32::WindowsBTScanner::IsScanOn()
{
	return this->watcher.Status() == BluetoothLEAdvertisementWatcherStatus::Started;
}

void Win32::WindowsBTScanner::ScanOn()
{
	this->watcher.Start();
}

void Win32::WindowsBTScanner::ScanOff()
{
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

Data::UInt64Map<IO::BTScanLog::ScanRecord*> *Win32::WindowsBTScanner::GetRecordMap(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->devMut);
	return this->devMap;
}
