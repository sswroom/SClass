#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CameraManager.h"
#include "Media/MMALStillCapture.h"

Media::CameraManager::CameraManager()
{
}

Media::CameraManager::~CameraManager()
{
}

OSInt Media::CameraManager::GetDeviceCount()
{
	return 1;
}

Optional<Media::PhotoCapturer> Media::CameraManager::CreateDevice(OSInt devId)
{
	Media::PhotoCapturer *capture = 0;
	if (devId == 0)
	{
		NEW_CLASS(capture, Media::MMALStillCapture());
	}
	return capture;
}
