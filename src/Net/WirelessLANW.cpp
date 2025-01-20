#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/WirelessLAN.h"
#include "Net/WLANWindowsCore.h"
#include "Net/WLANWindowsInterface.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include <windows.h>
#include <wlanapi.h>

Net::WirelessLAN::Network::Network(NN<Text::String> ssid, Double rssi)
{
	this->ssid = ssid->Clone();
	this->rssi = rssi;
}

Net::WirelessLAN::Network::Network(Text::CStringNN ssid, Double rssi)
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

Net::WirelessLAN::BSSInfo::BSSInfo(Text::CStringNN ssid, AnyType bssEntry)
{
	OSInt i;
	NN<WLAN_BSS_ENTRY> bss = bssEntry.GetNN<WLAN_BSS_ENTRY>();
	this->ssid = Text::String::New(ssid);
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
	const UInt8 *ptrCurr = bss->ulIeOffset + (const UTF8Char*)bssEntry.GetOSInt();
	const UInt8 *ptrEnd = ptrCurr + bss->ulIeSize;
	NN<Net::WirelessLANIE> ie;
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
		NEW_CLASSNN(ie, Net::WirelessLANIE(ptrCurr - 2));
		this->ieList.Add(ie);
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
							OPTSTR_DEL(this->devManuf);
							this->devManuf = Text::String::New(sbTmp.ToString(), sbTmp.GetLength());
							break;
						case 0x1023: //Model
							if (this->devModel.IsNull())
							{
								sbTmp.ClearStr();
								sbTmp.AppendC(&currItem[4], itemSize);
								this->devModel = Text::String::New(sbTmp.ToString(), sbTmp.GetLength());
							}
							break;
						case 0x1024: //Model Number
							sbTmp.ClearStr();
							sbTmp.AppendC(&currItem[4], itemSize);
							OPTSTR_DEL(this->devModel);
							this->devModel = Text::String::New(sbTmp.ToString(), sbTmp.GetLength());
							break;
						case 0x1042: //Serial
							sbTmp.ClearStr();
							sbTmp.AppendC(&currItem[4], itemSize);
							OPTSTR_DEL(this->devSN);
							this->devSN = Text::String::New(sbTmp.ToString(), sbTmp.GetLength());
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
	UOSInt i = this->ieList.GetCount();
	NN<Net::WirelessLANIE> ie;
	while (i-- > 0)
	{
		ie = this->ieList.GetItemNoCheck(i);
		ie.Delete();
	}
	this->ssid->Release();
	OPTSTR_DEL(this->devManuf);
	OPTSTR_DEL(this->devModel);
	OPTSTR_DEL(this->devSN);
}

NN<Text::String> Net::WirelessLAN::BSSInfo::GetSSID() const
{
	return this->ssid;
}

UInt32 Net::WirelessLAN::BSSInfo::GetPHYId()
{
	return this->phyId;
}

UnsafeArray<const UInt8> Net::WirelessLAN::BSSInfo::GetMAC()
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

Optional<Text::String> Net::WirelessLAN::BSSInfo::GetManuf()
{
	return this->devManuf;
}

Optional<Text::String> Net::WirelessLAN::BSSInfo::GetModel()
{
	return this->devModel;
}

Optional<Text::String> Net::WirelessLAN::BSSInfo::GetSN()
{
	return this->devSN;
}

UnsafeArrayOpt<const UTF8Char> Net::WirelessLAN::BSSInfo::GetCountry()
{
	if (this->devCountry[0])
		return this->devCountry;
	else
		return 0;
}

UnsafeArrayOpt<const UInt8> Net::WirelessLAN::BSSInfo::GetChipsetOUI(OSInt index)
{
	if (index < 0 || index >= WLAN_OUI_CNT)
		return 0;
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
	Net::WLANWindowsCore *core;
	NEW_CLASS(core, Net::WLANWindowsCore());
	this->clsData = core;
}

Net::WirelessLAN::~WirelessLAN()
{
	NN<Net::WLANWindowsCore> core = this->clsData.GetNN<Net::WLANWindowsCore>();
	core.Delete();
}

Bool Net::WirelessLAN::IsError()
{
	NN<Net::WLANWindowsCore> core = this->clsData.GetNN<Net::WLANWindowsCore>();
	return core->IsError();
}

UOSInt Net::WirelessLAN::GetInterfaces(NN<Data::ArrayListNN<Net::WirelessLAN::Interface>> outArr)
{
	NN<Net::WLANWindowsCore> core = this->clsData.GetNN<Net::WLANWindowsCore>();
	WLAN_INTERFACE_INFO_LIST *list;
	UOSInt retVal = 0;
	if (ERROR_SUCCESS == core->EnumInterfaces(0, (void**)&list))
	{
		UInt32 i;
		UInt32 j;
		i = 0;
		j = list->dwNumberOfItems;
		while (i < j)
		{
			NN<Net::WirelessLAN::Interface> interf;
			NN<Text::String> s = Text::String::NewNotNull(list->InterfaceInfo[i].strInterfaceDescription);
			NEW_CLASSNN(interf, Net::WLANWindowsInterface(s.Ptr(), &list->InterfaceInfo[i].InterfaceGuid, (Net::WirelessLAN::INTERFACE_STATE)list->InterfaceInfo[i].isState, core.Ptr()));
			s->Release();
			outArr->Add(interf);
			retVal++;
			i++;
		}
		core->FreeMemory(list);
	}

	return retVal;
}
