#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/FrameInfo.h"
#include "Media/CS/CSConverter.h"
#include "Media/OpenCV/OCVFrame.h"

Media::OpenCV::OCVFrame::OCVFrame(void *frame)
{
	this->frame = frame;
}

Media::OpenCV::OCVFrame::~OCVFrame()
{
}

void *Media::OpenCV::OCVFrame::GetFrame()
{
	return this->frame;
}

Media::OpenCV::OCVFrame *Media::OpenCV::OCVFrame::CreateYFrame(UInt8 **imgData, UOSInt dataSize, UInt32 fourcc, UOSInt dispWidth, UOSInt dispHeight, UOSInt storeWidth, UOSInt storeBPP, Media::PixelFormat pf)
{
	return 0;		
}
