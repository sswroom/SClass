#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AndroidVideoCapture.h"
#include "Media/V4LVideoCapture.h"
#include "Media/VideoCaptureMgr.h"

typedef struct
{
	Media::V4LVideoCaptureMgr *v4lMgr;
	Media::AndroidVideoCaptureMgr *androidMgr;
} ManagerData;

Media::VideoCaptureMgr::VideoCaptureMgr()
{
	ManagerData *data = MemAlloc(ManagerData, 1);
	NEW_CLASS(data->v4lMgr, Media::V4LVideoCaptureMgr());
	NEW_CLASS(data->androidMgr, Media::AndroidVideoCaptureMgr());
	this->mgrData = data;
}

Media::VideoCaptureMgr::~VideoCaptureMgr()
{
	ManagerData *data = (ManagerData*)this->mgrData;;
	DEL_CLASS(data->v4lMgr);
	DEL_CLASS(data->androidMgr);
	MemFree(data);
}

OSInt Media::VideoCaptureMgr::GetDeviceList(Data::ArrayList<DeviceInfo *> *devList)
{
	ManagerData *data = (ManagerData*)this->mgrData;
	DeviceInfo *devInfo;
	UTF8Char sbuff[512];
	Data::ArrayList<Int32> devIdList;
	OSInt ret = 0;
	OSInt i = 0;
	OSInt j = data->v4lMgr->GetDeviceList(&devIdList);
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
	OSInt i = devList->GetCount();
	while (i-- > 0)
	{
		devInfo = devList->GetItem(i);
		SDEL_TEXT(devInfo->devName);
		MemFree(devInfo);
	}
}

Media::IVideoCapture *Media::VideoCaptureMgr::CreateDevice(Int32 devType, OSInt devId)
{
	ManagerData *data = (ManagerData*)this->mgrData;
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

const UTF8Char *Media::VideoCaptureMgr::GetDevTypeName(Int32 devType)
{
	switch (devType)
	{
	case 0:
		return (const UTF8Char*)"V4L";
	case 1:
		return (const UTF8Char*)"Android";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
