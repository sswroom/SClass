#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "Net/WirelessLAN.h"
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
	const UTF8Char *devManuf;
	const UTF8Char *devModel;
	const UTF8Char *devSN;
	UTF8Char country[3];
	UInt8 ouis[WLAN_OUI_CNT][3];
	Data::ArrayList<Net::WirelessLANIE*> *ieList;
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



Net::WirelessLAN::Network::Network(const UTF8Char *ssid, Double rssi)
{
	this->ssid = Text::StrCopyNew(ssid);
	this->rssi = rssi;
}
Net::WirelessLAN::Network::~Network()
{
	Text::StrDelNew(this->ssid);
}

Double Net::WirelessLAN::Network::GetRSSI()
{
	return this->rssi;
}

const UTF8Char *Net::WirelessLAN::Network::GetSSID()
{
	return this->ssid;
}

Net::WirelessLAN::BSSInfo::BSSInfo(const UTF8Char *ssid, const void *bssEntry)
{
	BSSEntry *bss = (BSSEntry*)bssEntry;
	this->ssid = Text::StrCopyNew(ssid);
	this->phyId = bss->phyId;
	MemCopyNO(this->mac, bss->mac, 6);
	this->bssType = bss->bssType;
	this->phyType = bss->phyType;
	this->rssi = bss->rssi;
	this->linkQuality = bss->linkQuality;
	this->freq = bss->freq;
	this->devManuf = bss->devManuf?Text::StrCopyNew(bss->devManuf):0;
	this->devModel = bss->devModel?Text::StrCopyNew(bss->devModel):0;
	this->devSN = bss->devSN?Text::StrCopyNew(bss->devSN):0;
	Text::StrConcat(this->devCountry, bss->country);
	OSInt i = 0;
	while (i < WLAN_OUI_CNT)
	{
		this->chipsetOUIs[i][0] = bss->ouis[i][0];
		this->chipsetOUIs[i][1] = bss->ouis[i][1];
		this->chipsetOUIs[i][2] = bss->ouis[i][2];
		i++;
	}
	NEW_CLASS(this->ieList, Data::ArrayList<Net::WirelessLANIE*>());
	this->ieList->AddAll(bss->ieList);
	bss->ieList->Clear();
}

Net::WirelessLAN::BSSInfo::~BSSInfo()
{
	UOSInt i = this->ieList->GetCount();
	Net::WirelessLANIE *ie;
	while (i-- > 0)
	{
		ie = this->ieList->GetItem(i);
		DEL_CLASS(ie);
	}
	DEL_CLASS(this->ieList);
	SDEL_TEXT(this->ssid);
	SDEL_TEXT(this->devManuf);
	SDEL_TEXT(this->devModel);
	SDEL_TEXT(this->devSN);
}

const UTF8Char *Net::WirelessLAN::BSSInfo::GetSSID()
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

const UTF8Char *Net::WirelessLAN::BSSInfo::GetManuf()
{
	return this->devManuf;
}

const UTF8Char *Net::WirelessLAN::BSSInfo::GetModel()
{
	return this->devModel;
}

const UTF8Char *Net::WirelessLAN::BSSInfo::GetSN()
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
	return this->ieList->GetCount();
}

Net::WirelessLANIE *Net::WirelessLAN::BSSInfo::GetIE(UOSInt index)
{
	return this->ieList->GetItem(index);
}

void Net::WirelessLAN::Interface::Reopen()
{
	close(-1 + (int)(OSInt)this->id);
	this->id = (void*)(OSInt)(socket(AF_INET, SOCK_DGRAM, 0) + 1);
}

Net::WirelessLAN::Interface::Interface(const UTF8Char *name, void *id, INTERFACE_STATE state, void *clsData)
{
	if (clsData)
	{
		PrivCommands *cmds = MemAlloc(PrivCommands, 1);
		PrivCommands *scmds = (PrivCommands*)clsData;
		cmds->setCmd = scmds->setCmd;
		cmds->siteSurveyCmd = scmds->siteSurveyCmd;
		cmds->mut = scmds->mut;
		this->clsData = cmds;
	}
	else
	{
		this->clsData = 0;
	}
	this->name = Text::StrCopyNew(name);
	this->id = (void*)(OSInt)(socket(AF_INET, SOCK_DGRAM, 0) + 1);
	this->state = state;
}

Net::WirelessLAN::Interface::~Interface()
{
	if (this->clsData)
	{
		PrivCommands *cmds = (PrivCommands*)this->clsData;
		MemFree(cmds);
	}
	Text::StrDelNew(this->name);
	close(-1 + (int)(OSInt)this->id);
}

const UTF8Char *Net::WirelessLAN::Interface::GetName()
{
	return this->name;
}

Bool Net::WirelessLAN::Interface::Scan()
{
	struct iwreq wrq;
	int ret;
	if (this->clsData != 0)
	{
		PrivCommands *cmds = (PrivCommands*)this->clsData;
		Char sbuff[16];
//		return true;
		Text::StrConcat((UTF8Char*)wrq.ifr_ifrn.ifrn_name, this->name);
		wrq.u.data.length = (UInt16)(Text::StrConcat(sbuff, "SiteSurvey=1") - sbuff + 1);
		wrq.u.data.pointer = sbuff;
		wrq.u.data.flags = 0;
		Sync::MutexUsage mutUsage(cmds->mut);
//		printf("before ioctl, cmd = %04x, leng = %d\r\n", cmds->setCmd, wrq.u.data.length);
		ret = ioctl(-1 + (int)(OSInt)this->id, cmds->setCmd, &wrq);
//		printf("set SiteSurvey=1 ret = %d, errno = %d\r\n", ret, errno);
		mutUsage.EndUse();
		return ret >= 0;
	}

	Text::StrConcat((UTF8Char*)wrq.ifr_ifrn.ifrn_name, this->name);
	wrq.u.data.pointer = NULL;
	wrq.u.data.flags = 0;
	wrq.u.data.length = 0;
	ret = ioctl(-1 + (int)(OSInt)this->id, SIOCSIWSCAN, &wrq);
	if (ret < 0)
	{
		printf("SIOCSIWSCAN ret = %d, errno = %d\r\n", ret, errno);
		if (errno == 14)
		{
			this->Reopen();
		}
	}
	return ret >= 0;
}

UOSInt Net::WirelessLAN::Interface::GetNetworks(Data::ArrayList<Net::WirelessLAN::Network*> *networkList)
{
	UOSInt retVal = 0;
	Data::ArrayList<Net::WirelessLAN::BSSInfo*> bssList;
	Net::WirelessLAN::BSSInfo *bss;
	Net::WirelessLAN::Network *network;
	this->GetBSSList(&bssList);
	if (bssList.GetCount() > 0)
	{
		UOSInt i;
		retVal = bssList.GetCount();
		i = 0;
		while (i < retVal)
		{
			bss = bssList.GetItem(i);
			NEW_CLASS(network, Net::WirelessLAN::Network(bss->GetSSID(), bss->GetRSSI()));
			DEL_CLASS(bss);
			networkList->Add(network);
			i++;
		}
	}
	return retVal;
}

UOSInt Net::WirelessLAN::Interface::GetBSSList(Data::ArrayList<Net::WirelessLAN::BSSInfo*> *bssList) ///////////////////////////////////
{
	UOSInt retVal = 0;
	BSSInfo *bssInfo;
	BSSEntry bss;
	Net::WirelessLANIE *ie;
	struct iwreq wrq;
	int ret;
	UInt8 *buff;
	UOSInt buffSize = IW_SCAN_MAX_DATA;
	NEW_CLASS(bss.ieList, Data::ArrayList<Net::WirelessLANIE*>());
	Text::StrConcat((UTF8Char*)wrq.ifr_ifrn.ifrn_name, this->name);
	if (this->clsData)
	{
		PrivCommands *cmds = (PrivCommands*)this->clsData;
		buff = MemAlloc(UInt8, buffSize);
		wrq.u.data.pointer = buff;
		wrq.u.data.flags = 0;
		wrq.u.data.length = (UInt16)buffSize;
		Sync::MutexUsage mutUsage(cmds->mut);
//		printf("SiteSurvey ioctl before\r\n");
		ret = ioctl(-1 + (int)(OSInt)this->id, cmds->siteSurveyCmd, &wrq);
//		printf("get_site_survey return %d, errno = %d, len = %d\r\n", ret, errno, wrq.u.data.length);
		mutUsage.EndUse();
		if (ret == 0)
		{
			UOSInt lineCnt = 2;
			UOSInt colCnt;
			Char *lines[2];
			Char *cols[11];
			Char *macs[6];
			UOSInt ui;
			UOSInt channelInd = INVALID_INDEX;
			UOSInt ssidInd = INVALID_INDEX;
			UOSInt bssidInd = INVALID_INDEX;
			buff[wrq.u.data.length] = 0;
//			printf("%s\r\n", buff);
			lineCnt = Text::StrSplitLine(lines, 2, (Char*)buff);
			if (lines[0][0] == 0 && lineCnt == 2)
			{
				lineCnt = Text::StrSplitLine(lines, 2, lines[1]);
			}

			while (lineCnt == 2)
			{
				lineCnt = Text::StrSplitLine(lines, 2, lines[1]);
				if (channelInd == INVALID_INDEX)
				{
					bssidInd = Text::StrIndexOf(lines[0], ':') - 2;
					if (bssidInd == INVALID_INDEX)
					{
						continue;
					}
					lines[0][bssidInd - 1] = 0;
					colCnt = Text::StrSplitWS(cols, 11, lines[0]);
					ssidInd = (UOSInt)(cols[colCnt - 1] - lines[0]);
					channelInd = (UOSInt)(cols[colCnt - 2] - lines[0]);
				}
				else
				{
				}
				colCnt = Text::StrSplitWS(&cols[2], 9, &lines[0][bssidInd]) + 2;
				lines[0][bssidInd - 1] = 0;
				cols[1] = &lines[0][ssidInd];
				Text::StrTrim(cols[1]);
				lines[0][ssidInd - 1] = 0;
				cols[0] = &lines[0][channelInd];
				Text::StrTrim(cols[0]);
				if (colCnt >= 8)
				{
					ui = colCnt;
					while (ui-- > 0)
					{
						if (cols[ui][0] == ',')
						{
							cols[ui]++;
						}
					}
					if (Text::StrSplit(macs, 6, cols[2], ':') == 6)
					{
						bss.bssType = BST_INFRASTRUCTURE;
						bss.mac[0] = Text::StrHex2UInt8C(macs[0]);
						bss.mac[1] = Text::StrHex2UInt8C(macs[1]);
						bss.mac[2] = Text::StrHex2UInt8C(macs[2]);
						bss.mac[3] = Text::StrHex2UInt8C(macs[3]);
						bss.mac[4] = Text::StrHex2UInt8C(macs[4]);
						bss.mac[5] = Text::StrHex2UInt8C(macs[5]);
						bss.phyId = (UInt32)retVal;
						bss.linkQuality = Text::StrToUInt32(cols[4]);
						bss.freq = 0;
						bss.devManuf = 0;
						bss.devModel = 0;
						bss.devSN = 0;
						bss.country[0] = 0;
						ui = WLAN_OUI_CNT;
						while (ui-- > 0)
						{
							bss.ouis[ui][0] = 0;
							bss.ouis[ui][1] = 0;
							bss.ouis[ui][2] = 0;
						}
						if (Text::StrStartsWith(cols[5], "11b"))
						{
							switch (Text::StrToInt32(cols[0]))
							{
							case 1:
								bss.freq = 2412000000.0;
								break;
							case 2:
								bss.freq = 2417000000.0;
								break;
							case 3:
								bss.freq = 2422000000.0;
								break;
							case 4:
								bss.freq = 2427000000.0;
								break;
							case 5:
								bss.freq = 2432000000.0;
								break;
							case 6:
								bss.freq = 2437000000.0;
								break;
							case 7:
								bss.freq = 2442000000.0;
								break;
							case 8:
								bss.freq = 2447000000.0;
								break;
							case 9:
								bss.freq = 2452000000.0;
								break;
							case 10:
								bss.freq = 2457000000.0;
								break;
							case 11:
								bss.freq = 2462000000.0;
								break;
							case 12:
								bss.freq = 2467000000.0;
								break;
							case 13:
								bss.freq = 2472000000.0;
								break;
							case 14:
								bss.freq = 2484000000.0;
								break;
							}
						}
						else if (Text::StrStartsWith(cols[5], "11a"))
						{
							switch (Text::StrToInt32(cols[0]))
							{
							case 7:
								bss.freq = 5035000000.0;
								break;
							case 8:
								bss.freq = 5040000000.0;
								break;
							case 9:
								bss.freq = 5045000000.0;
								break;
							case 11:
								bss.freq = 5055000000.0;
								break;
							case 12:
								bss.freq = 5060000000.0;
								break;
							case 16:
								bss.freq = 5080000000.0;
								break;
							case 32:
								bss.freq = 5160000000.0;
								break;
							case 34:
								bss.freq = 5170000000.0;
								break;
							case 36:
								bss.freq = 5180000000.0;
								break;
							case 38:
								bss.freq = 5190000000.0;
								break;
							case 40:
								bss.freq = 5200000000.0;
								break;
							case 42:
								bss.freq = 5210000000.0;
								break;
							case 44:
								bss.freq = 5220000000.0;
								break;
							case 46:
								bss.freq = 5230000000.0;
								break;
							case 48:
								bss.freq = 5240000000.0;
								break;
							case 50:
								bss.freq = 5250000000.0;
								break;
							case 52:
								bss.freq = 5260000000.0;
								break;
							case 54:
								bss.freq = 5270000000.0;
								break;
							case 56:
								bss.freq = 5280000000.0;
								break;
							case 58:
								bss.freq = 5290000000.0;
								break;
							case 60:
								bss.freq = 5300000000.0;
								break;
							case 62:
								bss.freq = 5310000000.0;
								break;
							case 64:
								bss.freq = 5320000000.0;
								break;
							case 68:
								bss.freq = 5340000000.0;
								break;
							case 96:
								bss.freq = 5480000000.0;
								break;
							case 100:
								bss.freq = 5500000000.0;
								break;
							case 102:
								bss.freq = 5510000000.0;
								break;
							case 104:
								bss.freq = 5520000000.0;
								break;
							case 106:
								bss.freq = 5530000000.0;
								break;
							case 108:
								bss.freq = 5540000000.0;
								break;
							case 110:
								bss.freq = 5550000000.0;
								break;
							}
						}
						bss.phyType = 0;
						if (Text::StrEquals(cols[5], "11b/g"))
						{
							bss.phyType = 6;
						}
						else if (Text::StrEquals(cols[5], "11b/g/n"))
						{
							bss.phyType = 7;
						}
						else if (Text::StrEquals(cols[5], "11a/n"))
						{
							bss.phyType = 8;
						}
						bss.rssi = Text::StrToInt32(cols[4]);
						NEW_CLASS(bssInfo, BSSInfo((const UTF8Char *)cols[1], &bss));
						bssList->Add(bssInfo);
						retVal++;
					}
				}
			}
		}
		else
		{
			printf("get_site_survey return %d, errno = %d, len = %d\r\n", ret, errno, wrq.u.data.length);
		}
		MemFree(buff);
		DEL_CLASS(bss.ieList);
		return 0;
	}

	buffSize = IW_SCAN_MAX_DATA;
	while (true)
	{
//		printf("Alloc %d bytes\r\n", buffSize);
		buff = MemAlloc(UInt8, buffSize);
		wrq.u.data.pointer = buff;
		wrq.u.data.flags = 0;
		wrq.u.data.length = (UInt16)buffSize;
//		printf("SIOCGIWSCAN before\r\n");
		ret = ioctl(-1 + (int)(OSInt)this->id, SIOCGIWSCAN, &wrq);
//		printf("SIOCGIWSCAN return %d, errno = %d\r\n", ret, errno);
		if (ret >= 0)
		{
			break;
		}
		else if (errno == E2BIG)
		{
			if (wrq.u.data.length > buffSize)
			{
				buffSize = wrq.u.data.length;
			}
			else
			{
				buffSize = buffSize << 1;
				if (buffSize >= 65536)
				{
					buffSize = 65535;
				}
			}
			MemFree(buff);
		}
		else if (errno == EFAULT)
		{
//			printf("SIOCGIWSCAN return %d, errno = %d, buffSize = %d, buff = %x \r\n", ret, errno, buffSize, (int)(OSInt)buff);
			MemFree(buff);
			DEL_CLASS(bss.ieList);
			return 0;
		}
	}
//	printf("SIOCGIWSCAN result size = %d\r\n", wrq.u.data.length);
/*	Text::StringBuilderUTF8 sbTmp;
	sbTmp.AppendHexBuff(buff, wrq.u.data.length, ' ', Text::LBT_CRLF);
	printf("%s\r\n", sbTmp.ToString());*/
/*
          Cell 01 - Address: C0:56:27:74:7F:F9
18 00 15 8B 00 00 00 00 01 00 C0 56 27 74 7F F9 
00 00 00 00 00 00 00 00 
                    Channel:40
10 00 05 8B 00 00 00 00 28 00 00 00 00 00 00 00 
                    Frequency:5.2 GHz (Channel 40)
10 00 05 8B 00 00 00 00 50 14 00 00 06 00 00 00 
                    Quality=61/70  Signal level=-49 dBm  
0C 00 01 8C 00 00 00 00 3D CF 00 4B 
                    Encryption key:on
10 00 2B 8B 00 00 00 00 00 00 00 08 00 00 00 00 
                    ESSID:"Stoneroad-5G"
1C 00 1B 8B 00 00 00 00 0C 00 01 00 00 00 00 00 
53 74 6F 6E 65 72 6F 61 64 2D 35 47 
                    Bit Rates:6 Mb/s; 9 Mb/s; 12 Mb/s; 18 Mb/s; 24 Mb/s
                              36 Mb/s; 48 Mb/s; 54 Mb/s
48 00 21 8B 00 00 00 00 80 8D 5B 00 00 00 00 00 
40 54 89 00 00 00 00 00 00 1B B7 00 00 00 00 00 
80 A8 12 01 00 00 00 00 00 36 6E 01 00 00 00 00 
00 51 25 02 00 00 00 00 00 6C DC 02 00 00 00 00 
80 F9 37 03 00 00 00 00 
                    Mode:Master
0C 00 07 8B 00 00 00 00 03 00 00 00 
                    Extra:tsf=0000011a82b93ae2
24 00 02 8C 00 00 00 00 14 00 00 00 00 00 00 00 
74 73 66 3D 30 30 30 30 30 31 31 61 38 32 62 39 
33 61 65 32 
                    Extra: Last beacon: 1619860ms ago
2B 00 02 8C 00 00 00 00 1B 00 00 00 00 00 00 00 
20 4C 61 73 74 20 62 65 61 63 6F 6E 3A 20 31 34 
35 35 34 39 36 6D 73 20 61 67 6F 

                    IE: Unknown: 000C53746F6E65726F61642D3547
                    IE: Unknown: 01088C129824B048606C
                    IE: Unknown: 030128
                    IE: Unknown: 071E5553202401112801112C01113001119501179901179D0117A10117A50117
                    IE: Unknown: 200103
                    IE: IEEE 802.11i/WPA2 Version 1
                        Group Cipher : TKIP
                        Pairwise Ciphers (1) : CCMP
                        Authentication Suites (1) : PSK
                    IE: Unknown: 2D1A6F0017FFFFFF0001000000000000000000000000001FFF071800
                    IE: Unknown: 3D1628070400000000000000000000000000000000000000
                    IE: Unknown: 7F080000000000000040
                    IE: Unknown: BF0C30798333EAFF0000EAFF0000
                    IE: Unknown: C005012A00C0FF
                    IE: Unknown: C30402121212
                    IE: Unknown: DD1E00904C336E0017FFFFFF0001000000000000000000000000001FFF071800
                    IE: Unknown: DD1A00904C3428070000000000000000000000000000000000000000
                    IE: Unknown: DD06005043030000
                    IE: WPA Version 1
                        Group Cipher : TKIP
                        Pairwise Ciphers (1) : TKIP
                        Authentication Suites (1) : PSK
                    IE: Unknown: DD180050F2020101800003A4000027A4000042435E0062322F00
                    IE: Unknown: 7F06000000020040
                    IE: Unknown: DD930050F204104A0001101044000102103B00010310470010EEEA7B153D43CD7586804F871686A9581021000C4C696E6B7379732C204C4C4310230009575254313930304143102400095752543139303041431042000E31334A32303630353530313137321054000800060050F20400011011000B53746F6E65726F61642D3110080002200C103C0001021049000600372A000120
D0 01 05 8C 00 00 00 00 C0 01 00 00 00 00 00 00 
00 0C 53 74 6F 6E 65 72 6F 61 64 2D 35 47 01 08 
8C 12 98 24 B0 48 60 6C 03 01 28 07 1E 55 53 20 
24 01 11 28 01 11 2C 01 11 30 01 11 95 01 17 99 
01 17 9D 01 17 A1 01 17 A5 01 17 20 01 03 30 14 
01 00 00 0F AC 02 01 00 00 0F AC 04 01 00 00 0F 
AC 02 00 00 2D 1A 6F 00 17 FF FF FF 00 01 00 00 
00 00 00 00 00 00 00 00 00 00 00 1F FF 07 18 00 
3D 16 28 07 04 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 7F 08 00 00 00 00 00 00 
00 40 BF 0C 30 79 83 33 EA FF 00 00 EA FF 00 00 
C0 05 01 2A 00 C0 FF C3 04 02 12 12 12 DD 1E 00 
90 4C 33 6E 00 17 FF FF FF 00 01 00 00 00 00 00 
00 00 00 00 00 00 00 00 1F FF 07 18 00 DD 1A 00 
90 4C 34 28 07 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 DD 06 00 50 43 03 00 
00 DD 16 00 50 F2 01 01 00 00 50 F2 02 01 00 00 
50 F2 02 01 00 00 50 F2 02 DD 18 00 50 F2 02 01 
01 80 00 03 A4 00 00 27 A4 00 00 42 43 5E 00 62 
32 2F 00 7F 06 00 00 00 02 00 40 DD 93 00 50 F2 
04 10 4A 00 01 10 10 44 00 01 02 10 3B 00 01 03 
10 47 00 10 EE EA 7B 15 3D 43 CD 75 86 80 4F 87 
16 86 A9 58 10 21 00 0C 4C 69 6E 6B 73 79 73 2C 
20 4C 4C 43 10 23 00 09 57 52 54 31 39 30 30 41 
43 10 24 00 09 57 52 54 31 39 30 30 41 43 10 42 
00 0E 31 33 4A 32 30 36 30 35 35 30 31 31 37 32 
10 54 00 08 00 06 00 50 F2 04 00 01 10 11 00 0B 
53 74 6F 6E 65 72 6F 61 64 2D 31 10 08 00 02 20 
0C 10 3C 00 01 02 10 49 00 06 00 37 2A 00 01 20
*/

	UInt8 *buffCurr = buff;
	UInt8 *buffEnd = buff + wrq.u.data.length;
	ret = 1;
	UTF8Char essid[IW_ESSID_MAX_SIZE + 1];
	OSInt i;
	UInt16 cmd;
	UInt16 len;
	OSInt firstOfst = 8;
	essid[0] = 0;
	while (buffEnd - buffCurr >= 8)
	{
		len = ReadUInt16(&buffCurr[0]);
		cmd = ReadUInt16(&buffCurr[2]);
//		printf("%x %d\r\n", cmd, len);
		if (len < 8 || (buffEnd - buffCurr) < len)
		{
			break;
		}
		switch(cmd)
		{
		case 0x8B15: //SIOCGIWAP
			if (len == 24)
			{
				if (retVal > 0)
				{
					NEW_CLASS(bssInfo, BSSInfo(essid, &bss));
					bssList->Add(bssInfo);
					SDEL_TEXT(bss.devManuf);
					SDEL_TEXT(bss.devModel);
					SDEL_TEXT(bss.devSN);
				}
				retVal++;
				bss.bssType = BST_INFRASTRUCTURE;
				bss.freq = 0;
				bss.linkQuality = 0;
				MemCopyNO(bss.mac, &buffCurr[10], 6);
				bss.phyId = (UInt32)retVal;
				bss.phyType = 0;
				bss.rssi = 0;
				bss.devManuf = 0;
				bss.devModel = 0;
				bss.devSN = 0;
				bss.country[0] = 0;
				i = WLAN_OUI_CNT;
				while (i-- > 0)
				{
					bss.ouis[i][0] = 0;
					bss.ouis[i][1] = 0;
					bss.ouis[i][2] = 0;
				}
				essid[0] = 0;
				firstOfst = 8;
			}
			else if (len == 20)
			{
				if (retVal > 0)
				{
					NEW_CLASS(bssInfo, BSSInfo(essid, &bss));
					bssList->Add(bssInfo);
					SDEL_TEXT(bss.devManuf);
					SDEL_TEXT(bss.devModel);
					SDEL_TEXT(bss.devSN);
				}
				retVal++;
				bss.bssType = BST_INFRASTRUCTURE;
				bss.freq = 0;
				bss.linkQuality = 0;
				MemCopyNO(bss.mac, &buffCurr[6], 6);
				bss.phyId = (UInt32)retVal;
				bss.phyType = 0;
				bss.rssi = 0;
				bss.devManuf = 0;
				bss.devModel = 0;
				bss.devSN = 0;
				bss.country[0] = 0;
				i = WLAN_OUI_CNT;
				while (i-- > 0)
				{
					bss.ouis[i][0] = 0;
					bss.ouis[i][1] = 0;
					bss.ouis[i][2] = 0;
				}
				essid[0] = 0;
				firstOfst = 4;
			}
			break;
		case 0x8B03: //SIOCGIWNWID:
			break;
		case 0x8B05: //SIOCGIWFREQ:
			if (len >= firstOfst + 8)
			{
				UInt32 exp = ReadUInt32(&buffCurr[firstOfst + 4]);
				if (exp > 0)
				{
					bss.freq = ReadUInt32(&buffCurr[firstOfst]);
					if (exp < 10)
					{
						while (exp-- > 0)
						{
							bss.freq *= 10;
						}
					}
				}
			}
			break;
		case 0x8B07: //SIOCGIWMODE:
			if (len >= firstOfst + 4)
			{
				UInt32 mode = ReadUInt32(&buffCurr[firstOfst]);
				if (mode >= 2)
				{
					bss.bssType = BST_INFRASTRUCTURE;
				}
				else
				{
					bss.bssType = BST_INDEPENDENT;
				}
			}
			break;
		case 0x8B01: //SIOCGIWNAME:
			{
				Text::StringBuilderUTF8 sbTmp;
				sbTmp.AppendC(&buffCurr[firstOfst], (UOSInt)(len - firstOfst));
				if (sbTmp.Equals((const UTF8Char*)"IEEE 802.11gn"))
				{
					bss.phyType = 7;
				}
				else if (sbTmp.Equals((const UTF8Char*)"IEEE 802.11bgn"))
				{
					bss.phyType = 7;
				}
				else if (sbTmp.Equals((const UTF8Char*)"IEEE 802.11bg"))
				{
					bss.phyType = 6;
				}
				else if (sbTmp.Equals((const UTF8Char*)"IEEE 802.11AC"))
				{
					bss.phyType = 8;
				}
				else
				{
					printf("SIOCGIWNAME: %s\r\n", sbTmp.ToString());
				}
			}
			break;
		case 0x8B1B: //SIOCGIWESSID:
			if (len > firstOfst + firstOfst && len < IW_ESSID_MAX_SIZE + firstOfst + firstOfst)
			{
				Text::StrConcatC(essid, &buffCurr[firstOfst + firstOfst], (UOSInt)(len - firstOfst - firstOfst));
			}
			break;
		case 0x8B2B: //SIOCGIWENCODE:
			break;
		case 0x8B21: //SIOCGIWRATE:
			break;
		case 0x8B2F: //SIOCGIWMODUL:
			break;
		case 0x8C01: // IWEVQUAL:
			if (len == firstOfst + 4)
			{
				bss.rssi = (Int8)buffCurr[firstOfst + 1];
				bss.linkQuality = buffCurr[firstOfst];
			}
			else
			{
				printf("Unsupported quality buffer\r\n");
			}
			break;
		case 0x8C02: //IWEVCUSTOM:
			if (len > 16)
			{
/*				Text::StringBuilderUTF8 sbTmp;
				sbTmp.Append(&buffCurr[16], len - 16);
				printf("Extra: %s\r\n", sbTmp.ToString());*/
			}
			break;
		case 0x8C05: //IWEVGENIE
			if (len >= firstOfst + firstOfst)
			{
				Text::StringBuilderUTF8 sbTmp;
				UInt8 *ptrEnd = buffCurr + len;
				UInt8 *ptrCurr = buffCurr + firstOfst + firstOfst;
				UInt8 ieCmd;
				UInt8 ieSize;
				while (ptrEnd - ptrCurr >= 2)
				{
					ieCmd = ptrCurr[0];
					ieSize = ptrCurr[1];
					ptrCurr += 2;
					if (ptrEnd - ptrCurr < ieSize)
					{
						break;
					}
					NEW_CLASS(ie, Net::WirelessLANIE(ptrCurr - 2));
					bss.ieList->Add(ie);
					switch (ieCmd)
					{
					case 0:
//						sbTmp.ClearStr();
//						sbTmp.Append((const UTF8Char*)"IE ");
//						sbTmp.AppendI16(ieCmd);
//						sbTmp.Append((const UTF8Char*)" - ");
//						sbTmp.Append(ptrCurr, ieSize);
//						printf("%s\r\n", sbTmp.ToString());
						break;
					case 7:
						if (ieSize >= 3)
						{
							bss.country[0] = ptrCurr[0];
							bss.country[1] = ptrCurr[1];
							bss.country[2] = 0;
						}
						break;
					case 0xdd:
						if (ptrCurr[0] == 0 && ptrCurr[1] == 0x50 && ptrCurr[2] == 0xF2 && ptrCurr[3] == 4) //WPS?
						{
							UInt8 *currItem = ptrCurr + 4;
							UInt8 *itemEnd = ptrCurr + ieSize;
							UInt16 itemId;
							UInt16 itemSize;
							while (itemEnd - currItem >= 4)
							{
								itemId = ReadMUInt16(currItem);
								itemSize = ReadMUInt16(&currItem[2]);
								if (currItem + itemSize + 4 > itemEnd)
								{
									break;
								}
								switch (itemId)
								{
								case 0x1011: //DeviceName
									break;
								case 0x1021: //Manu
									sbTmp.ClearStr();
									sbTmp.AppendC(&currItem[4], itemSize);
									SDEL_TEXT(bss.devManuf);
									bss.devManuf = Text::StrCopyNew(sbTmp.ToString());
									break;
								case 0x1023: //Model
									sbTmp.ClearStr();
									sbTmp.AppendC(&currItem[4], itemSize);
									SDEL_TEXT(bss.devModel);
									bss.devModel = Text::StrCopyNew(sbTmp.ToString());
									break;
								case 0x1042: //Serial
									sbTmp.ClearStr();
									sbTmp.AppendC(&currItem[4], itemSize);
									SDEL_TEXT(bss.devSN);
									bss.devSN = Text::StrCopyNew(sbTmp.ToString());
									break;
								}
								currItem += itemSize + 4; 
							}
						}
						else
						{
							i = 0;
							while (i < WLAN_OUI_CNT)
							{
								if (bss.ouis[i][0] == 0 && bss.ouis[i][1] == 0 && bss.ouis[i][2] == 0)
								{
									bss.ouis[i][0] = ptrCurr[0];
									bss.ouis[i][1] = ptrCurr[1];
									bss.ouis[i][2] = ptrCurr[2];
									break;
								}
								else if (bss.ouis[i][0] == ptrCurr[0] && bss.ouis[i][1] == ptrCurr[1] && bss.ouis[i][2] == ptrCurr[2])
								{
									break;
								}
								i++;
							}
						}
						break;
					default:
						break;
					}
					ptrCurr += ieSize;
				}
			}
			break;
		default:
			printf("Unknown cmd %x\r\n", cmd);
			break;
		}
		buffCurr += len;
	}
	if (retVal > 0)
	{
		NEW_CLASS(bssInfo, BSSInfo(essid, &bss));
		bssList->Add(bssInfo);
		SDEL_TEXT(bss.devManuf);
		SDEL_TEXT(bss.devModel);
		SDEL_TEXT(bss.devSN);
		bss.ieList->Clear();
	}
	
	MemFree(buff);
	DEL_CLASS(bss.ieList);
	return retVal;
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

UOSInt Net::WirelessLAN::GetInterfaces(Data::ArrayList<Net::WirelessLAN::Interface*> *outArr)
{
	WirelessLANData *thisData = (WirelessLANData*)this->clsData;
	IO::FileStream *fs;
	UOSInt ret = 0;
	Net::WirelessLAN::Interface *interf;
/*	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/proc/net/wireless", IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!fs->IsError())
	{
		Text::StringBuilderUTF8 sb;
		Text::UTF8Reader *reader;
		UOSInt i;
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sb.ClearStr();
		reader->ReadLine(&sb, 1024);
		sb.ClearStr();
		reader->ReadLine(&sb, 1024);
		sb.ClearStr();
		while (reader->ReadLine(&sb, 1024))
		{
			sb.Trim();
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

	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/proc/net/dev", IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!fs->IsError())
	{
		Text::StringBuilderUTF8 sb;
		Text::UTF8Reader *reader;
		UOSInt i;
		struct iwreq wrq;
		UInt8 *buff;
		UOSInt buffSize = 16;
		int ioret;
		buff = MemAlloc(UInt8, buffSize * sizeof(iw_priv_args));

		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sb.ClearStr();
		reader->ReadLine(&sb, 1024);
		sb.ClearStr();
		reader->ReadLine(&sb, 1024);
		sb.ClearStr();
		while (reader->ReadLine(&sb, 1024))
		{
			sb.Trim();
			i = sb.IndexOf(':');
			if (i != INVALID_INDEX)
			{
				sb.TrimToLength((UOSInt)i);
				Text::StrConcat((UTF8Char*)wrq.ifr_ifrn.ifrn_name, sb.ToString());
//				printf("Trying interface = %s\r\n", sb.ToString());
				wrq.u.data.pointer = buff;
				wrq.u.data.flags = 0;
				wrq.u.data.length = 16;
//				printf("SIOCGIWSCAN before\r\n");
				ioret = ioctl(-1 + thisData->fd, SIOCGIWSCAN, &wrq);
//				printf("SIOCGIWSCAN return %d, errno = %d\r\n", ioret, errno);
				if (ioret >= 0 || errno == E2BIG)
				{
					NEW_CLASS(interf, Net::WirelessLAN::Interface(sb.ToString(), (void*)(OSInt)thisData->fd, Net::WirelessLAN::INTERFACE_STATE_CONNECTED, 0));
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
							PrivCommands cmds;
							cmds.siteSurveyCmd = 0;
							cmds.setCmd = 0;
							cmds.mut = thisData->mut;
//							printf("SIOCGIWPRIV if = %s:\r\n", wrq.ifr_ifrn.ifrn_name);
							iw_priv_args *args = (iw_priv_args*)buff;
							OSInt j;
							j = 0;
							while (j < wrq.u.data.length)
							{
								if (Text::StrEquals(args[j].name, "get_site_survey"))
								{
									cmds.siteSurveyCmd = args[j].cmd;
								}
								else if (Text::StrEquals(args[j].name, "set"))
								{
									cmds.setCmd = args[j].cmd;								
								}
								printf("Priv: %s %04x %d %d:\r\n", args[j].name, args[j].cmd, args[j].get_args, args[j].set_args);
								if (cmds.siteSurveyCmd != 0 && cmds.setCmd != 0)
								{
									NEW_CLASS(interf, Net::WirelessLAN::Interface(sb.ToString(), (void*)(OSInt)thisData->fd, Net::WirelessLAN::INTERFACE_STATE_NOT_READY, &cmds));
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
		DEL_CLASS(reader);

		MemFree(buff);
	}
	DEL_CLASS(fs);

	return ret;
}
