#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CameraManager.h"

Media::CameraManager::CameraManager()
{
}

Media::CameraManager::~CameraManager()
{
}

OSInt Media::CameraManager::GetDeviceCount()
{
	return 0;
}

Media::IPhotoCapture *Media::CameraManager::CreateDevice(OSInt devId)
{
	return 0;
}
