#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
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

UOSInt Media::AndroidVideoCaptureMgr::GetDeviceList(Data::ArrayList<UInt32> *devList)
{
	return 0;
}

UTF8Char *Media::AndroidVideoCaptureMgr::GetDeviceName(UTF8Char *buff, UOSInt devId)
{
	return 0;
}

Media::IVideoCapture *Media::AndroidVideoCaptureMgr::CreateDevice(UOSInt devId)
{
	return 0;
}
