#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/VideoCaptureMgr.h"

Media::VideoCaptureMgr::VideoCaptureMgr()
{
	this->clsData = 0;
}

Media::VideoCaptureMgr::~VideoCaptureMgr()
{
}

UOSInt Media::VideoCaptureMgr::GetDeviceList(Data::ArrayList<DeviceInfo *> *devList)
{
	return 0;
}

void Media::VideoCaptureMgr::FreeDeviceList(Data::ArrayList<DeviceInfo *> *devList)
{
}

Media::VideoCapturer *Media::VideoCaptureMgr::CreateDevice(Int32 devType, UOSInt devId)
{
	return 0;
}

Text::CString Media::VideoCaptureMgr::GetDevTypeName(Int32 devType)
{
	return CSTR("Unknown");
}
