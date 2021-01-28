#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ImageCmp/ImagePSNR.h"
#include "Math/Math.h"

Media::ImageCmp::ImagePSNR::ImagePSNR()
{
	NEW_CLASS(msr, Media::ImageCmp::ImageMeanSqrErr());
}

Media::ImageCmp::ImagePSNR::~ImagePSNR()
{
	DEL_CLASS(msr);
}

Double Media::ImageCmp::ImagePSNR::CompareImage(Media::Image *oriImage, Media::Image *cmpImage)
{
	Double val = msr->CompareImage(oriImage, cmpImage);
	if (val < 0)
	{
		return -2;
	}
	else if (val == 0)
	{
		return -1;
	}
	Int32 nBits = 8;
	if (oriImage->info->fourcc == 0 || oriImage->info->fourcc == *(Int32*)"DIB")
	{
		if (oriImage->info->bpp >= 48)
		{
			nBits = 16;
		}
	}
	else if (oriImage->info->fourcc == *(Int32*)"LRGB")
	{
		nBits = 14;
	}
	return 20 * Math::Log10((Math::Pow(2, nBits) - 1) / Math::Sqrt(val));
}
