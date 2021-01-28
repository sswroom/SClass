#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Media/AndroidVideoCapture.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

#include <stdio.h>

Media::AndroidVideoCaptureMgr::AndroidVideoCaptureMgr()
{

}

Media::AndroidVideoCaptureMgr::~AndroidVideoCaptureMgr()
{

}

OSInt Media::AndroidVideoCaptureMgr::GetDeviceList(Data::ArrayList<Int32> *devList)
{
	return 0;
}

UTF8Char *Media::AndroidVideoCaptureMgr::GetDeviceName(UTF8Char *buff, OSInt devId)
{
	return 0;
}

Media::IVideoCapture *Media::AndroidVideoCaptureMgr::CreateDevice(OSInt devId)
{
	return 0;
}
