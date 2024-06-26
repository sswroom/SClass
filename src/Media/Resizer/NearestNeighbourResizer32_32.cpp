#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/Resizer/NearestNeighbourResizer32_32.h"

extern "C"
{
	void NearestNeighbourResizer32_32_Resize(const UInt8 *inPt, UInt8 *outPt, UOSInt dwidth, UOSInt dheight, OSInt dbpl, OSInt *xindex, OSInt *yindex);
}

Media::Resizer::NearestNeighbourResizer32_32::NearestNeighbourResizer32_32() : Media::IImgResizer(Media::AT_NO_ALPHA)
{
	this->lastsbpl = 0;
	this->lastsheight = 0;
	this->lastswidth = 0;
	this->lastdheight = 0;
	this->lastdwidth = 0;
	this->xindex = 0;
	this->yindex = 0;
}

Media::Resizer::NearestNeighbourResizer32_32::~NearestNeighbourResizer32_32()
{
	if (this->xindex)
	{
		MemFree(this->xindex);
		this->xindex = 0;
	}
	if (this->yindex)
	{
		MemFree(this->yindex);
		this->yindex = 0;
	}
}

void Media::Resizer::NearestNeighbourResizer32_32::Resize(UnsafeArray<const UInt8> src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UnsafeArray<UInt8> dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight)
{
	UOSInt i;
	UOSInt j;
	Double v;
	Double currV;
	if (this->lastswidth != swidth || this->lastdwidth != dwidth)
	{
		if (this->xindex)
		{
			MemFree(this->xindex);
		}
		this->xindex = MemAlloc(OSInt, dwidth);
		this->lastswidth = swidth;
		this->lastdwidth = dwidth;
		v = swidth / (Double)dwidth;
		currV = xOfst;
		i = 0;
		while (i < dwidth)
		{
			j = (UOSInt)currV;
			currV += v;
			if (UOSInt2Double(j) >= swidth)
			{
				j = (UOSInt)swidth - 1;
			}
			this->xindex[i] = (OSInt)j << 2;
			i++;
		}
	}
	if (this->lastsheight != sheight || this->lastdheight != dheight || this->lastsbpl != sbpl)
	{
		if (this->yindex)
		{
			MemFree(this->yindex);
		}
		this->yindex = MemAlloc(OSInt, dheight);
		this->lastsheight = sheight;
		this->lastdheight = dheight;
		this->lastsbpl = sbpl;
		v = sheight / (Double)dheight;
		currV = yOfst;
		i = 0;
		while (i < dheight)
		{
			j = (UOSInt)currV;
			currV += v;
			if (UOSInt2Double(j) >= sheight)
			{
				j = (UOSInt)sheight - 1;
			}
			this->yindex[i] = (OSInt)j * sbpl;
			i++;
		}
	}
	NearestNeighbourResizer32_32_Resize(src.Ptr(), dest.Ptr(), dwidth, dheight, dbpl, this->xindex, this->yindex);
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
		Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(srcImg->info.dispSize.x), UOSInt2Double(srcImg->info.dispSize.y), 0, 0, destImg->data, (OSInt)destImg->GetDataBpl(), destImg->info.dispSize.x, destImg->info.dispSize.y);
		return true;
	}
	else
	{
		OSInt dAdd = (OSInt)destImg->GetDataBpl();
		Resize(srcImg->data, (OSInt)srcImg->GetDataBpl(), UOSInt2Double(srcImg->info.dispSize.x), UOSInt2Double(srcImg->info.dispSize.y), 0, 0, destImg->data + (OSInt)(destImg->info.storeSize.y - 1) * dAdd, -dAdd, destImg->info.dispSize.x, destImg->info.dispSize.y);
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

Media::StaticImage *Media::Resizer::NearestNeighbourResizer32_32::ProcessToNewPartial(NN<const Media::RasterImage> srcImage, Math::Coord2DDbl srcTL, Math::Coord2DDbl srcBR)
{
	Media::FrameInfo destInfo;
	Media::StaticImage *newImage;
	if (srcImage->GetImageType() != Media::RasterImage::ImageType::Static || !IsSupported(srcImage->info))
	{
		return 0;
	}
	Math::Size2D<UOSInt> targetSize = this->targetSize;
	if (targetSize.x == 0)
	{
		targetSize.x = (UOSInt)Double2OSInt(srcBR.x - srcTL.x);
	}
	if (targetSize.y == 0)
	{
		targetSize.y = (UOSInt)Double2OSInt(srcBR.y - srcTL.y);
	}
	CalOutputSize(srcImage->info, targetSize, destInfo, this->rar);
	destInfo.color.Set(srcImage->info.color);;
	destInfo.atype = srcImage->info.atype;
	NEW_CLASS(newImage, Media::StaticImage(destInfo));
	Int32 tlx = (Int32)srcTL.x;
	Int32 tly = (Int32)srcTL.y;
	Resize(((Media::StaticImage*)srcImage.Ptr())->data + (tlx << 2) + tly * (OSInt)srcImage->GetDataBpl(), (OSInt)srcImage->GetDataBpl(), srcBR.x - srcTL.x, srcBR.y - srcTL.y, srcTL.x - tlx, srcTL.y - tly, newImage->data, (OSInt)newImage->GetDataBpl(), newImage->info.dispSize.x, newImage->info.dispSize.y);
	return newImage;

}
