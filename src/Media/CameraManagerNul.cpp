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

Optional<Media::PhotoCapturer> Media::CameraManager::CreateDevice(OSInt devId)
{
	return 0;
}
