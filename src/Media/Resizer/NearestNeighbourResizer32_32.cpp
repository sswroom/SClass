#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/Resizer/NearestNeighbourResizer32_32.h"

extern "C"
{
	void NearestNeighbourResizer32_32_Resize(UInt8 *inPt, UInt8 *outPt, OSInt dwidth, OSInt dheight, OSInt dbpl, OSInt *xindex, OSInt *yindex);
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

void Media::Resizer::NearestNeighbourResizer32_32::Resize(UInt8 *src, OSInt sbpl, Double swidth, Double sheight, Double xOfst, Double yOfst, UInt8 *dest, OSInt dbpl, UOSInt dwidth, UOSInt dheight)
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
			if (Math::UOSInt2Double(j) >= swidth)
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
			if (Math::UOSInt2Double(j) >= sheight)
			{
				j = (UOSInt)sheight - 1;
			}
			this->yindex[i] = j * sbpl;
			i++;
		}
	}
	NearestNeighbourResizer32_32_Resize(src, dest, dwidth, dheight, dbpl, this->xindex, this->yindex);
}

Bool Media::Resizer::NearestNeighbourResizer32_32::Resize(Media::StaticImage *srcImg, Media::StaticImage *destImg)
{
	if (srcImg->info->fourcc != 0 && srcImg->info->fourcc != *(UInt32*)"DIB")
		return false;
	if (destImg->info->fourcc != 0 && destImg->info->fourcc != *(UInt32*)"DIB")
		return false;
	if (srcImg->info->pf != Media::PF_B8G8R8A8 || destImg->info->pf != Media::PF_B8G8R8A8)
		return false;
	destImg->info->color->Set(srcImg->info->color);
	if (srcImg->info->fourcc == destImg->info->fourcc)
	{
		Resize(srcImg->data, srcImg->GetDataBpl(), Math::OSInt2Double(srcImg->info->dispWidth), Math::OSInt2Double(srcImg->info->dispHeight), 0, 0, destImg->data, destImg->GetDataBpl(), destImg->info->dispWidth, destImg->info->dispHeight);
		return true;
	}
	else
	{
		OSInt dbpl = destImg->GetDataBpl();
		Resize(srcImg->data, srcImg->GetDataBpl(), Math::OSInt2Double(srcImg->info->dispWidth), Math::OSInt2Double(srcImg->info->dispHeight), 0, 0, destImg->data + (destImg->info->storeHeight - 1) * dbpl, -dbpl, destImg->info->dispWidth, destImg->info->dispHeight);
		return true;
	}
}

Bool Media::Resizer::NearestNeighbourResizer32_32::IsSupported(Media::FrameInfo *srcInfo)
{
	if (srcInfo->fourcc != 0)
		return false;
	if (srcInfo->pf != Media::PF_B8G8R8A8)
		return false;
	return true;
}

Media::StaticImage *Media::Resizer::NearestNeighbourResizer32_32::ProcessToNewPartial(Media::StaticImage *srcImage, Double srcX1, Double srcY1, Double srcX2, Double srcY2)
{
	Media::FrameInfo destInfo;
	Media::StaticImage *newImage;
	if (!IsSupported(srcImage->info))
	{
		return 0;
	}
	OSInt targetWidth = this->targetWidth;
	OSInt targetHeight = this->targetHeight;
	if (targetWidth == 0)
	{
		targetWidth = Math::Double2Int32(srcX2 - srcX1);
	}
	if (targetHeight == 0)
	{
		targetHeight = Math::Double2Int32(srcY2 - srcY1);
	}
	CalOutputSize(srcImage->info, targetWidth, targetHeight, &destInfo, this->rar);
	destInfo.color->Set(srcImage->info->color);;
	destInfo.atype = srcImage->info->atype;
	NEW_CLASS(newImage, Media::StaticImage(&destInfo));
	Int32 tlx = (Int32)srcX1;
	Int32 tly = (Int32)srcY1;
	Resize(srcImage->data + (tlx << 2) + tly * srcImage->GetDataBpl(), srcImage->GetDataBpl(), srcX2 - srcX1, srcY2 - srcY1, srcX1 - tlx, srcY1 - tly, newImage->data, newImage->GetDataBpl(), newImage->info->dispWidth, newImage->info->dispHeight);
	return newImage;

}
