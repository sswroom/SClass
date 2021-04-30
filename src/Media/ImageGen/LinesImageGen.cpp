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

const UTF8Char *Media::ImageGen::LinesImageGen::GetName()
{
	return (const UTF8Char*)"Lines Image";
}

Media::Image *Media::ImageGen::LinesImageGen::GenerateImage(Media::ColorProfile *colorProfile, UOSInt width, UOSInt height)
{
	Media::StaticImage *outImage;
	Int64 *imgPtr;
	UInt8 *imgPtr2;
	UInt16 c[4];
	UOSInt i;
	UOSInt j;
	Double v;
	if (width < 8 || height < 4)
		return 0;
	Media::CS::TransferFunc *rfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetRTranParam());
	Media::CS::TransferFunc *gfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetGTranParam());
	Media::CS::TransferFunc *bfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetBTranParam());
	NEW_CLASS(outImage, Media::StaticImage(width, height, 0, 64, Media::PF_LE_B16G16R16A16, 0, colorProfile, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));

	imgPtr2 = outImage->data;
	
	j = 0;
	while (j < height)
	{
		imgPtr = (Int64*)imgPtr2;
		i = 0;
		while (i < width)
		{
			v = 0.5 + Math::Sin(Math::UOSInt2Double(i)) * 0.25 + Math::Sin(Math::UOSInt2Double(j)) * 0.25;
			c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(v) * 65535.0);
			c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(v) * 65535.0);
			c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(v) * 65535.0);
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
