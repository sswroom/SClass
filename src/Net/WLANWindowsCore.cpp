#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Library.h"
#include "Net/WLANWindowsCore.h"
#include <windows.h>
#include <wlanapi.h>

typedef void (__stdcall *FreeMemoryFunc)(void *pMemory);
typedef UInt32 (__stdcall *OpenHandleFunc)(UInt32 dwClientVersion, void *pReserved, UInt32 *pdwNegoiatedVersion, void **phClientHandle);
typedef UInt32 (__stdcall *CloseHandleFunc)(void *hClientHandle, void *pReserved);
typedef UInt32 (__stdcall *ScanFunc)(void *hClientHandle, void *pInterfaceGuid, void *pDot11Ssid, void *pIeData, void *pReserved);
typedef UInt32 (__stdcall *EnumInterfacesFunc)(void *hClientHandle, void *pReserved, void **ppInterfaceList);
typedef UInt32 (__stdcall *GetAvailableNetworkListFunc)(void *hClientHandle, void *pInterfaceGuid, UInt32 dwFlags, void *pReserved, void **ppAvailableNetworkList);
typedef UInt32 (__stdcall *GetNetworkBssListFunc)(void *hClientHandle, void *pInterfaceGuid, void *pDot11Ssid, Int32 dot11BssType, Int32 bSecurityEnabled, void *pReserved, void **ppWlanBssList);

struct Net::WLANWindowsCore::ClassData
{
	IO::Library *apiLib;
	void *hand;
	FreeMemoryFunc WlanFreeMemoryFunc;
	ScanFunc WlanScanFunc;
	EnumInterfacesFunc WlanEnumInterfacesFunc;
	GetAvailableNetworkListFunc WlanGetAvailableNetworkListFunc;
	GetNetworkBssListFunc WlanGetNetworkBssListFunc;
};

Net::WLANWindowsCore::WLANWindowsCore()
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

Net::WLANWindowsCore::~WLANWindowsCore()
{
	if (!this->clsData->apiLib->IsError())
	{
		CloseHandleFunc func = (CloseHandleFunc)this->clsData->apiLib->GetFunc("WlanCloseHandle");
		func(this->clsData->hand, 0);
	}
	DEL_CLASS(this->clsData->apiLib);
	MemFree(this->clsData);
}

Bool Net::WLANWindowsCore::IsError()
{
	return this->clsData->apiLib->IsError();
}

void Net::WLANWindowsCore::FreeMemory(void *pMemory)
{
	if (clsData->WlanFreeMemoryFunc)
	{
		clsData->WlanFreeMemoryFunc(pMemory);
	}
}

UInt32 Net::WLANWindowsCore::Scan(void *pInterfaceGuid, void *pDot11Ssid, void *pIeData, void *pReserved)
{
	if (clsData->WlanScanFunc)
	{
		return clsData->WlanScanFunc(clsData->hand, pInterfaceGuid, pDot11Ssid, pIeData, pReserved);
	}
	return ERROR_INVALID_PARAMETER;
}

UInt32 Net::WLANWindowsCore::EnumInterfaces(void *pReserved, void **ppInterfaceList)
{
	if (clsData->WlanEnumInterfacesFunc)
	{
		return clsData->WlanEnumInterfacesFunc(clsData->hand, pReserved, ppInterfaceList);
	}
	return ERROR_INVALID_PARAMETER;
}

UInt32 Net::WLANWindowsCore::GetAvailableNetworkList(void *pInterfaceGuid, UInt32 dwFlags, void *pReserved, void **ppAvailableNetworkList)
{
	if (clsData->WlanGetAvailableNetworkListFunc)
	{
		return clsData->WlanGetAvailableNetworkListFunc(clsData->hand, pInterfaceGuid, dwFlags, pReserved, ppAvailableNetworkList);
	}
	return ERROR_INVALID_PARAMETER;
}

UInt32 Net::WLANWindowsCore::GetNetworkBssList(void *pInterfaceGuid, void *pDot11Ssid, Int32 dot11BssType, Int32 bSecurityEnabled, void *pReserved, void **ppWlanBssList)
{
	if (clsData->WlanGetNetworkBssListFunc)
	{
		return clsData->WlanGetNetworkBssListFunc(clsData->hand, pInterfaceGuid, pDot11Ssid, dot11BssType, bSecurityEnabled, pReserved, ppWlanBssList);
	}
	return ERROR_INVALID_PARAMETER;
}

