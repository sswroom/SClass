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

Media::OpenCV::OCVFrame *Media::OpenCV::OCVFrame::CropToNew(Math::RectArea<UOSInt> *area)
{
	return 0;
}

void Media::OpenCV::OCVFrame::ClearOutsidePolygon(UOSInt *poly, UOSInt nPoints, UInt8 color)
{

}

UOSInt Media::OpenCV::OCVFrame::GetWidth()
{
	return 0;
}

UOSInt Media::OpenCV::OCVFrame::GetHeight()
{
	return 0;
}

OSInt Media::OpenCV::OCVFrame::GetBpl()
{
	return 0;
}

UInt8 *Media::OpenCV::OCVFrame::GetDataPtr()
{
	return 0;
}

void Media::OpenCV::OCVFrame::GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown)
{

}

Media::StaticImage *Media::OpenCV::OCVFrame::CreateStaticImage()
{
	return 0;
}

void Media::OpenCV::OCVFrame::ToBlackAndWhite(UInt8 middleV)
{

}

void Media::OpenCV::OCVFrame::Normalize()
{

}

Media::OpenCV::OCVFrame *Media::OpenCV::OCVFrame::CreateYFrame(UInt8 **imgData, UOSInt dataSize, UInt32 fourcc, UOSInt dispWidth, UOSInt dispHeight, UOSInt storeWidth, UOSInt storeBPP, Media::PixelFormat pf)
{
	return 0;		
}

Media::OpenCV::OCVFrame *Media::OpenCV::OCVFrame::CreateYFrame(Media::StaticImage *simg)
{
	return 0;		
}
