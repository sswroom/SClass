#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/FileStream.h"
#include "Math/Math_C.h"
#include "Net/WirelessLAN.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

Net::WirelessLAN::Network::Network(Text::CStringNN ssid, Double rssi)
{
	this->ssid = Text::String::New(ssid);
	this->rssi = rssi;
}

Net::WirelessLAN::Network::Network(NN<Text::String> ssid, Double rssi)
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

NN<Text::String> Net::WirelessLAN::Network::GetSSID() const
{
	return this->ssid;
}

Net::WirelessLAN::BSSInfo::BSSInfo(Text::CStringNN ssid, AnyType bssEntry)
{
	this->ssid = Text::String::New(ssid);
	this->phyId = 0;
	MemClear(this->mac, 6);
	this->bssType = Net::WirelessLAN::BST_INFRASTRUCTURE;
	this->phyType = 0;
	this->rssi = 0;
	this->linkQuality = 0;
	this->freq = 0;
	this->devManuf = nullptr;
	this->devModel = nullptr;
	this->devSN = nullptr;
	this->devCountry[0] = 0;
	IntOS i = 0;
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
	this->ieList.DeleteAll();
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
		return nullptr;
}

UnsafeArrayOpt<const UInt8> Net::WirelessLAN::BSSInfo::GetChipsetOUI(IntOS index)
{
	if (index < 0 || index >= WLAN_OUI_CNT)
	{
		return nullptr;
	}
	return this->chipsetOUIs[index];
}

UIntOS Net::WirelessLAN::BSSInfo::GetIECount()
{
	return this->ieList.GetCount();
}

Optional<Net::WirelessLANIE> Net::WirelessLAN::BSSInfo::GetIE(UIntOS index)
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
}

Net::WirelessLAN::~WirelessLAN()
{
}

Bool Net::WirelessLAN::IsError()
{
	return true;
}

UIntOS Net::WirelessLAN::GetInterfaces(NN<Data::ArrayListNN<Net::WirelessLAN::Interface>> outArr)
{
	return 0;
}
