#include "Stdafx.h"
#include "Net/WLANWindowsInterface.h"
#include <windows.h>
#include <wlanapi.h>

Net::WLANWindowsInterface::WLANWindowsInterface(Text::String *name, void *guid, Net::WirelessLAN::INTERFACE_STATE state, Net::WLANWindowsCore *core)
{
	this->core = core;
	this->name = name->Clone();
	this->id = MemAlloc(GUID, 1);
	MemCopyNO(this->id, guid, sizeof(GUID));
	this->state = state;
}

Net::WLANWindowsInterface::~WLANWindowsInterface()
{
	MemFree(this->id);
}

Bool Net::WLANWindowsInterface::Scan()
{
	return this->core->Scan(this->id, 0, 0, 0) == ERROR_SUCCESS;
}

UOSInt Net::WLANWindowsInterface::GetNetworks(NotNullPtr<Data::ArrayListNN<Net::WirelessLAN::Network>> networkList)
{
	WLAN_AVAILABLE_NETWORK_LIST *list;
	UOSInt retVal = 0;
	if (ERROR_SUCCESS == this->core->GetAvailableNetworkList(this->id, 0, 0, (void**)&list))
	{
		UInt32 i = 0;
		UInt32 j = list->dwNumberOfItems;
		while (i < j)
		{
			NotNullPtr<Net::WirelessLAN::Network> net;
			UTF8Char buff[33];
			UTF8Char *sptr = Text::StrConcatC(buff, list->Network[i].dot11Ssid.ucSSID, list->Network[i].dot11Ssid.uSSIDLength);
			NEW_CLASSNN(net, Net::WirelessLAN::Network(CSTRP(buff, sptr), -100 + 0.5 * list->Network[i].wlanSignalQuality));
			networkList->Add(net);
			retVal++;
			i++;
		}
		this->core->FreeMemory(list);
	}
	return retVal;
}

UOSInt Net::WLANWindowsInterface::GetBSSList(NotNullPtr<Data::ArrayListNN<Net::WirelessLAN::BSSInfo>> bssList)
{
	WLAN_BSS_LIST *list;
	UOSInt retVal = 0;
	if (ERROR_SUCCESS == this->core->GetNetworkBssList(this->id, 0, 0, 0, 0, (void**)&list))
	{
		UInt32 i = 0;
		UInt32 j = list->dwNumberOfItems;
		while (i < j)
		{
			NotNullPtr<Net::WirelessLAN::BSSInfo> bss;
			UTF8Char buff[33];
			UTF8Char *sptr;
			sptr = Text::StrConcatC(buff, list->wlanBssEntries[i].dot11Ssid.ucSSID, list->wlanBssEntries[i].dot11Ssid.uSSIDLength);
			NEW_CLASSNN(bss, Net::WirelessLAN::BSSInfo(CSTRP(buff, sptr), &list->wlanBssEntries[i]));
			bssList->Add(bss);
			retVal++;
			i++;
		}
		this->core->FreeMemory(list);
	}
	return retVal;
}
