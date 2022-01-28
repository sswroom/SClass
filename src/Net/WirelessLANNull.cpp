#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "Net/WirelessLAN.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
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
	this->ssid = 0;
	this->phyId = 0;
	MemClear(this->mac, 6);
	this->bssType = Net::WirelessLAN::BST_INFRASTRUCTURE;
	this->phyType = 0;
	this->rssi = 0;
	this->linkQuality = 0;
	this->freq = 0;
	this->devManuf = 0;
	this->devModel = 0;
	this->devSN = 0;
	this->devCountry[0] = 0;
	OSInt i = 0;
	while (i < WLAN_OUI_CNT)
	{
		this->chipsetOUIs[i][0] = 0;
		this->chipsetOUIs[i][1] = 0;
		this->chipsetOUIs[i][2] = 0;
		i++;
	}
	NEW_CLASS(this->ieList, Data::ArrayList<Net::WirelessLANIE*>());
}

Net::WirelessLAN::BSSInfo::~BSSInfo()
{
	OSInt i = this->ieList->GetCount();
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

Net::WirelessLAN::Interface::Interface(const UTF8Char *name, void *id, INTERFACE_STATE state, void *clsData)
{
	this->clsData = 0;
	this->name = Text::String::NewNotNull(name);
	this->id = id;
	this->state = state;
}

Net::WirelessLAN::Interface::~Interface()
{
	this->name->Release();
}

Text::String *Net::WirelessLAN::Interface::GetName()
{
	return this->name;
}

Bool Net::WirelessLAN::Interface::Scan()
{
	return false;
}

UOSInt Net::WirelessLAN::Interface::GetNetworks(Data::ArrayList<Net::WirelessLAN::Network*> *networkList)
{
	return 0;
}

UOSInt Net::WirelessLAN::Interface::GetBSSList(Data::ArrayList<Net::WirelessLAN::BSSInfo*> *bssList) ///////////////////////////////////
{
	return 0;
}

Net::WirelessLAN::WirelessLAN()
{
}

Net::WirelessLAN::~WirelessLAN()
{
}

Bool Net::WirelessLAN::IsError()
{
	return true;
}

UOSInt Net::WirelessLAN::GetInterfaces(Data::ArrayList<Net::WirelessLAN::Interface*> *outArr)
{
	return 0;
}
