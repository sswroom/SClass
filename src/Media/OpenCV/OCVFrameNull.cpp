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

NN<Media::OpenCV::OCVFrame> Media::OpenCV::OCVFrame::CropToNew(Math::RectArea<UIntOS> area)
{
	return NEW_CLASS_D(OCVFrame(this->frame));
}

void Media::OpenCV::OCVFrame::ClearOutsidePolygon(UnsafeArray<Math::Coord2D<UIntOS>> poly, UIntOS nPoints, UInt8 color)
{

}

UIntOS Media::OpenCV::OCVFrame::GetWidth() const
{
	return 0;
}

UIntOS Media::OpenCV::OCVFrame::GetHeight() const
{
	return 0;
}

Math::Size2D<UIntOS> Media::OpenCV::OCVFrame::GetSize() const
{
	return Math::Size2D<UIntOS>(0, 0);
}

IntOS Media::OpenCV::OCVFrame::GetBpl()
{
	return 0;
}

UInt8 *Media::OpenCV::OCVFrame::GetDataPtr()
{
	return 0;
}

void Media::OpenCV::OCVFrame::GetImageData(UnsafeArray<UInt8> destBuff, IntOS left, IntOS top, UIntOS width, UIntOS height, UIntOS destBpl, Bool upsideDown, Media::RotateType destRotate)
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

Optional<Media::OpenCV::OCVFrame> Media::OpenCV::OCVFrame::CreateYFrame(UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, UInt32 fourcc, Math::Size2D<UIntOS> dispSize, UIntOS storeWidth, UIntOS storeBPP, Media::PixelFormat pf)
{
	return 0;		
}

Optional<Media::OpenCV::OCVFrame> Media::OpenCV::OCVFrame::CreateYFrame(NN<Media::StaticImage> simg)
{
	return 0;
}
