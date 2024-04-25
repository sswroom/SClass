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

Text::CStringNN Media::ImageGen::Gradian32ImageGen::GetName() const
{
	return CSTR("Gradian Test 32-bit");
}

Media::RasterImage *Media::ImageGen::Gradian32ImageGen::GenerateImage(NN<const Media::ColorProfile> colorProfile, Math::Size2D<UOSInt> size)
{
	Media::StaticImage *outImage;
	UInt8 *imgPtr;
	UOSInt i;
	UOSInt j;
	UInt8 hv;
	UInt8 vv;
	UOSInt bpl;
	if (size.x < 16 || size.y < 16)
		return 0;
//	Media::CS::TransferFunc *rfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetRTranParam());
//	Media::CS::TransferFunc *gfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetGTranParam());
//	Media::CS::TransferFunc *bfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetBTranParam());
	NEW_CLASS(outImage, Media::StaticImage(size, 0, 32, Media::PF_B8G8R8A8, 0, colorProfile, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	bpl = outImage->GetDataBpl();
	imgPtr = outImage->data;
	i = size.y;
	while (i-- > 0)
	{
		vv = (UInt8)((i << 4) / size.y);
		j = 0;
		while (j < size.x)
		{
			hv = (UInt8)((j << 4) / size.x);
			imgPtr[(j << 2)] = (UInt8)(hv + (vv << 4));
			imgPtr[(j << 2) + 1] = (UInt8)(hv + (vv << 4));
			imgPtr[(j << 2) + 2] = (UInt8)(hv + (vv << 4));
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
