#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "Net/WirelessLAN.h"
#include "Net/WLANLinuxInterface.h"
#include "Net/WLANLinuxMTKInterface.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/types.h>		/* for "caddr_t" et al		*/
#if defined(__ARM_TUNE_MARVELL_F__)
#include <net/if.h>
#endif
#include <sys/ioctl.h>
#include <sys/socket.h>
#ifndef __user
#define __user
#endif

#include <linux/wireless.h>
/*
==5258== Warning: set address range perms: large range [0x1b13d040, 0x2b13d044) (undefined)
==5258== Warning: set address range perms: large range [0x1b13d028, 0x2b13d05c) (noaccess)
==5258== Warning: set address range perms: large range [0x2b13e040, 0x4b13e044) (undefined)
==5258== Warning: set address range perms: large range [0x2b13e028, 0x4b13e05c) (noaccess)
==5258== Warning: set address range perms: large range [0x59c8a040, 0x99c8a044) (undefined)
==5258== Warning: set address range perms: large range [0x59c8a028, 0x99c8a05c) (noaccess)
==5258== Warning: set address range perms: large range [0x99c8b040, 0x119c8b044) (undefined)
==5258== Warning: set address range perms: large range [0x99c8b028, 0x119c8b05c) (noaccess)
==5258== Warning: set address range perms: large range [0x119c8c040, 0x219c8c044) (undefined)
==5258== Warning: set address range perms: large range [0x119c8c028, 0x219c8c05c) (noaccess)
==5258== Warning: set address range perms: large range [0x219c8d040, 0x419c8d044) (undefined)
==5258== Warning: set address range perms: large range [0x219c8d028, 0x419c8d05c) (noaccess)
==5258== Thread 9:
==5258== Invalid free() / delete / delete[] / realloc()
==5258==    at 0x48369AB: free (vg_replace_malloc.c:530)
==5258==    by 0x21BAE7: MemFree(void*) (MyMemoryL.cpp:255)
==5258==    by 0x2643D3: Net::WirelessLAN::Interface::GetBSSList(Data::ArrayList<Net::WirelessLAN::BSSInfo*>*) (WirelessLANL.cpp:597)
==5258==    by 0x25CDC6: ScanThread(void*) (WiFiCaptureMain.cpp:354)
==5258==    by 0x4917FA2: start_thread (pthread_create.c:486)
==5258==    by 0x4D4D4CE: clone (clone.S:95)
==5258==  Address 0xfffffffffffffffc is not stack'd, malloc'd or (recently) free'd
==5258== 
==5258== Warning: set address range perms: large range [0x1b13d040, 0x2b13d044) (undefined)
*/

typedef struct
{
	UInt32 phyId;
	UInt8 mac[6];
	Net::WirelessLAN::BSSType bssType;
	Int32 phyType;
	Double rssi;
	UInt32 linkQuality;
	Double freq; //Hz
	Text::String *devManuf;
	Text::String *devModel;
	Text::String *devSN;
	UTF8Char country[3];
	UInt8 ouis[WLAN_OUI_CNT][3];
	Data::ArrayListNN<Net::WirelessLANIE> ieList;
} BSSEntry;

typedef struct
{
	UInt32 setCmd;
	UInt32 siteSurveyCmd;
	Sync::Mutex *mut;
} PrivCommands;

typedef struct
{
	int fd;
	Sync::Mutex *mut;
} WirelessLANData;



Net::WirelessLAN::Network::Network(NN<Text::String> ssid, Double rssi)
{
	this->ssid = ssid->Clone();
	this->rssi = rssi;
}

Net::WirelessLAN::Network::Network(Text::CString ssid, Double rssi)
{
	this->ssid = Text::String::New(ssid);
	this->rssi = rssi;
}

Net::WirelessLAN::Network::~Network()
{
	this->ssid->Release();
}

Double Net::WirelessLAN::Network::GetRSSI() const
{
	return this->rssi;
}

NN<Text::String> Net::WirelessLAN::Network::GetSSID() const
{
	return this->ssid;
}

Net::WirelessLAN::BSSInfo::BSSInfo(Text::CString ssid, const void *bssEntry)
{
	BSSEntry *bss = (BSSEntry*)bssEntry;
	this->ssid = Text::String::New(ssid);
	this->phyId = bss->phyId;
	MemCopyNO(this->mac, bss->mac, 6);
	this->bssType = bss->bssType;
	this->phyType = bss->phyType;
	this->rssi = bss->rssi;
	this->linkQuality = bss->linkQuality;
	this->freq = bss->freq;
	this->devManuf = SCOPY_STRING(bss->devManuf);
	this->devModel = SCOPY_STRING(bss->devModel);
	this->devSN = SCOPY_STRING(bss->devSN);
	Text::StrConcat(this->devCountry, bss->country);
	OSInt i = 0;
	while (i < WLAN_OUI_CNT)
	{
		this->chipsetOUIs[i][0] = bss->ouis[i][0];
		this->chipsetOUIs[i][1] = bss->ouis[i][1];
		this->chipsetOUIs[i][2] = bss->ouis[i][2];
		i++;
	}
	this->ieList.AddAll(bss->ieList);
	bss->ieList.Clear();
}

Net::WirelessLAN::BSSInfo::~BSSInfo()
{
	this->ieList.DeleteAll();
	this->ssid->Release();
	SDEL_STRING(this->devManuf);
	SDEL_STRING(this->devModel);
	SDEL_STRING(this->devSN);
}

NN<Text::String> Net::WirelessLAN::BSSInfo::GetSSID() const
{
	return this->ssid;
}

UInt32 Net::WirelessLAN::BSSInfo::GetPHYId()
{
	return this->phyId;
}

const UInt8 *Net::WirelessLAN::BSSInfo::GetMAC()
{
	return this->mac;
}

Net::WirelessLAN::BSSType Net::WirelessLAN::BSSInfo::GetBSSType()
{
	return this->bssType;
}

Int32 Net::WirelessLAN::BSSInfo::GetPHYType()
{
	return this->phyType;
}

Double Net::WirelessLAN::BSSInfo::GetRSSI()
{
	return this->rssi;
}

UInt32 Net::WirelessLAN::BSSInfo::GetLinkQuality()
{
	return this->linkQuality;
}

Double Net::WirelessLAN::BSSInfo::GetFreq()
{
	return this->freq;
}

Text::String *Net::WirelessLAN::BSSInfo::GetManuf()
{
	return this->devManuf;
}

Text::String *Net::WirelessLAN::BSSInfo::GetModel()
{
	return this->devModel;
}

Text::String *Net::WirelessLAN::BSSInfo::GetSN()
{
	return this->devSN;
}

const UTF8Char *Net::WirelessLAN::BSSInfo::GetCountry()
{
	if (this->devCountry[0])
		return this->devCountry;
	else
		return 0;
}

const UInt8 *Net::WirelessLAN::BSSInfo::GetChipsetOUI(OSInt index)
{
	if (index < 0 || index >= WLAN_OUI_CNT)
	{
		return 0;
	}
	return this->chipsetOUIs[index];
}

UOSInt Net::WirelessLAN::BSSInfo::GetIECount()
{
	return this->ieList.GetCount();
}

Optional<Net::WirelessLANIE> Net::WirelessLAN::BSSInfo::GetIE(UOSInt index)
{
	return this->ieList.GetItem(index);
}

Net::WirelessLAN::Interface::Interface()
{
	this->name = Text::String::NewEmpty();
}

Net::WirelessLAN::Interface::~Interface()
{
	this->name->Release();
}

NN<Text::String> Net::WirelessLAN::Interface::GetName() const
{
	return this->name;
}

Net::WirelessLAN::WirelessLAN()
{
	WirelessLANData *thisData = MemAlloc(WirelessLANData, 1);
	NEW_CLASS(thisData->mut, Sync::Mutex());
	thisData->fd = socket(AF_INET, SOCK_DGRAM, 0) + 1;
	this->clsData = thisData;
}

Net::WirelessLAN::~WirelessLAN()
{
	WirelessLANData *thisData = (WirelessLANData*)this->clsData;
	if (thisData->fd != 0)
	{
		close(-1 + thisData->fd);
	}
	DEL_CLASS(thisData->mut);
	MemFree(thisData);
}

Bool Net::WirelessLAN::IsError()
{
	WirelessLANData *thisData = (WirelessLANData*)this->clsData;
	return thisData->fd == 0;
}

UOSInt Net::WirelessLAN::GetInterfaces(NN<Data::ArrayListNN<Net::WirelessLAN::Interface>> outArr)
{
	WirelessLANData *thisData = (WirelessLANData*)this->clsData;
	UOSInt ret = 0;
	NN<Net::WirelessLAN::Interface> interf;
/*	NEW_CLASS(fs, IO::FileStream(CSTR("/proc/net/wireless"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		Text::StringBuilderUTF8 sb;
		Text::UTF8Reader *reader;
		UOSInt i;
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sb.ClearStr();
		reader->ReadLine(sb, 1024);
		sb.ClearStr();
		reader->ReadLine(sb, 1024);
		sb.ClearStr();
		while (reader->ReadLine(sb, 1024))
		{
			sb.TrimC();
			i = sb.IndexOf(':');
			if (i != INVALID_INDEX)
			{
				sb.TrimToLength(i);
				NEW_CLASS(interf, Net::WirelessLAN::Interface(sb.ToString(), (void*)(OSInt)thisData->fd, Net::WirelessLAN::INTERFACE_STATE_CONNECTED, 0));
				outArr->Add(interf);
				ret++;
			}
			sb.ClearStr();
		}
		DEL_CLASS(reader);		
	}
	DEL_CLASS(fs);*/

	IO::FileStream fs(CSTR("/proc/net/dev"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::StringBuilderUTF8 sb;
		UOSInt i;
		struct iwreq wrq;
		UInt8 *buff;
		UOSInt buffSize = 16;
		int ioret;
		buff = MemAlloc(UInt8, buffSize * sizeof(iw_priv_args));

		Text::UTF8Reader reader(fs);
		sb.ClearStr();
		reader.ReadLine(sb, 1024);
		sb.ClearStr();
		reader.ReadLine(sb, 1024);
		sb.ClearStr();
		while (reader.ReadLine(sb, 1024))
		{
			sb.Trim();
			i = sb.IndexOf(':');
			if (i != INVALID_INDEX)
			{
				sb.TrimToLength((UOSInt)i);
				Text::StrConcatC((UTF8Char*)wrq.ifr_ifrn.ifrn_name, sb.ToString(), sb.GetLength());
//				printf("Trying interface = %s\r\n", sb.ToString());
				wrq.u.data.pointer = buff;
				wrq.u.data.flags = 0;
				wrq.u.data.length = 16;
//				printf("SIOCGIWSCAN before\r\n");
				ioret = ioctl(-1 + thisData->fd, SIOCGIWSCAN, &wrq);
//				printf("SIOCGIWSCAN return %d, errno = %d\r\n", ioret, errno);
				if (ioret >= 0 || errno == E2BIG)
				{
					NEW_CLASSNN(interf, Net::WLANLinuxInterface(sb.ToCString(), (void*)(OSInt)thisData->fd, Net::WirelessLAN::INTERFACE_STATE_CONNECTED));
					outArr->Add(interf);
					ret++;
				}
				else
				{
					wrq.u.data.pointer = buff;
					wrq.u.data.length = (UInt16)buffSize;
					wrq.u.data.flags = 0;
					while (true)
					{
						ioret = ioctl(-1 + thisData->fd, SIOCGIWPRIV, &wrq);
//						printf("SIOCGIWPRIV ret = %d, errno = %d\r\n", ioret, errno);
						if (ioret >= 0)
						{
							UInt32 siteSurveyCmd = 0;
							UInt32 setCmd = 0;
//							printf("SIOCGIWPRIV if = %s:\r\n", wrq.ifr_ifrn.ifrn_name);
							iw_priv_args *args = (iw_priv_args*)buff;
							OSInt j;
							j = 0;
							while (j < wrq.u.data.length)
							{
								if (Text::StrEquals(args[j].name, "get_site_survey"))
								{
									siteSurveyCmd = args[j].cmd;
								}
								else if (Text::StrEquals(args[j].name, "set"))
								{
									setCmd = args[j].cmd;								
								}
								printf("Priv: %s %04x %d %d:\r\n", args[j].name, args[j].cmd, args[j].get_args, args[j].set_args);
								if (siteSurveyCmd != 0 && setCmd != 0)
								{
									NEW_CLASSNN(interf, Net::WLANLinuxMTKInterface(sb.ToCString(), (void*)(OSInt)thisData->fd, Net::WirelessLAN::INTERFACE_STATE_NOT_READY, setCmd, siteSurveyCmd));
									outArr->Add(interf);
									ret++;
									break;
								}

								j++;
							}
							break;						
						}
						else if (errno == E2BIG)
						{
							if (buffSize > 32)
							{
								break;
							}
							buffSize = buffSize << 1;
							MemFree(buff);
							buff = MemAlloc(UInt8, buffSize * sizeof(iw_priv_args));
							wrq.u.data.pointer = buff;
							wrq.u.data.length = (UInt16)buffSize;
						}
						else
						{
//							printf("SIOCGIWPRIV if = %s, return %d, errno = %d\r\n", wrq.ifr_ifrn.ifrn_name, ioret, errno);
							break;
						}
					}
				}
			}
			sb.ClearStr();
		}
		MemFree(buff);
	}
	return ret;
}
