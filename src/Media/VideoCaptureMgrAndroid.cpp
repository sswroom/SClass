#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AndroidVideoCapture.h"
#include "Media/V4LVideoCapture.h"
#include "Media/VideoCaptureMgr.h"

struct Media::VideoCaptureMgr::ClassData
{
	Media::V4LVideoCaptureMgr *v4lMgr;
	Media::AndroidVideoCaptureMgr *androidMgr;
};

Media::VideoCaptureMgr::VideoCaptureMgr()
{
	ClassData *data = MemAlloc(ClassData, 1);
	NEW_CLASS(data->v4lMgr, Media::V4LVideoCaptureMgr());
	NEW_CLASS(data->androidMgr, Media::AndroidVideoCaptureMgr());
	this->clsData = data;
}

Media::VideoCaptureMgr::~VideoCaptureMgr()
{
	ClassData *data = (ClassData*)this->clsData;;
	DEL_CLASS(data->v4lMgr);
	DEL_CLASS(data->androidMgr);
	MemFree(data);
}

UOSInt Media::VideoCaptureMgr::GetDeviceList(Data::ArrayList<DeviceInfo *> *devList)
{
	ClassData *data = (ClassData*)this->clsData;
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

	devIdList.Clear();
	i = 0;
	j = data->androidMgr->GetDeviceList(&devIdList);
	while (i < j)
	{
		devInfo = MemAlloc(DeviceInfo, 1);
		devInfo->devType = 1;
		devInfo->devId = devIdList.GetItem(i);
		data->androidMgr->GetDeviceName(sbuff, devInfo->devId);
		devInfo->devName = Text::StrCopyNew(sbuff);
		devList->Add(devInfo);
		ret++;
		i++;
	}
	return ret;
}

void Media::VideoCaptureMgr::FreeDeviceList(Data::ArrayList<DeviceInfo *> *devList)
{
	DeviceInfo *devInfo;
	UOSInt i = devList->GetCount();
	while (i-- > 0)
	{
		devInfo = devList->GetItem(i);
		SDEL_TEXT(devInfo->devName);
		MemFree(devInfo);
	}
}

Media::IVideoCapture *Media::VideoCaptureMgr::CreateDevice(Int32 devType, UOSInt devId)
{
	ClassData *data = (ClassData*)this->clsData;
	if (devType == 0)
	{
		return data->v4lMgr->CreateDevice(devId);
	}
	else if (devType == 1)
	{
		return data->androidMgr->CreateDevice(devId);
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
		return CSTR("Android");
	default:
		return CSTR("Unknown");
	}
}
