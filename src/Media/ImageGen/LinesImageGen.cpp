#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/StaticImage.h"
#include "Media/CS/TransferFunc.h"
#include "Media/ImageGen/LinesImageGen.h"

Media::ImageGen::LinesImageGen::LinesImageGen()
{
}

Media::ImageGen::LinesImageGen::~LinesImageGen()
{
}

Text::CStringNN Media::ImageGen::LinesImageGen::GetName() const
{
	return CSTR("Lines Image");
}

Media::Image *Media::ImageGen::LinesImageGen::GenerateImage(NotNullPtr<const Media::ColorProfile> colorProfile, Math::Size2D<UOSInt> size)
{
	Media::StaticImage *outImage;
	Int64 *imgPtr;
	UInt8 *imgPtr2;
	UInt16 c[4];
	UOSInt i;
	UOSInt j;
	Double v;
	if (size.x < 8 || size.y < 4)
		return 0;
	Media::CS::TransferFunc *rfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetRTranParamRead());
	Media::CS::TransferFunc *gfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetGTranParamRead());
	Media::CS::TransferFunc *bfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetBTranParamRead());
	NEW_CLASS(outImage, Media::StaticImage(size, 0, 64, Media::PF_LE_B16G16R16A16, 0, colorProfile, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));

	imgPtr2 = outImage->data;
	
	j = 0;
	while (j < size.y)
	{
		imgPtr = (Int64*)imgPtr2;
		i = 0;
		while (i < size.x)
		{
			v = 0.5 + Math_Sin(UOSInt2Double(i)) * 0.25 + Math_Sin(UOSInt2Double(j)) * 0.25;
			c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(v) * 65535.0);
			c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(v) * 65535.0);
			c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(v) * 65535.0);
			c[3] = 65535;

			*imgPtr++ = *(Int64*)c;
			i++;
		}

		imgPtr2 += outImage->GetDataBpl();
		j++;
	}
	DEL_CLASS(bfunc);
	DEL_CLASS(gfunc);
	DEL_CLASS(rfunc);
	return outImage;
}
