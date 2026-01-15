#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CameraManager.h"

Media::CameraManager::CameraManager()
{
}

Media::CameraManager::~CameraManager()
{
}

IntOS Media::CameraManager::GetDeviceCount()
{
	return 0;
}

Optional<Media::PhotoCapturer> Media::CameraManager::CreateDevice(IntOS devId)
{
	return nullptr;
}
