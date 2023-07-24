#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "Net/WirelessLAN.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

Net::WirelessLAN::Network::Network(Text::CString ssid, Double rssi)
{
	this->ssid = Text::String::New(ssid);
	this->rssi = rssi;
}

Net::WirelessLAN::Network::Network(NotNullPtr<Text::String> ssid, Double rssi)
{
	this->ssid = ssid->Clone();
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

NotNullPtr<Text::String> Net::WirelessLAN::Network::GetSSID() const
{
	return this->ssid;
}

Net::WirelessLAN::BSSInfo::BSSInfo(Text::CString ssid, const void *bssEntry)
{
	this->ssid = Text::String::New(ssid);
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
}

Net::WirelessLAN::BSSInfo::~BSSInfo()
{
	OSInt i = this->ieList.GetCount();
	Net::WirelessLANIE *ie;
	while (i-- > 0)
	{
		ie = this->ieList.GetItem(i);
		DEL_CLASS(ie);
	}
	this->ssid->Release();
	SDEL_STRING(this->devManuf);
	SDEL_STRING(this->devModel);
	SDEL_STRING(this->devSN);
}

NotNullPtr<Text::String> Net::WirelessLAN::BSSInfo::GetSSID() const
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

Net::WirelessLANIE *Net::WirelessLAN::BSSInfo::GetIE(UOSInt index)
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

NotNullPtr<Text::String> Net::WirelessLAN::Interface::GetName() const
{
	return this->name;
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
