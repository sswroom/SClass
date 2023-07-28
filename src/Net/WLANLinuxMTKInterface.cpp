#include "Stdafx.h"
#include "Net/WLANLinuxMTKInterface.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/types.h>		/* for "caddr_t" et al		*/
#if defined(__ARM_TUNE_MARVELL_F__)
#include <net/if.h>
#endif
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/wireless.h>

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
	Data::ArrayList<Net::WirelessLANIE*> ieList;
} BSSEntry;

void Net::WLANLinuxMTKInterface::Reopen()
{
	close(-1 + (int)(OSInt)this->id);
	this->id = (void*)(OSInt)(socket(AF_INET, SOCK_DGRAM, 0) + 1);
}

Net::WLANLinuxMTKInterface::WLANLinuxMTKInterface(Text::CString name, void *id, Net::WirelessLAN::INTERFACE_STATE state, UInt32 setCmd, UInt32 siteSurveyCmd)
{
	this->name = Text::String::New(name);
	this->id = (void*)(OSInt)(socket(AF_INET, SOCK_DGRAM, 0) + 1);
	this->state = state;
	this->setCmd = setCmd;
	this->siteSurveyCmd = siteSurveyCmd;
}

Net::WLANLinuxMTKInterface::~WLANLinuxMTKInterface()
{
	close(-1 + (int)(OSInt)this->id);
	this->name->Release();
}

Bool Net::WLANLinuxMTKInterface::Scan()
{
	struct iwreq wrq;
	int ret;
	UTF8Char sbuff[16];
//	return true;
	this->name->ConcatTo((UTF8Char*)wrq.ifr_ifrn.ifrn_name);
	wrq.u.data.length = (UInt16)(Text::StrConcatC(sbuff, UTF8STRC("SiteSurvey=1")) - sbuff + 1);
	wrq.u.data.pointer = sbuff;
	wrq.u.data.flags = 0;
//	Sync::MutexUsage mutUsage(cmds->mut);
//	printf("before ioctl, cmd = %04x, leng = %d\r\n", cmds->setCmd, wrq.u.data.length);
#if defined(THREADSAFE)
	MemLock();
	ret = ioctl(-1 + (int)(OSInt)this->id, this->setCmd, &wrq);
	MemUnlock();
#else
	ret = ioctl(-1 + (int)(OSInt)this->id, this->setCmd, &wrq);
#endif
//		printf("set SiteSurvey=1 ret = %d, errno = %d\r\n", ret, errno);
//	mutUsage.EndUse();
	return ret >= 0;
}

UOSInt Net::WLANLinuxMTKInterface::GetNetworks(Data::ArrayList<Net::WirelessLAN::Network*> *networkList)
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

UOSInt Net::WLANLinuxMTKInterface::GetBSSList(Data::ArrayList<Net::WirelessLAN::BSSInfo*> *bssList)
{
	UOSInt retVal = 0;
	Net::WirelessLAN::BSSInfo *bssInfo;
	BSSEntry bss;
	struct iwreq wrq;
	int ret;
	UInt8 *buff;
	UOSInt buffSize = IW_SCAN_MAX_DATA;
	this->name->ConcatTo((UTF8Char*)wrq.ifr_ifrn.ifrn_name);

	buff = MemAlloc(UInt8, buffSize);
	wrq.u.data.pointer = buff;
	wrq.u.data.flags = 0;
	wrq.u.data.length = (UInt16)buffSize;
//	Sync::MutexUsage mutUsage(cmds->mut);
//		printf("SiteSurvey ioctl before\r\n");
#if defined(THREADSAFE)
	MemLock();
	ret = ioctl(-1 + (int)(OSInt)this->id, this->siteSurveyCmd, &wrq);
	MemUnlock();
#else
	ret = ioctl(-1 + (int)(OSInt)this->id, this->siteSurveyCmd, &wrq);
#endif
//		printf("get_site_survey return %d, errno = %d, len = %d\r\n", ret, errno, wrq.u.data.length);
//	mutUsage.EndUse();
	if (ret == 0)
	{
		UOSInt lineCnt = 2;
		UOSInt colCnt;
		Text::PString lines[2];
		Text::PString cols[11];
		UTF8Char *macs[6];
		UOSInt ui;
		UOSInt channelInd = INVALID_INDEX;
		UOSInt ssidInd = INVALID_INDEX;
		UOSInt bssidInd = INVALID_INDEX;
		buff[wrq.u.data.length] = 0;

//		syslog(LOG_DEBUG, (const Char*)buff);
//			printf("%s\r\n", buff);
		lineCnt = Text::StrSplitLineP(lines, 2, {buff, wrq.u.data.length});
		if (lines[0].v[0] == 0 && lineCnt == 2)
		{
			lineCnt = Text::StrSplitLineP(lines, 2, lines[1]);
		}

		while (lineCnt == 2)
		{
			lineCnt = Text::StrSplitLineP(lines, 2, lines[1]);
			if (channelInd == INVALID_INDEX)
			{
				bssidInd = Text::StrIndexOfCharC(lines[0].v, lines[0].leng, ':');
				if (bssidInd == INVALID_INDEX)
				{
					continue;
				}
				bssidInd -= 2;
				lines[0].v[bssidInd - 1] = 0;
				colCnt = Text::StrSplitWSP(cols, 11, lines[0]);
				if (colCnt < 2)
				{
					continue;
				}
				ssidInd = (UOSInt)(cols[colCnt - 1].v - lines[0].v);
				channelInd = (UOSInt)(cols[colCnt - 2].v - lines[0].v);
			}
			else if (lines[0].leng < bssidInd)
			{
				continue;
			}
			colCnt = Text::StrSplitWSP(&cols[2], 9, {&lines[0].v[bssidInd], lines[0].leng - bssidInd}) + 2;
			lines[0].v[bssidInd - 1] = 0;
			cols[1].v = &lines[0].v[ssidInd];
			cols[1].leng = (UOSInt)(&lines[0].v[bssidInd - 1] - cols[1].v);
			cols[1].Trim();
			lines[0].v[ssidInd - 1] = 0;
			cols[0].v = &lines[0].v[channelInd];
			cols[0].leng = (UOSInt)(&lines[0].v[ssidInd - 1] - cols[0].v);
			cols[0].Trim();
			if (colCnt >= 8)
			{
				ui = colCnt;
				while (ui-- > 0)
				{
					if (cols[ui].v[0] == ',')
					{
						cols[ui] = cols[ui].Substring(1);
					}
				}
				if (Text::StrSplit(macs, 6, cols[2].v, ':') == 6)
				{
					bss.bssType = Net::WirelessLAN::BST_INFRASTRUCTURE;
					bss.mac[0] = Text::StrHex2UInt8C(macs[0]);
					bss.mac[1] = Text::StrHex2UInt8C(macs[1]);
					bss.mac[2] = Text::StrHex2UInt8C(macs[2]);
					bss.mac[3] = Text::StrHex2UInt8C(macs[3]);
					bss.mac[4] = Text::StrHex2UInt8C(macs[4]);
					bss.mac[5] = Text::StrHex2UInt8C(macs[5]);
					bss.phyId = (UInt32)retVal;
					bss.linkQuality = Text::StrToUInt32(cols[4].v);
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
					if (cols[5].StartsWith(UTF8STRC("11b")))
					{
						switch (Text::StrToInt32(cols[0].v))
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
					else if (cols[5].StartsWith(UTF8STRC("11a")))
					{
						switch (Text::StrToInt32(cols[0].v))
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
					if (cols[5].Equals(UTF8STRC("11b/g")))
					{
						bss.phyType = 6;
					}
					else if (cols[5].Equals(UTF8STRC("11b/g/n")))
					{
						bss.phyType = 7;
					}
					else if (cols[5].Equals(UTF8STRC("11a/n")))
					{
						bss.phyType = 8;
					}
					Int32 quality = Text::StrToInt32(cols[4].v);
					if (quality > 0)
						bss.rssi = (quality / 2) - 100;
					else
						bss.rssi = quality;
					
					NEW_CLASS(bssInfo, Net::WirelessLAN::BSSInfo(cols[1].ToCString(), &bss));
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
	return 0;
}
