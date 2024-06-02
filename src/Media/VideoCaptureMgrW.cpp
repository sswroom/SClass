#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/VideoCaptureMgr.h"
#include "Media/DShow/DShowVideoCapture.h"

struct Media::VideoCaptureMgr::ClassData
{
	Media::DShow::DShowVideoCaptureMgr *dshowMgr;
};

Media::VideoCaptureMgr::VideoCaptureMgr()
{
	this->clsData = MemAlloc(ClassData, 1);
	NEW_CLASS(this->clsData->dshowMgr, Media::DShow::DShowVideoCaptureMgr());
}

Media::VideoCaptureMgr::~VideoCaptureMgr()
{
	DEL_CLASS(this->clsData->dshowMgr);
	MemFree(this->clsData);
}

UOSInt Media::VideoCaptureMgr::GetDeviceList(NN<Data::ArrayListNN<DeviceInfo>> devList)
{
	NN<DeviceInfo> devInfo;
	UTF8Char sbuff[512];
	UOSInt ret = 0;
	UOSInt i = 0;
	UOSInt j = this->clsData->dshowMgr->GetDeviceCount();
	while (i < j)
	{
		devInfo = MemAllocNN(DeviceInfo);
		devInfo->devType = 0;
		devInfo->devId = i;
		this->clsData->dshowMgr->GetDeviceName(sbuff, i);
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

Optional<Media::IVideoCapture> Media::VideoCaptureMgr::CreateDevice(Int32 devType, UOSInt devId)
{
	if (devType == 0)
	{
		return this->clsData->dshowMgr->GetDevice(devId);
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
		return CSTR("DShow");
	default:
		return CSTR("Unknown");
	}
}
