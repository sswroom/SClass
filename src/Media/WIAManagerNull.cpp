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


OSInt Media::WIAManager::GetDeviceCount()
{
	return 0;
}

const UTF8Char *Media::WIAManager::GetDeviceName(OSInt index)
{
	return 0;
}

Media::WIADevice *Media::WIAManager::CreateDevice(OSInt index)
{
	return 0;
}

Media::WIADevice::WIADevice(void *pWiaItem)
{
	this->pWiaItem = pWiaItem;
}

Media::WIADevice::~WIADevice()
{
}