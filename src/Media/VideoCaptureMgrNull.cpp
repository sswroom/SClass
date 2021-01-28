#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/VideoCaptureMgr.h"

Media::VideoCaptureMgr::VideoCaptureMgr()
{
	this->mgrData = 0;
}

Media::VideoCaptureMgr::~VideoCaptureMgr()
{
}

OSInt Media::VideoCaptureMgr::GetDeviceList(Data::ArrayList<DeviceInfo *> *devList)
{
	return 0;
}

void Media::VideoCaptureMgr::FreeDeviceList(Data::ArrayList<DeviceInfo *> *devList)
{
}

Media::IVideoCapture *Media::VideoCaptureMgr::CreateDevice(Int32 devType, OSInt devId)
{
	return 0;
}

const UTF8Char *Media::VideoCaptureMgr::GetDevTypeName(Int32 devType)
{
	return (const UTF8Char*)"Unknown";
}
