#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/WirelessLAN.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include <windows.h>
#include <wlanapi.h>

typedef void (__stdcall *FreeMemoryFunc)(void *pMemory);
typedef UInt32 (__stdcall *OpenHandleFunc)(UInt32 dwClientVersion, void *pReserved, UInt32 *pdwNegoiatedVersion, void **phClientHandle);
typedef UInt32 (__stdcall *CloseHandleFunc)(void *hClientHandle, void *pReserved);
typedef UInt32 (__stdcall *ScanFunc)(void *hClientHandle, void *pInterfaceGuid, void *pDot11Ssid, void *pIeData, void *pReserved);
typedef UInt32 (__stdcall *EnumInterfacesFunc)(void *hClientHandle, void *pReserved, void **ppInterfaceList);
typedef UInt32 (__stdcall *GetAvailableNetworkListFunc)(void *hClientHandle, void *pInterfaceGuid, UInt32 dwFlags, void *pReserved, void **ppAvailableNetworkList);
typedef UInt32 (__stdcall *GetNetworkBssListFunc)(void *hClientHandle, void *pInterfaceGuid, void *pDot11Ssid, Int32 dot11BssType, Int32 bSecurityEnabled, void *pReserved, void **ppWlanBssList);

typedef struct
{
	IO::Library *apiLib;
	void *hand;
	FreeMemoryFunc WlanFreeMemoryFunc;
	ScanFunc WlanScanFunc;
	EnumInterfacesFunc WlanEnumInterfacesFunc;
	GetAvailableNetworkListFunc WlanGetAvailableNetworkListFunc;
	GetNetworkBssListFunc WlanGetNetworkBssListFunc;
} ClassData;


void WirelessLAN_FreeMemory(ClassData *clsData, void *pMemory)
{
	if (clsData->WlanFreeMemoryFunc)
	{
		clsData->WlanFreeMemoryFunc(pMemory);
	}
}

UInt32 WirelessLAN_Scan(ClassData *clsData, void *pInterfaceGuid, void *pDot11Ssid, void *pIeData, void *pReserved)
{
	if (clsData->WlanScanFunc)
	{
		return clsData->WlanScanFunc(clsData->hand, pInterfaceGuid, pDot11Ssid, pIeData, pReserved);
	}
	return ERROR_INVALID_PARAMETER;
}

UInt32 WirelessLAN_EnumInterfaces(ClassData *clsData, void *pReserved, void **ppInterfaceList)
{
	if (clsData->WlanEnumInterfacesFunc)
	{
		return clsData->WlanEnumInterfacesFunc(clsData->hand, pReserved, ppInterfaceList);
	}
	return ERROR_INVALID_PARAMETER;
}

UInt32 WirelessLAN_GetAvailableNetworkList(ClassData *clsData, void *pInterfaceGuid, UInt32 dwFlags, void *pReserved, void **ppAvailableNetworkList)
{
	if (clsData->WlanGetAvailableNetworkListFunc)
	{
		return clsData->WlanGetAvailableNetworkListFunc(clsData->hand, pInterfaceGuid, dwFlags, pReserved, ppAvailableNetworkList);
	}
	return ERROR_INVALID_PARAMETER;
}

UInt32 WirelessLAN_GetNetworkBssList(ClassData *clsData, void *pInterfaceGuid, void *pDot11Ssid, Int32 dot11BssType, Int32 bSecurityEnabled, void *pReserved, void **ppWlanBssList)
{
	if (clsData->WlanGetNetworkBssListFunc)
	{
		return clsData->WlanGetNetworkBssListFunc(clsData->hand, pInterfaceGuid, pDot11Ssid, dot11BssType, bSecurityEnabled, pReserved, ppWlanBssList);
	}
	return ERROR_INVALID_PARAMETER;
}

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
	OSInt i;
	WLAN_BSS_ENTRY *bss = (WLAN_BSS_ENTRY*)bssEntry;
	this->ssid = Text::StrCopyNew(ssid);
	this->phyId = bss->uPhyId;
	memcpy(this->mac, bss->dot11Bssid, 6);
	this->bssType = (BSSType)bss->dot11BssType;
	this->phyType = bss->dot11BssPhyType;
	this->rssi = bss->lRssi;
	this->linkQuality = bss->uLinkQuality;
	this->freq = bss->ulChCenterFrequency * 1000.0;
	this->devManuf = 0;
	this->devModel = 0;
	this->devSN = 0;
	this->devCountry[0] = 0;
	i = 0;
	while (i < WLAN_OUI_CNT)
	{
		this->chipsetOUIs[i][0] = 0;
		this->chipsetOUIs[i][1] = 0;
		this->chipsetOUIs[i][2] = 0;
		i++;
	}
	NEW_CLASS(this->ieList, Data::ArrayList<Net::WirelessLANIE*>());

	const UInt8 *ptrCurr = bss->ulIeOffset + (const UTF8Char *)bssEntry;
	const UInt8 *ptrEnd = ptrCurr + bss->ulIeSize;
	Net::WirelessLANIE *ie;
	Text::StringBuilderUTF8 sbTmp;
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
		this->ieList->Add(ie);
		switch (ieCmd)
		{
		case 0:
//			sbTmp.ClearStr();
//			sbTmp.AppendC(UTF8STRC("IE "));
//			sbTmp.AppendI16(ieCmd);
//			sbTmp.AppendC(UTF8STRC(" - "));
//			sbTmp.Append(ptrCurr, ieSize);
//			printf("%s\r\n", sbTmp.ToString());
			break;
		case 7:
			if (ieSize >= 3)
			{
				this->devCountry[0] = ptrCurr[0];
				this->devCountry[1] = ptrCurr[1];
				this->devCountry[2] = 0;
			}
			break;
		case 0xdd:
			if (ptrCurr[0] == 0 && ptrCurr[1] == 0x50 && ptrCurr[2] == 0xF2) //Microsoft
			{
				if (ptrCurr[3] == 4) //WPS?
				{
					const UInt8 *currItem = ptrCurr + 4;
					const UInt8 *itemEnd = ptrCurr + ieSize;
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
						case 0x1021: //Manu
							sbTmp.ClearStr();
							sbTmp.AppendC(&currItem[4], itemSize);
							SDEL_TEXT(this->devManuf);
							this->devManuf = Text::StrCopyNew(sbTmp.ToString());
							break;
						case 0x1023: //Model
							sbTmp.ClearStr();
							sbTmp.AppendC(&currItem[4], itemSize);
							SDEL_TEXT(this->devModel);
							this->devModel = Text::StrCopyNew(sbTmp.ToString());
							break;
						case 0x1042: //Serial
							sbTmp.ClearStr();
							sbTmp.AppendC(&currItem[4], itemSize);
							SDEL_TEXT(this->devSN);
							this->devSN = Text::StrCopyNew(sbTmp.ToString());
							break;
						}
						currItem += itemSize + 4; 
					}
				}
			}
			else
			{
				i = 0;
				while (i < WLAN_OUI_CNT)
				{
					if (this->chipsetOUIs[i][0] == ptrCurr[0] && this->chipsetOUIs[i][1] == ptrCurr[1] && this->chipsetOUIs[i][2] == ptrCurr[2])
					{
						break;
					}
					else if (this->chipsetOUIs[i][0] == 0 && this->chipsetOUIs[i][1] == 0 && this->chipsetOUIs[i][2] == 0)
					{
						this->chipsetOUIs[i][0] = ptrCurr[0];
						this->chipsetOUIs[i][1] = ptrCurr[1];
						this->chipsetOUIs[i][2] = ptrCurr[2];
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
		return 0;
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

Net::WirelessLAN::Interface::Interface(const UTF8Char *name, void *id, INTERFACE_STATE state, void *clsData)
{
	this->clsData = clsData;
	this->name = Text::StrCopyNew(name);
	this->id = MemAlloc(GUID, 1);
	MemCopyNO(this->id, id, sizeof(GUID));
	this->state = state;
}

Net::WirelessLAN::Interface::~Interface()
{
	MemFree(this->id);
	Text::StrDelNew(this->name);
}

const UTF8Char *Net::WirelessLAN::Interface::GetName()
{
	return this->name;
}

Bool Net::WirelessLAN::Interface::Scan()
{
	return WirelessLAN_Scan((ClassData*)this->clsData, this->id, 0, 0, 0) == ERROR_SUCCESS;
}

UOSInt Net::WirelessLAN::Interface::GetNetworks(Data::ArrayList<Net::WirelessLAN::Network*> *networkList)
{
	WLAN_AVAILABLE_NETWORK_LIST *list;
	UOSInt retVal = 0;
	if (ERROR_SUCCESS == WirelessLAN_GetAvailableNetworkList((ClassData*)this->clsData, this->id, 0, 0, (void**)&list))
	{
		UInt32 i = 0;
		UInt32 j = list->dwNumberOfItems;
		while (i < j)
		{
			Net::WirelessLAN::Network *net;
			UTF8Char buff[33];
			Text::StrConcatC(buff, list->Network[i].dot11Ssid.ucSSID, list->Network[i].dot11Ssid.uSSIDLength);
			NEW_CLASS(net, Net::WirelessLAN::Network(buff, -100 + 0.5 * list->Network[i].wlanSignalQuality));
			networkList->Add(net);
			retVal++;
			i++;
		}
		WirelessLAN_FreeMemory((ClassData*)this->clsData, list);
	}
	return retVal;
}

UOSInt Net::WirelessLAN::Interface::GetBSSList(Data::ArrayList<Net::WirelessLAN::BSSInfo*> *bssList)
{
	WLAN_BSS_LIST *list;
	UOSInt retVal = 0;
	if (ERROR_SUCCESS == WirelessLAN_GetNetworkBssList((ClassData*)this->clsData, this->id, 0, 0, 0, 0, (void**)&list))
	{
		UInt32 i = 0;
		UInt32 j = list->dwNumberOfItems;
		while (i < j)
		{
			Net::WirelessLAN::BSSInfo *bss;
			UTF8Char buff[33];
			Text::StrConcatC(buff, list->wlanBssEntries[i].dot11Ssid.ucSSID, list->wlanBssEntries[i].dot11Ssid.uSSIDLength);
			NEW_CLASS(bss, Net::WirelessLAN::BSSInfo(buff, &list->wlanBssEntries[i]));
			bssList->Add(bss);
			retVal++;
			i++;
		}
		WirelessLAN_FreeMemory((ClassData*)this->clsData, list);
	}
	return retVal;
}

Net::WirelessLAN::WirelessLAN()
{
	ClassData *clsData = MemAlloc(ClassData, 1);
	this->clsData = clsData;
	UInt32 ver;
	NEW_CLASS(clsData->apiLib, IO::Library((const UTF8Char*)"Wlanapi.dll"));
	if (!clsData->apiLib->IsError())
	{
		clsData->WlanFreeMemoryFunc = (FreeMemoryFunc)clsData->apiLib->GetFunc("WlanFreeMemory");
		clsData->WlanScanFunc = (ScanFunc)clsData->apiLib->GetFunc("WlanScan");
		clsData->WlanEnumInterfacesFunc = (EnumInterfacesFunc)clsData->apiLib->GetFunc("WlanEnumInterfaces");
		clsData->WlanGetAvailableNetworkListFunc = (GetAvailableNetworkListFunc)clsData->apiLib->GetFunc("WlanGetAvailableNetworkList");
		clsData->WlanGetNetworkBssListFunc = (GetNetworkBssListFunc)clsData->apiLib->GetFunc("WlanGetNetworkBssList");
		OpenHandleFunc func = (OpenHandleFunc)clsData->apiLib->GetFunc("WlanOpenHandle");
		func(1, 0, &ver, &clsData->hand);
	}
	else
	{
		clsData->WlanFreeMemoryFunc = 0;
		clsData->WlanScanFunc = 0;
		clsData->WlanEnumInterfacesFunc = 0;
		clsData->WlanGetAvailableNetworkListFunc = 0;
		clsData->WlanGetNetworkBssListFunc = 0;
	}
}

Net::WirelessLAN::~WirelessLAN()
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (!clsData->apiLib->IsError())
	{
		CloseHandleFunc func = (CloseHandleFunc)clsData->apiLib->GetFunc("WlanCloseHandle");
		func(clsData->hand, 0);
	}
	DEL_CLASS(clsData->apiLib);
	MemFree(clsData);
}

Bool Net::WirelessLAN::IsError()
{
	ClassData *clsData = (ClassData*)this->clsData;
	return clsData->apiLib->IsError();
}

UOSInt Net::WirelessLAN::GetInterfaces(Data::ArrayList<Net::WirelessLAN::Interface*> *outArr)
{
	ClassData *clsData = (ClassData*)this->clsData;
	WLAN_INTERFACE_INFO_LIST *list;
	UOSInt retVal = 0;
	if (ERROR_SUCCESS == WirelessLAN_EnumInterfaces(clsData, 0, (void**)&list))
	{
		UInt32 i;
		UInt32 j;
		i = 0;
		j = list->dwNumberOfItems;
		while (i < j)
		{
			Net::WirelessLAN::Interface *interf;
			const UTF8Char *csptr = Text::StrToUTF8New(list->InterfaceInfo[i].strInterfaceDescription);
			NEW_CLASS(interf, Net::WirelessLAN::Interface(csptr, &list->InterfaceInfo[i].InterfaceGuid, (Net::WirelessLAN::INTERFACE_STATE)list->InterfaceInfo[i].isState, this->clsData));
			Text::StrDelNew(csptr);
			outArr->Add(interf);
			retVal++;
			i++;
		}
		WirelessLAN_FreeMemory(clsData, list);
	}

	return retVal;
}
