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

void Media::OpenCV::OCVFrame::ClearOutsidePolygon(Math::Coord2D<UOSInt> *poly, UOSInt nPoints, UInt8 color)
{

}

UOSInt Media::OpenCV::OCVFrame::GetWidth() const
{
	return 0;
}

UOSInt Media::OpenCV::OCVFrame::GetHeight() const
{
	return 0;
}

Math::Size2D<UOSInt> Media::OpenCV::OCVFrame::GetSize() const
{
	return Math::Size2D<UOSInt>(0, 0);
}

OSInt Media::OpenCV::OCVFrame::GetBpl()
{
	return 0;
}

UInt8 *Media::OpenCV::OCVFrame::GetDataPtr()
{
	return 0;
}

void Media::OpenCV::OCVFrame::GetImageData(UnsafeArray<UInt8> destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate)
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

NN<Media::OpenCV::OCVFrame> Media::OpenCV::OCVFrame::BilateralFilter(Int32 d, Double sigmaColor, Double sigmaSpace)
{
	NN<Media::OpenCV::OCVFrame> fr;
	NEW_CLASSNN(fr, OCVFrame(this->frame));
	return fr;
}

Optional<Media::OpenCV::OCVFrame> Media::OpenCV::OCVFrame::CreateYFrame(UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, UInt32 fourcc, Math::Size2D<UOSInt> dispSize, UOSInt storeWidth, UOSInt storeBPP, Media::PixelFormat pf)
{
	return 0;		
}

Optional<Media::OpenCV::OCVFrame> Media::OpenCV::OCVFrame::CreateYFrame(NN<Media::StaticImage> simg)
{
	return 0;
}
