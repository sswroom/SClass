#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AndroidVideoCapture.h"
#include "Media/V4LVideoCapture.h"
#include "Media/VideoCaptureMgr.h"

struct Media::VideoCaptureMgr::ClassData
{
	NN<Media::V4LVideoCaptureMgr> v4lMgr;
	NN<Media::AndroidVideoCaptureMgr> androidMgr;
};

Media::VideoCaptureMgr::VideoCaptureMgr()
{
	NN<ClassData> data = MemAllocNN(ClassData);
	NEW_CLASSNN(data->v4lMgr, Media::V4LVideoCaptureMgr());
	NEW_CLASSNN(data->androidMgr, Media::AndroidVideoCaptureMgr());
	this->clsData = data;
}

Media::VideoCaptureMgr::~VideoCaptureMgr()
{
	NN<ClassData> data = this->clsData;
	data->v4lMgr.Delete();
	data->androidMgr.Delete();
	MemFreeNN(data);
}

UIntOS Media::VideoCaptureMgr::GetDeviceList(NN<Data::ArrayListNN<DeviceInfo>> devList)
{
	NN<ClassData> data = this->clsData;
	NN<DeviceInfo> devInfo;
	UTF8Char sbuff[512];
	Data::ArrayListNative<UInt32> devIdList;
	UIntOS ret = 0;
	UIntOS i = 0;
	UIntOS j = data->v4lMgr->GetDeviceList(devIdList);
	while (i < j)
	{
		devInfo = MemAllocNN(DeviceInfo);
		devInfo->devType = 0;
		devInfo->devId = devIdList.GetItem(i);
		data->v4lMgr->GetDeviceName(sbuff, devInfo->devId);
		devInfo->devName = Text::StrCopyNew(sbuff).Ptr();
		devList->Add(devInfo);
		ret++;
		i++;
	}

	devIdList.Clear();
	i = 0;
	j = data->androidMgr->GetDeviceList(devIdList);
	while (i < j)
	{
		devInfo = MemAllocNN(DeviceInfo);
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

void Media::VideoCaptureMgr::FreeDeviceList(NN<Data::ArrayListNN<DeviceInfo>> devList)
{
	NN<DeviceInfo> devInfo;
	UIntOS i = devList->GetCount();
	while (i-- > 0)
	{
		devInfo = devList->GetItemNoCheck(i);
		Text::StrDelNew(devInfo->devName);
		MemFreeNN(devInfo);
	}
}

Optional<Media::VideoCapturer> Media::VideoCaptureMgr::CreateDevice(Int32 devType, UIntOS devId)
{
	NN<ClassData> data = this->clsData;
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
		return nullptr;
	}
}

Text::CStringNN Media::VideoCaptureMgr::GetDevTypeName(Int32 devType)
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
