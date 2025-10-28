#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Math/Math_C.h"
#include "Net/WiFiCapturer.h"
#include "Sync/Interlocked.h"

void __stdcall Net::WiFiCapturer::ScanThread(NN<Sync::Thread> thread)
{
	NN<Net::WiFiCapturer> me = thread->GetUserObj().GetNN<Net::WiFiCapturer>();
	NN<Net::WirelessLAN::Interface> interf;
	if (!me->interf.SetTo(interf))
	{
		return;
	}
	NN<Net::WirelessLAN::BSSInfo> bss;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	OSInt si;
	UInt64 imac;
	UInt8 mac[8];
	UnsafeArray<const UInt8> macPtr;
	NN<Net::WiFiLogFile::LogFileEntry> entry;
	UInt64 maxIMAC;
	Int32 maxRSSI;
	Data::Timestamp lastStoreTime;
	Data::Timestamp currTime;

	{
		Data::ArrayListNN<Net::WirelessLAN::BSSInfo> bssList;
		mac[0] = 0;
		mac[1] = 0;
		lastStoreTime = Data::Timestamp::Now();
		while (!thread->IsStopping())
		{
			if (interf->Scan())
			{
				thread->Wait(10000);
				if (thread->IsStopping()) break;
				interf->GetBSSList(bssList);
				currTime = Data::Timestamp::Now();
				me->lastScanTime = currTime;
				
				maxIMAC = 0;
				maxRSSI = -128;
				i = 0;
				j = bssList.GetCount();
				while (i < j)
				{
					bss = bssList.GetItemNoCheck(i);
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
						maxRSSI = Double2Int32(bss->GetRSSI());
						maxIMAC = imac;
					}
					Sync::MutexUsage mutUsage(me->logMut);
					entry = me->wifiLog.AddBSSInfo(bss, si);
					entry->lastScanTime = currTime;
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
					if (me->wifiLog.Get(maxIMAC).SetTo(entry))
					{
						i = 0;
						j = bssList.GetCount();
						while (i < j)
						{
							bss = bssList.GetItemNoCheck(i);
		//					ssid = bss->GetSSID();
							MemCopyNO(&mac[2], bss->GetMAC().Ptr(), 6);
							mac[0] = 0;
							mac[1] = 0;
							imac = ReadMUInt64(mac);
							if (imac != maxIMAC)
							{
								Bool found = false;
								Int32 minRSSI;
								UOSInt minIndex;
								Int32 rssi1 = Double2Int32(bss->GetRSSI());
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
				}

				i = bssList.GetCount();
				while (i-- > 0)
				{
					bss = bssList.GetItemNoCheck(i);
					bss.Delete();
				}
				bssList.Clear();
			}
			else
			{
				thread->Wait(5000);
			}
			currTime = Data::Timestamp::Now();
			if (currTime.DiffMS(lastStoreTime) >= 600000)
			{
				lastStoreTime = currTime;
				me->StoreStatus();
			}
		}
	}
}

Net::WiFiCapturer::WiFiCapturer() : thread(ScanThread, this, CSTR("WiFiCapturer"))
{
	this->lastFileName = 0;
	this->interf = 0;
	this->hdlr = 0;
	this->hdlrObj = 0;
	this->lastScanTime = 0;
}

Net::WiFiCapturer::~WiFiCapturer()
{
	this->Stop();
	SDEL_TEXT(this->lastFileName);
	this->interf.Delete();
}

Bool Net::WiFiCapturer::IsError()
{
	return this->wlan.IsError();
}

Bool Net::WiFiCapturer::IsStarted()
{
	return this->thread.IsRunning();
}

Data::Timestamp Net::WiFiCapturer::GetLastScanTime()
{
	return this->lastScanTime;
}

Bool Net::WiFiCapturer::Start()
{
	if (this->IsError() || this->IsStarted())
	{
		return false;
	}
	this->interf.Delete();
	UOSInt i;
	NN<Text::String> namePtr;
	Data::ArrayListNN<Net::WirelessLAN::Interface> interfaces;
	NN<Net::WirelessLAN::Interface> ifObj;
	this->wlan.GetInterfaces(interfaces);
	i = interfaces.GetCount();
	Bool found = false;
	while (i-- > 0)
	{
		ifObj = interfaces.GetItemNoCheck(i);
		namePtr = ifObj->GetName();
		if (namePtr->StartsWith(UTF8STRC("rai")))
		{

		}
		else
		{
			this->interf = ifObj;
			this->thread.Start();
			found = true;
			break;
		}
	}
			
	i = interfaces.GetCount();
	while (i-- > 0)
	{
		ifObj = interfaces.GetItemNoCheck(i);
		if (ifObj.Ptr() != this->interf.OrNull())
		{
			ifObj.Delete();
		}
	}			
	return found;	
}

void Net::WiFiCapturer::Stop()
{
	if (this->thread.IsRunning())
	{
		this->thread.Stop();
		this->interf.Delete();
	}
}

void Net::WiFiCapturer::StoreStatus()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Data::DateTime dt;
	UOSInt i;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[i + 1];
	sptr = Text::StrConcatC(sptr, UTF8STRC("wifi"));
	dt.SetCurrTime();
	sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".txt"));

	Sync::MutexUsage mutUsage(this->logMut);
	if (this->wifiLog.StoreFile(CSTRP(sbuff, sptr)))
	{
		UnsafeArray<const UTF8Char> lastFileName;
		if (this->lastFileName.SetTo(lastFileName))
		{
			IO::Path::DeleteFile(lastFileName);
			Text::StrDelNew(lastFileName);
		}
		this->lastFileName = Text::StrCopyNewC(sbuff, (UOSInt)(sptr - sbuff));
	}
}

NN<Data::ArrayListNN<Net::WiFiLogFile::LogFileEntry>> Net::WiFiCapturer::GetLogList(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->logMut);
	return this->wifiLog.GetLogList();
}

void Net::WiFiCapturer::SetUpdateHandler(UpdateHandler hdlr, AnyType hdlrObj)
{
	this->hdlrObj = hdlrObj;
	this->hdlr = hdlr;
}
