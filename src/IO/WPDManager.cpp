#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/WPDManager.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderW.h"
#include <windows.h>
#include <portabledevice.h>
#include <portabledevicetypes.h>
#include <portabledeviceapi.h>

IO::WPDeviceInfo::WPDeviceInfo(NN<WPDManager> mgr, UnsafeArray<const WChar> devId)
{
	Text::StringBuilderUTF8 sb;
	this->mgr = mgr;
	this->devId = Text::StrCopyNew(devId);
	this->mgr->GetDevName(this->devId, sb);
	this->devName = Text::StrToWCharNew(sb.ToString());
	sb.ClearStr();
	this->mgr->GetDevDesc(this->devId, sb);
	this->devDesc = Text::StrToWCharNew(sb.ToString());
	sb.ClearStr();
	this->mgr->GetDevManu(this->devId, sb);
	this->devManu = Text::StrToWCharNew(sb.ToString());
}

IO::WPDeviceInfo::~WPDeviceInfo()
{
	Text::StrDelNew(this->devId);
	Text::StrDelNew(this->devName);
	Text::StrDelNew(this->devDesc);
	Text::StrDelNew(this->devManu);
}

IO::WPDManager::WPDManager()
{
	IPortableDeviceManager *pPortableDeviceManager;
	CoInitializeEx(0, COINIT_MULTITHREADED);
	HRESULT hr = CoCreateInstance(CLSID_PortableDeviceManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pPortableDeviceManager));
	if (FAILED(hr))
	{
		this->mgr = 0;
	}
	else
	{
		this->mgr = pPortableDeviceManager;
	}
}

IO::WPDManager::~WPDManager()
{
	if (this->mgr)
	{
		IPortableDeviceManager *pPortableDeviceManager = (IPortableDeviceManager*)this->mgr;
		pPortableDeviceManager->Release();
	}
	CoUninitialize();
}

IntOS IO::WPDManager::GetDevices(NN<Data::ArrayListNN<WPDeviceInfo>> devList)
{
	UnsafeArray<WChar*> sarr;
	NN<WPDeviceInfo> dev;
	UInt32 devCnt;
	UInt32 i;
	IPortableDeviceManager *pPortableDeviceManager = (IPortableDeviceManager*)this->mgr;
	HRESULT hr = pPortableDeviceManager->GetDevices(0, (DWORD*)&devCnt);
	if (FAILED(hr))
		return 0;
	if (devCnt == 0)
		return 0;
	sarr = MemAllocArr(WChar *, devCnt);
	hr = pPortableDeviceManager->GetDevices(sarr.Ptr(), (DWORD*)&devCnt);

	if (FAILED(hr))
	{
		MemFreeArr(sarr);
		return 0;
	}

	i = 0;
	while (i < devCnt)
	{
		NEW_CLASSNN(dev, WPDeviceInfo(*this, sarr[i]));
		devList->Add(dev);
		i++;
	}
	MemFreeArr(sarr);
	return (IntOS)devCnt;
}

Bool IO::WPDManager::GetDevName(UnsafeArray<const WChar> devId, NN<Text::StringBuilderUTF8> sb)
{
	IPortableDeviceManager *pPortableDeviceManager = (IPortableDeviceManager*)this->mgr;
	HRESULT hr;
	UInt32 strCnt;
	strCnt = 0;
	hr = pPortableDeviceManager->GetDeviceFriendlyName(devId.Ptr(), 0, (DWORD*)&strCnt);
	if (FAILED(hr))
		return false;
	WChar *wptr = sb->BeginAppendW(strCnt - 1);
	hr = pPortableDeviceManager->GetDeviceFriendlyName(devId.Ptr(), wptr, (DWORD*)&strCnt);
	if (FAILED(hr))
	{
		sb->EndAppendW(0);
		return false;
	}
	sb->EndAppendW(wptr + Text::StrCharCnt(wptr));
	return true;
}

Bool IO::WPDManager::GetDevDesc(UnsafeArray<const WChar> devId, NN<Text::StringBuilderUTF8> sb)
{
	IPortableDeviceManager *pPortableDeviceManager = (IPortableDeviceManager*)this->mgr;
	HRESULT hr;
	UInt32 strCnt;
	strCnt = 0;
	hr = pPortableDeviceManager->GetDeviceDescription(devId.Ptr(), 0, (DWORD*)&strCnt);
	if (FAILED(hr))
		return false;
	WChar *wptr = sb->BeginAppendW(strCnt - 1);
	hr = pPortableDeviceManager->GetDeviceDescription(devId.Ptr(), wptr, (DWORD*)&strCnt);
	if (FAILED(hr))
	{
		sb->EndAppendW(0);
		return false;
	}
	sb->EndAppendW(wptr + Text::StrCharCnt(wptr));
	return true;
}

Bool IO::WPDManager::GetDevManu(UnsafeArray<const WChar> devId, NN<Text::StringBuilderUTF8> sb)
{
	IPortableDeviceManager *pPortableDeviceManager = (IPortableDeviceManager*)this->mgr;
	HRESULT hr;
	UInt32 strCnt;
	strCnt = 0;
	hr = pPortableDeviceManager->GetDeviceManufacturer(devId.Ptr(), 0, (DWORD*)&strCnt);
	if (FAILED(hr))
		return false;
	WChar *wptr = sb->BeginAppendW(strCnt - 1);
	hr = pPortableDeviceManager->GetDeviceManufacturer(devId.Ptr(), wptr, (DWORD*)&strCnt);
	if (FAILED(hr))
	{
		sb->EndAppendW(0);
		return false;
	}
	sb->EndAppendW(wptr + Text::StrCharCnt(wptr));
	return true;
}
