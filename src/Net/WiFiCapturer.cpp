#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Math/Math.h"
#include "Net/WiFiCapturer.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"

UInt32 __stdcall Net::WiFiCapturer::ScanThread(void *userObj)
{
	Net::WiFiCapturer *me = (Net::WiFiCapturer*)userObj;
	Net::WirelessLAN::Interface *interf = me->interf;
	Data::ArrayList<Net::WirelessLAN::BSSInfo *> *bssList;
	Net::WirelessLAN::BSSInfo *bss;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	OSInt si;
	UInt64 imac;
	UInt8 mac[8];
	const UInt8 *macPtr;
	Net::WiFiLogFile::LogFileEntry *entry;
	Data::DateTime *dt;
	UInt64 maxIMAC;
	Int32 maxRSSI;
	Int64 lastStoreTime;
	Int64 currTime;

	me->threadRunning = true;
	mac[0] = 0;
	mac[1] = 0;
	NEW_CLASS(bssList, Data::ArrayList<Net::WirelessLAN::BSSInfo*>());
	NEW_CLASS(dt, Data::DateTime());
	dt->SetCurrTimeUTC();
	lastStoreTime = dt->ToTicks();	
	while (!me->threadToStop)
	{
		if (interf->Scan())
		{
			Sync::Thread::Sleep(10000);
			interf->GetBSSList(bssList);
			dt->SetCurrTimeUTC();
			currTime = dt->ToTicks();
			me->lastScanTimeTicks = currTime;
			
			maxIMAC = 0;
			maxRSSI = -128;
			i = 0;
			j = bssList->GetCount();
			while (i < j)
			{
				bss = bssList->GetItem(i);
				macPtr = bss->GetMAC();
				mac[2] = macPtr[0];
				mac[3] = macPtr[1];
				mac[4] = macPtr[2];
				mac[5] = macPtr[3];
				mac[6] = macPtr[4];
				mac[7] = macPtr[5];
				imac = ReadMUInt64(mac);
				if (maxRSSI < bss->GetRSSI() && bss->GetRSSI() < 0)
				{
					maxRSSI = Math::Double2Int32(bss->GetRSSI());
					maxIMAC = imac;
				}
				Sync::MutexUsage mutUsage(me->logMut);
				entry = me->wifiLog->AddBSSInfo(bss, &si);
				entry->lastScanTimeTicks = currTime;
				mutUsage.EndUse();

				if (me->hdlr)
				{
					me->hdlr(bss, currTime, me->hdlrObj);
				}
				i++;
			}

			if (maxRSSI >= -60 && maxRSSI < 0)
			{
				Sync::MutexUsage mutUsage(me->logMut);
				entry = me->wifiLog->Get(maxIMAC);
				i = 0;
				j = bssList->GetCount();
				while (i < j)
				{
					bss = bssList->GetItem(i);
//					ssid = bss->GetSSID();
					MemCopyNO(&mac[2], bss->GetMAC(), 6);
					mac[0] = 0;
					mac[1] = 0;
					imac = ReadMUInt64(mac);
					if (imac != maxIMAC)
					{
						Bool found = false;
						Int32 minRSSI;
						UOSInt minIndex;
						Int32 rssi1 = Math::Double2Int32(bss->GetRSSI());
						minRSSI = 0;
						minIndex = 0;
						k = 0;
						while (k < 20)
						{
							Int8 rssi2 = (Int8)((entry->neighbour[k] >> 48) & 0xff);
							if ((entry->neighbour[k] & 0xffffffffffffLL) == imac)
							{
								found = true;
								if (rssi1 > rssi2)
								{
									entry->neighbour[k] = imac | (((UInt64)rssi1 & 0xff) << 48) | (((UInt64)bss->GetLinkQuality()) << 56);
								}
								break;
							}
							else if (entry->neighbour[k] == 0)
							{
								entry->neighbour[k] = imac | (((UInt64)rssi1 & 0xff) << 48) | (((UInt64)bss->GetLinkQuality()) << 56);
								found = true;
								break;
							}
							else if (rssi2 < minRSSI)
							{
								minRSSI = rssi2;
								minIndex = k;
							}
							
							k++;
						}

						if (!found && minRSSI < rssi1)
						{
							entry->neighbour[minIndex] = imac | (((UInt64)rssi1 & 0xff) << 48) | (((UInt64)bss->GetLinkQuality()) << 56);
						}
					}
					i++;
				}
			}

			i = bssList->GetCount();
			while (i-- > 0)
			{
				bss = bssList->GetItem(i);
				DEL_CLASS(bss);
			}
			bssList->Clear();
		}
		else
		{
			Sync::Thread::Sleep(5000);
		}
		dt->SetCurrTimeUTC();
		currTime = dt->ToTicks();
		if ((currTime - lastStoreTime) >= 600000)
		{
			lastStoreTime = currTime;
			me->StoreStatus();
		}
	}
	DEL_CLASS(dt);
	DEL_CLASS(bssList);
	me->threadRunning = false;
	return 0;
}

Net::WiFiCapturer::WiFiCapturer()
{
	this->lastFileName = 0;
	NEW_CLASS(this->wifiLog, Net::WiFiLogFile());
	NEW_CLASS(this->logMut, Sync::Mutex());
	NEW_CLASS(this->wlan, Net::WirelessLAN());
	this->threadRunning = false;
	this->threadToStop = false;
	this->interf = 0;
	this->hdlr = 0;
	this->hdlrObj = 0;
	this->lastScanTimeTicks = 0;
}

Net::WiFiCapturer::~WiFiCapturer()
{
	this->Stop();
	DEL_CLASS(this->logMut);
	DEL_CLASS(this->wifiLog);
	DEL_CLASS(this->wlan);
	SDEL_TEXT(this->lastFileName);
	SDEL_CLASS(this->interf);
}

Bool Net::WiFiCapturer::IsError()
{
	return this->wlan->IsError();
}

Bool Net::WiFiCapturer::IsStarted()
{
	return this->threadRunning;
}

Int64 Net::WiFiCapturer::GetLastScanTimeTicks()
{
	return this->lastScanTimeTicks;
}

Bool Net::WiFiCapturer::Start()
{
	if (this->IsError() || this->IsStarted())
	{
		return false;
	}
	this->threadToStop = false;
	SDEL_CLASS(this->interf);
	UOSInt i;
	const UTF8Char *namePtr;
	Data::ArrayList<Net::WirelessLAN::Interface*> interfaces;
	Net::WirelessLAN::Interface *interface;
	wlan->GetInterfaces(&interfaces);
	i = interfaces.GetCount();
	Bool found = false;
	while (i-- > 0)
	{
		interface = interfaces.GetItem(i);
		namePtr = interface->GetName();
		if (Text::StrStartsWith(namePtr, (const UTF8Char*)"rai"))
		{

		}
		else
		{
			this->interf = interface;
			Sync::Thread::Create(ScanThread, this);
			found = true;
			break;
		}
	}
			
	i = interfaces.GetCount();
	while (i-- > 0)
	{
		interface = interfaces.GetItem(i);
		if (interface != this->interf)
		{
			DEL_CLASS(interface);
		}
	}			
	return found;	
}

void Net::WiFiCapturer::Stop()
{
	if (this->threadRunning)
	{
		threadToStop = true;
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
		SDEL_CLASS(this->interf);
	}
}

void Net::WiFiCapturer::StoreStatus()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Data::DateTime dt;
	OSInt si;
	IO::Path::GetProcessFileName(sbuff);
	si = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[si + 1];
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"wifi");
	dt.SetCurrTime();
	sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".txt");

	Sync::MutexUsage mutUsage(this->logMut);
	if (this->wifiLog->StoreFile(sbuff))
	{
		if (this->lastFileName)
		{
			IO::Path::DeleteFile(this->lastFileName);
			Text::StrDelNew(this->lastFileName);
		}
		this->lastFileName = Text::StrCopyNew(sbuff);
	}
}

Data::ArrayList<Net::WiFiLogFile::LogFileEntry*> *Net::WiFiCapturer::GetLogList(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->logMut);
	return wifiLog->GetLogList();
}

void Net::WiFiCapturer::SetUpdateHandler(UpdateHandler hdlr, void *hdlrObj)
{
	this->hdlrObj = hdlrObj;
	this->hdlr = hdlr;
}
