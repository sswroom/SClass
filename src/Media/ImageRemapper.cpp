#include "Media/ImageRemapper.h"

Media::ImageRemapper::ImageRemapper()
{
	this->srcImgPtr = 0;
	this->srcBpl = 0;
	this->srcWidth = 0;
	this->srcHeight = 0;
	this->srcPF = Media::PF_UNKNOWN;
	this->srcPal = 0;
	this->destBpl = 0;
	this->destWidth = 0;
	this->destHeight = 0;
}

void Media::ImageRemapper::SetSourceImage32(UnsafeArray<const UInt8> srcImgPtr, UOSInt srcBpl, UOSInt srcWidth, UOSInt srcHeight)
{
	this->srcImgPtr = srcImgPtr;
	this->srcBpl = srcBpl;
	this->srcWidth = srcWidth;
	this->srcHeight = srcHeight;
	this->srcPF = Media::PF_B8G8R8A8;
	this->srcPal = 0;
}

void Media::ImageRemapper::SetSourceImage(NN<Media::StaticImage> srcImg)
{
	this->srcImgPtr = UnsafeArray<const UInt8>(srcImg->data);
	this->srcBpl = srcImg->GetDataBpl();
	this->srcWidth = srcImg->info.dispSize.x;
	this->srcHeight = srcImg->info.dispSize.y;
	this->srcPF = srcImg->info.pf;
	this->srcPal = srcImg->pal;
}

Bool Media::ImageRemapper::Remap(UnsafeArray<UInt8> destImgPtr, UOSInt destBpl, UOSInt destWidth, UOSInt destHeight, Math::Quadrilateral destQuad)
{
	UnsafeArray<const UInt8> srcImgPtr;
	if (!this->srcImgPtr.SetTo(srcImgPtr) || this->srcWidth == 0 || this->srcHeight == 0 || destWidth == 0 || destHeight == 0)
	{
		return false;
	}
	this->destBpl = destBpl;
	this->destWidth = destWidth;
	this->destHeight = destHeight;
	this->destQuad = destQuad;
	this->DoRemap(srcImgPtr, destImgPtr);
	return true;
}
