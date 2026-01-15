#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Math/Math_C.h"
#include "Media/AndroidVideoCapture.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

#include <stdio.h>

Media::AndroidVideoCaptureMgr::AndroidVideoCaptureMgr()
{

}

Media::AndroidVideoCaptureMgr::~AndroidVideoCaptureMgr()
{

}

UIntOS Media::AndroidVideoCaptureMgr::GetDeviceList(Data::ArrayList<UInt32> *devList)
{
	return 0;
}

UTF8Char *Media::AndroidVideoCaptureMgr::GetDeviceName(UTF8Char *buff, UIntOS devId)
{
	return 0;
}

Media::VideoCapturer *Media::AndroidVideoCaptureMgr::CreateDevice(UIntOS devId)
{
	return 0;
}
