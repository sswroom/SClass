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
	IO::BTScanLog::AddressType addrType;
	Int8 txPower;
#if _MSC_VER >= 1920
	if (args.BluetoothAddressType() == BluetoothLEAdvertisementAddressType::Public)
	{
		addrType = IO::BTScanLog::AT_PUBLIC;
	}
	else if (args.BluetoothAddressType() == BluetoothLEAdvertisementAddressType::Random)
	{
		addrType = IO::BTScanLog::AT_RANDOM;
	}
	else
	{
		addrType = IO::BTScanLog::AT_UNKNOWN;
	}
	txPower = (Int8)args.TransmitPowerLevelInDBm();
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

Win32::WindowsBTScanner::WindowsBTScanner()
{
	Win32::WinRTCore::Init();
	this->recHdlr = 0;
	this->recHdlrObj = 0;
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

Data::UInt64Map<IO::BTScanLog::ScanRecord3*> *Win32::WindowsBTScanner::GetPublicMap(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->devMut);
	return this->pubDevMap;
}

Data::UInt64Map<IO::BTScanLog::ScanRecord3*> *Win32::WindowsBTScanner::GetRandomMap(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->devMut);
	return this->pubDevMap;
}
