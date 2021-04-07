#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/StaticImage.h"
#include "Media/ImageGen/Gradian32ImageGen.h"

Media::ImageGen::Gradian32ImageGen::Gradian32ImageGen()
{
}

Media::ImageGen::Gradian32ImageGen::~Gradian32ImageGen()
{
}

const UTF8Char *Media::ImageGen::Gradian32ImageGen::GetName()
{
	return (const UTF8Char*)"Gradian Test 32-bit";
}

Media::Image *Media::ImageGen::Gradian32ImageGen::GenerateImage(Media::ColorProfile *colorProfile, UOSInt width, UOSInt height)
{
	Media::StaticImage *outImage;
	UInt8 *imgPtr;
	OSInt i;
	OSInt j;
	UInt8 hv;
	UInt8 vv;
	OSInt bpl;
	if (width < 16 || height < 16)
		return 0;
//	Media::CS::TransferFunc *rfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetRTranParam());
//	Media::CS::TransferFunc *gfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetGTranParam());
//	Media::CS::TransferFunc *bfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetBTranParam());
	NEW_CLASS(outImage, Media::StaticImage(width, height, 0, 32, Media::PF_B8G8R8A8, 0, colorProfile, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	bpl = outImage->GetDataBpl();
	imgPtr = outImage->data;
	i = height;
	while (i-- > 0)
	{
		vv = (UInt8)((i << 4) / height);
		j = 0;
		while (j < width)
		{
			hv = (UInt8)((j << 4) / width);
			imgPtr[(j << 2)] = hv + (vv << 4);
			imgPtr[(j << 2) + 1] = hv + (vv << 4);
			imgPtr[(j << 2) + 2] = hv + (vv << 4);
			imgPtr[(j << 2) + 3] = 0xff;
			j++;
		}
		imgPtr += bpl;
	}
//	DEL_CLASS(bfunc);
//	DEL_CLASS(gfunc);
//	DEL_CLASS(rfunc);
	return outImage;
}
