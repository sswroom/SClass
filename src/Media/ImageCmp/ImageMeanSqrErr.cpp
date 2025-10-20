#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ImageCmp/ImageMeanSqrErr.h"

Media::ImageCmp::ImageMeanSqrErr::ImageMeanSqrErr()
{
}

Media::ImageCmp::ImageMeanSqrErr::~ImageMeanSqrErr()
{
}

Double Media::ImageCmp::ImageMeanSqrErr::CompareImage(NN<Media::RasterImage> oriImage, NN<Media::RasterImage> cmpImage)
{
	if (oriImage->info.fourcc != cmpImage->info.fourcc)
		return -1;
	if (oriImage->info.storeBPP != cmpImage->info.storeBPP)
		return -1;
	if (oriImage->info.dispSize.GetWidth() != cmpImage->info.dispSize.GetWidth() || oriImage->info.dispSize.GetHeight() != cmpImage->info.dispSize.GetHeight())
		return -1;
	if (oriImage->info.fourcc == 0 || oriImage->info.fourcc == *(Int32*)"DIB")
	{
		Int64 diffSum = 0;
		if (oriImage->info.storeBPP == 32)
		{
			UOSInt w = oriImage->info.dispSize.GetWidth();
			UOSInt bpl = w << 2;
			UInt8 *srcImg;
			UInt8 *destImg;
			UOSInt i = oriImage->info.dispSize.GetHeight();
			srcImg = MemAlloc(UInt8, bpl);
			destImg = MemAlloc(UInt8, bpl);
			UInt8 *srcPtr;
			UInt8 *destPtr;
			UOSInt j;
			OSInt db;
			OSInt dg;
			OSInt dr;
			while (i-- > 0)
			{
				oriImage->GetRasterData(srcImg, 0, (OSInt)i, w, 1, bpl, false, Media::RotateType::None);
				cmpImage->GetRasterData(destImg, 0, (OSInt)i, w, 1, bpl, false, Media::RotateType::None);
				srcPtr = srcImg;
				destPtr = destImg;
				j = w;
				while (j-- > 0)
				{
					db = srcPtr[0] - destPtr[0];
					dg = srcPtr[1] - destPtr[1];
					dr = srcPtr[2] - destPtr[2];
					diffSum += dr * dr + dg * dg + db * db;
					srcPtr += 4;
					destPtr += 4;
				}
			}
			MemFree(srcImg);
			MemFree(destImg);
			return diffSum / (Double)(w * oriImage->info.dispSize.GetHeight() * 3);
		}
		return -1;
	}
	else
	{
		return -1;
	}
}
