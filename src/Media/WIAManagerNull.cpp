#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/WIAManager.h"
#include "Text/MyString.h"


Media::WIAManager::WIAManager()
{
}

Media::WIAManager::~WIAManager()
{
}


UOSInt Media::WIAManager::GetDeviceCount()
{
	return 0;
}

UnsafeArrayOpt<const UTF8Char> Media::WIAManager::GetDeviceName(UOSInt index)
{
	return nullptr;
}

Optional<Media::WIADevice> Media::WIAManager::CreateDevice(UOSInt index)
{
	return nullptr;
}

Media::WIADevice::WIADevice(void *pWiaItem)
{
	this->pWiaItem = pWiaItem;
}

Media::WIADevice::~WIADevice()
{
}