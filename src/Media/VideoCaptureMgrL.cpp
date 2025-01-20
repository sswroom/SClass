#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/V4LVideoCapture.h"
#include "Media/VideoCaptureMgr.h"

struct Media::VideoCaptureMgr::ClassData
{
	Media::V4LVideoCaptureMgr *v4lMgr;
};

Media::VideoCaptureMgr::VideoCaptureMgr()
{
	this->clsData = MemAlloc(ClassData, 1);
	NEW_CLASS(this->clsData->v4lMgr, Media::V4LVideoCaptureMgr());
}

Media::VideoCaptureMgr::~VideoCaptureMgr()
{
	DEL_CLASS(this->clsData->v4lMgr);
	MemFree(this->clsData);
}

UOSInt Media::VideoCaptureMgr::GetDeviceList(NN<Data::ArrayListNN<DeviceInfo>> devList)
{
	NN<DeviceInfo> devInfo;
	UTF8Char sbuff[512];
	Data::ArrayList<UInt32> devIdList;
	UOSInt ret = 0;
	UOSInt i = 0;
	UOSInt j = this->clsData->v4lMgr->GetDeviceList(&devIdList);
	while (i < j)
	{
		devInfo = MemAllocNN(DeviceInfo);
		devInfo->devType = 0;
		devInfo->devId = devIdList.GetItem(i);
		this->clsData->v4lMgr->GetDeviceName(sbuff, devInfo->devId);
		devInfo->devName = Text::StrCopyNew(sbuff);
		devList->Add(devInfo);
		ret++;
		i++;
	}
	return ret;
}

void Media::VideoCaptureMgr::FreeDeviceList(NN<Data::ArrayListNN<DeviceInfo>> devList)
{
	NN<DeviceInfo> devInfo;
	UOSInt i = devList->GetCount();
	while (i-- > 0)
	{
		devInfo = devList->GetItemNoCheck(i);
		Text::StrDelNew(devInfo->devName);
		MemFreeNN(devInfo);
	}
}

Optional<Media::VideoCapturer> Media::VideoCaptureMgr::CreateDevice(Int32 devType, UOSInt devId)
{
	if (devType == 0)
	{
		return this->clsData->v4lMgr->CreateDevice(devId);
	}
	else
	{
		return 0;
	}
}

Text::CStringNN Media::VideoCaptureMgr::GetDevTypeName(Int32 devType)
{
	switch (devType)
	{
	case 0:
		return CSTR("V4L");
	default:
		return CSTR("Unknown");
	}
}
