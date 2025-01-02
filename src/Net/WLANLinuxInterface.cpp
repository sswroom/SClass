#include "Stdafx.h"
#include "Net/WLANLinuxInterface.h"

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
	Data::ArrayListNN<Net::WirelessLANIE> ieList;
} BSSEntry;

void Net::WLANLinuxInterface::Reopen()
{
	close(-1 + (int)(OSInt)this->id);
	this->id = (void*)(OSInt)(socket(AF_INET, SOCK_DGRAM, 0) + 1);
}

Net::WLANLinuxInterface::WLANLinuxInterface(Text::CStringNN name, void *id, Net::WirelessLAN::INTERFACE_STATE state)
{
	this->name = Text::String::New(name);
	this->id = (void*)(OSInt)(socket(AF_INET, SOCK_DGRAM, 0) + 1);
	this->state = state;
}

Net::WLANLinuxInterface::~WLANLinuxInterface()
{
	close(-1 + (int)(OSInt)this->id);
}

Bool Net::WLANLinuxInterface::Scan()
{
	struct iwreq wrq;
//	struct iw_scan_req sreq;
	int ret;
	MemClear(&wrq, sizeof(wrq));
//	MemClear(&sreq, sizeof(sreq));
	this->name->ConcatTo((UTF8Char*)wrq.ifr_ifrn.ifrn_name);
	wrq.u.data.pointer = 0;//&sreq;
	wrq.u.data.length = 0;//sizeof(sreq);
	wrq.u.data.flags = IW_SCAN_DEFAULT;
	ret = ioctl(-1 + (int)(OSInt)this->id, SIOCSIWSCAN, &wrq);
	if (ret < 0)
	{
		printf("WLANLinuxInterface Scan failed, name = %s, ret = %d, errno = %d\r\n", this->name->v.Ptr(), ret, errno);
		if (errno == 14)
		{
			this->Reopen();
		}
	}
	return ret >= 0;
}


UOSInt Net::WLANLinuxInterface::GetNetworks(NN<Data::ArrayListNN<Net::WirelessLAN::Network>> networkList)
{
	UOSInt retVal = 0;
	Data::ArrayListNN<Net::WirelessLAN::BSSInfo> bssList;
	NN<Net::WirelessLAN::BSSInfo> bss;
	NN<Net::WirelessLAN::Network> network;
	this->GetBSSList(bssList);
	if (bssList.GetCount() > 0)
	{
		UOSInt i;
		retVal = bssList.GetCount();
		i = 0;
		while (i < retVal)
		{
			bss = bssList.GetItemNoCheck(i);
			NEW_CLASSNN(network, Net::WirelessLAN::Network(bss->GetSSID(), bss->GetRSSI()));
			bss.Delete();
			networkList->Add(network);
			i++;
		}
	}
	return retVal;
}

UOSInt Net::WLANLinuxInterface::GetBSSList(NN<Data::ArrayListNN<Net::WirelessLAN::BSSInfo>> bssList)
{
	UOSInt retVal = 0;
	NN<Net::WirelessLAN::BSSInfo> bssInfo;
	BSSEntry bss;
	NN<Net::WirelessLANIE> ie;
	struct iwreq wrq;
	int ret;
	UInt8 *buff;
	UOSInt buffSize = IW_SCAN_MAX_DATA;
	this->name->ConcatTo((UTF8Char*)wrq.ifr_ifrn.ifrn_name);


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
			return 0;
		}
		else
		{
			printf("SIOCGIWSCAN return %d, errno = %d, buffSize = %d, buff = %x \r\n", ret, errno, (UInt32)buffSize, (int)(OSInt)buff);
			MemFree(buff);
			return 0;
		}
	}
//	printf("SIOCGIWSCAN result size = %d\r\n", wrq.u.data.length);
/*	Text::StringBuilderUTF8 sbTmp;
	sbTmp.AppendHexBuff(buff, wrq.u.data.length, ' ', Text::LineBreakType::CRLF);
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
	UnsafeArray<UTF8Char> essidEnd;
	OSInt i;
	UInt16 cmd;
	UInt16 len;
	OSInt firstOfst = 8;
	essid[0] = 0;
	essidEnd = essid;
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
					NEW_CLASSNN(bssInfo, Net::WirelessLAN::BSSInfo(CSTRP(essid, essidEnd), &bss));
					bssList->Add(bssInfo);
					SDEL_STRING(bss.devManuf);
					SDEL_STRING(bss.devModel);
					SDEL_STRING(bss.devSN);
				}
				retVal++;
				bss.bssType = Net::WirelessLAN::BST_INFRASTRUCTURE;
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
				essidEnd = essid;
				firstOfst = 8;
			}
			else if (len == 20)
			{
				if (retVal > 0)
				{
					NEW_CLASSNN(bssInfo, Net::WirelessLAN::BSSInfo(CSTRP(essid, essidEnd), &bss));
					bssList->Add(bssInfo);
					SDEL_STRING(bss.devManuf);
					SDEL_STRING(bss.devModel);
					SDEL_STRING(bss.devSN);
				}
				retVal++;
				bss.bssType = Net::WirelessLAN::BST_INFRASTRUCTURE;
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
				essidEnd = essid;
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
					bss.bssType = Net::WirelessLAN::BST_INFRASTRUCTURE;
				}
				else
				{
					bss.bssType = Net::WirelessLAN::BST_INDEPENDENT;
				}
			}
			break;
		case 0x8B01: //SIOCGIWNAME:
			{
				Text::StringBuilderUTF8 sbTmp;
				sbTmp.AppendC(&buffCurr[firstOfst], (UOSInt)(len - firstOfst));
				if (sbTmp.Equals(UTF8STRC("IEEE 802.11gn")))
				{
					bss.phyType = 7;
				}
				else if (sbTmp.Equals(UTF8STRC("IEEE 802.11bgn")))
				{
					bss.phyType = 7;
				}
				else if (sbTmp.Equals(UTF8STRC("IEEE 802.11bg")))
				{
					bss.phyType = 6;
				}
				else if (sbTmp.Equals(UTF8STRC("IEEE 802.11AC")))
				{
					bss.phyType = 8;
				}
				else
				{
					printf("SIOCGIWNAME: %s\r\n", sbTmp.ToPtr());
				}
			}
			break;
		case 0x8B1B: //SIOCGIWESSID:
			if (len > firstOfst + firstOfst && len < IW_ESSID_MAX_SIZE + firstOfst + firstOfst)
			{
				essidEnd = Text::StrConcatC(essid, &buffCurr[firstOfst + firstOfst], (UOSInt)(len - firstOfst - firstOfst));
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
					NEW_CLASSNN(ie, Net::WirelessLANIE(ptrCurr - 2));
					bss.ieList.Add(ie);
					switch (ieCmd)
					{
					case 0:
//						sbTmp.ClearStr();
//						sbTmp.AppendC(UTF8STRC("IE "));
//						sbTmp.AppendI16(ieCmd);
//						sbTmp.AppendC(UTF8STRC(" - "));
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
									SDEL_STRING(bss.devManuf);
									bss.devManuf = Text::String::New(sbTmp.ToString(), sbTmp.GetLength()).Ptr();
									break;
								case 0x1023: //Model
									if (bss.devModel == 0)
									{
										sbTmp.ClearStr();
										sbTmp.AppendC(&currItem[4], itemSize);
										bss.devModel = Text::String::New(sbTmp.ToString(), sbTmp.GetLength()).Ptr();
									}
									break;
								case 0x1024: //Model Number
									sbTmp.ClearStr();
									sbTmp.AppendC(&currItem[4], itemSize);
									SDEL_STRING(bss.devModel);
									bss.devModel = Text::String::New(sbTmp.ToString(), sbTmp.GetLength()).Ptr();
									break;
								case 0x1042: //Serial
									sbTmp.ClearStr();
									sbTmp.AppendC(&currItem[4], itemSize);
									SDEL_STRING(bss.devSN);
									bss.devSN = Text::String::New(sbTmp.ToString(), sbTmp.GetLength()).Ptr();
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
		NEW_CLASSNN(bssInfo, Net::WirelessLAN::BSSInfo(CSTRP(essid, essidEnd), &bss));
		bssList->Add(bssInfo);
		SDEL_STRING(bss.devManuf);
		SDEL_STRING(bss.devModel);
		SDEL_STRING(bss.devSN);
		bss.ieList.Clear();
	}
	
	MemFree(buff);
	return retVal;
}
