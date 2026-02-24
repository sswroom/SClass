#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/Resizer/NearestNeighbourResizer32_32.h"

extern "C"
{
	void NearestNeighbourResizer32_32_Resize(const UInt8 *inPt, UInt8 *outPt, UIntOS dwidth, UIntOS dheight, IntOS dbpl, IntOS *xindex, IntOS *yindex);
}

Media::Resizer::NearestNeighbourResizer32_32::NearestNeighbourResizer32_32() : Media::ImageResizer(Media::AT_IGNORE_ALPHA)
{
	this->lastsbpl = 0;
	this->lastsheight = 0;
	this->lastswidth = 0;
	this->lastdheight = 0;
	this->lastdwidth = 0;
	this->xindex = nullptr;
	this->yindex = nullptr;
}

Media::Resizer::NearestNeighbourResizer32_32::~NearestNeighbourResizer32_32()
{
	UnsafeArray<IntOS> xindex;
	UnsafeArray<IntOS> yindex;
	if (this->xindex.SetTo(xindex))
	{
		MemFreeArr(xindex);
		this->xindex = nullptr;
	}
	if (this->yindex.SetTo(yindex))
	{
		MemFreeArr(yindex);
		this->yindex = nullptr;
	}
}

void Media::Resizer::NearestNeighbourResizer32_32::Resize(UnsafeArray<const UInt8> src, IntOS sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, IntOS dbpl, UIntOS dwidth, UIntOS dheight)
{
	UnsafeArray<IntOS> xindex;
	UnsafeArray<IntOS> yindex;
	UIntOS i;
	UIntOS j;
	Double v;
	Double currV;
	if (this->lastswidth != swidth || this->lastdwidth != dwidth || !this->xindex.SetTo(xindex))
	{
		if (this->xindex.SetTo(xindex))
		{
			MemFreeArr(xindex);
		}
		this->xindex = xindex = MemAlloc(IntOS, dwidth);
		this->lastswidth = swidth;
		this->lastdwidth = dwidth;
		v = swidth / (Double)dwidth;
		currV = xOfst;
		i = 0;
		while (i < dwidth)
		{
			j = (UIntOS)currV;
			currV += v;
			if (UIntOS2Double(j) >= swidth)
			{
				j = (UIntOS)swidth - 1;
			}
			xindex[i] = (IntOS)j << 2;
			i++;
		}
	}
	if (this->lastsheight != sheight || this->lastdheight != dheight || this->lastsbpl != sbpl || !this->yindex.SetTo(yindex))
	{
		if (this->yindex.SetTo(yindex))
		{
			MemFreeArr(yindex);
		}
		this->yindex = yindex = MemAlloc(IntOS, dheight);
		this->lastsheight = sheight;
		this->lastdheight = dheight;
		this->lastsbpl = sbpl;
		v = sheight / (Double)dheight;
		currV = yOfst;
		i = 0;
		while (i < dheight)
		{
			j = (UIntOS)currV;
			currV += v;
			if (UIntOS2Double(j) >= sheight)
			{
				j = (UIntOS)sheight - 1;
			}
			yindex[i] = (IntOS)j * sbpl;
			i++;
		}
	}
	NearestNeighbourResizer32_32_Resize(src.Ptr(), dest.Ptr(), dwidth, dheight, dbpl, xindex.Ptr(), yindex.Ptr());
}

Bool Media::Resizer::NearestNeighbourResizer32_32::Resize(NN<const Media::StaticImage> srcImg, NN<Media::StaticImage> destImg)
{
	if (srcImg->info.fourcc != 0 && srcImg->info.fourcc != *(UInt32*)"DIB")
		return false;
	if (destImg->info.fourcc != 0 && destImg->info.fourcc != *(UInt32*)"DIB")
		return false;
	if (srcImg->info.pf != Media::PF_B8G8R8A8 || destImg->info.pf != Media::PF_B8G8R8A8)
		return false;
	destImg->info.color.Set(srcImg->info.color);
	if (srcImg->info.fourcc == destImg->info.fourcc)
	{
		Resize(srcImg->data, (IntOS)srcImg->GetDataBpl(), UIntOS2Double(srcImg->info.dispSize.x), UIntOS2Double(srcImg->info.dispSize.y), 0, 0, destImg->data, (IntOS)destImg->GetDataBpl(), destImg->info.dispSize.x, destImg->info.dispSize.y);
		return true;
	}
	else
	{
		IntOS dAdd = (IntOS)destImg->GetDataBpl();
		Resize(srcImg->data, (IntOS)srcImg->GetDataBpl(), UIntOS2Double(srcImg->info.dispSize.x), UIntOS2Double(srcImg->info.dispSize.y), 0, 0, destImg->data + (IntOS)(destImg->info.storeSize.y - 1) * dAdd, -dAdd, destImg->info.dispSize.x, destImg->info.dispSize.y);
		return true;
	}
}

Bool Media::Resizer::NearestNeighbourResizer32_32::IsSupported(NN<const Media::FrameInfo> srcInfo)
{
	if (srcInfo->fourcc != 0)
		return false;
	if (srcInfo->pf != Media::PF_B8G8R8A8)
		return false;
	return true;
}

Optional<Media::StaticImage> Media::Resizer::NearestNeighbourResizer32_32::ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR)
{
	Media::FrameInfo destInfo;
	Media::StaticImage *newImage;
	if (srcImage->GetImageClass() != Media::RasterImage::ImageClass::StaticImage || !IsSupported(srcImage->info))
	{
		return nullptr;
	}
	Math::Size2D<UIntOS> targetSize = this->targetSize;
	if (targetSize.x == 0)
	{
		targetSize.x = (UIntOS)Double2IntOS(srcBR.x - srcTL.x);
	}
	if (targetSize.y == 0)
	{
		targetSize.y = (UIntOS)Double2IntOS(srcBR.y - srcTL.y);
	}
	CalOutputSize(srcImage->info, targetSize, destInfo, this->rar);
	destInfo.color.Set(srcImage->info.color);;
	destInfo.atype = srcImage->info.atype;
	NEW_CLASS(newImage, Media::StaticImage(destInfo));
	Int32 tlx = (Int32)srcTL.x;
	Int32 tly = (Int32)srcTL.y;
	Resize(((Media::StaticImage*)srcImage.Ptr())->data + (tlx << 2) + tly * (IntOS)srcImage->GetDataBpl(), (IntOS)srcImage->GetDataBpl(), srcBR.x - srcTL.x, srcBR.y - srcTL.y, srcTL.x - tlx, srcTL.y - tly, newImage->data, (IntOS)newImage->GetDataBpl(), newImage->info.dispSize.x, newImage->info.dispSize.y);
	return newImage;

}
