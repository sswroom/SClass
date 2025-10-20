#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ImageCmp/ImagePSNR.h"
#include "Math/Math.h"

Media::ImageCmp::ImagePSNR::ImagePSNR()
{
}

Media::ImageCmp::ImagePSNR::~ImagePSNR()
{
}

Double Media::ImageCmp::ImagePSNR::CompareImage(NN<Media::RasterImage> oriImage, NN<Media::RasterImage> cmpImage)
{
	Double val = this->msr.CompareImage(oriImage, cmpImage);
	if (val < 0)
	{
		return -2;
	}
	else if (val == 0)
	{
		return -1;
	}
	Int32 nBits = 8;
	if (oriImage->info.fourcc == 0 || oriImage->info.fourcc == *(Int32*)"DIB")
	{
		if (oriImage->info.storeBPP >= 48)
		{
			nBits = 16;
		}
	}
	else if (oriImage->info.fourcc == *(Int32*)"LRGB")
	{
		nBits = 14;
	}
	return 20 * Math_Log10((Math_Pow(2, nBits) - 1) / Math_Sqrt(val));
}
