#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/MMALVideoCapture.h"
#include "Media/V4LVideoCapture.h"
#include "Media/VideoCaptureMgr.h"

typedef struct
{
	Media::V4LVideoCaptureMgr *v4lMgr;
} ManagerData;

Media::VideoCaptureMgr::VideoCaptureMgr()
{
	ManagerData *data = MemAlloc(ManagerData, 1);
	NEW_CLASS(data->v4lMgr, Media::V4LVideoCaptureMgr());
	this->mgrData = data;
}

Media::VideoCaptureMgr::~VideoCaptureMgr()
{
	ManagerData *data = (ManagerData*)this->mgrData;;
	DEL_CLASS(data->v4lMgr);
	MemFree(data);
}

UOSInt Media::VideoCaptureMgr::GetDeviceList(Data::ArrayList<DeviceInfo *> *devList)
{
	ManagerData *data = (ManagerData*)this->mgrData;
	DeviceInfo *devInfo;
	UTF8Char sbuff[512];
	Data::ArrayList<UInt32> devIdList;
	UOSInt ret = 0;
	UOSInt i = 0;
	UOSInt j = data->v4lMgr->GetDeviceList(&devIdList);
	while (i < j)
	{
		devInfo = MemAlloc(DeviceInfo, 1);
		devInfo->devType = 0;
		devInfo->devId = devIdList.GetItem(i);
		data->v4lMgr->GetDeviceName(sbuff, devInfo->devId);
		devInfo->devName = Text::StrCopyNew(sbuff);
		devList->Add(devInfo);
		ret++;
		i++;
	}

	if (Media::MMALVideoCapture::IsAvailable())
	{
		devInfo = MemAlloc(DeviceInfo, 1);
		devInfo->devType = 1;
		devInfo->devId = 0;
		devInfo->devName = Text::StrCopyNew((const UTF8Char*)"Broadcom VC Camera");
		devList->Add(devInfo);
		ret++;
	}
	return ret;
}

void Media::VideoCaptureMgr::FreeDeviceList(Data::ArrayList<DeviceInfo *> *devList)
{
	DeviceInfo *devInfo;
	OSInt i = devList->GetCount();
	while (i-- > 0)
	{
		devInfo = devList->GetItem(i);
		SDEL_TEXT(devInfo->devName);
		MemFree(devInfo);
	}
}

Media::VideoCapturer *Media::VideoCaptureMgr::CreateDevice(Int32 devType, UOSInt devId)
{
	ManagerData *data = (ManagerData*)this->mgrData;
	if (devType == 0)
	{
		return data->v4lMgr->CreateDevice(devId);
	}
	else if (devType == 1)
	{
		Media::MMALVideoCapture *dev;
		NEW_CLASS(dev, Media::MMALVideoCapture(false));
		return dev;
	}
	else
	{
		return 0;
	}
}

Text::CString Media::VideoCaptureMgr::GetDevTypeName(Int32 devType)
{
	switch (devType)
	{
	case 0:
		return CSTR("V4L");
	case 1:
		return CSTR("Broadcom VC");
	default:
		return CSTR("Unknown");
	}
}
