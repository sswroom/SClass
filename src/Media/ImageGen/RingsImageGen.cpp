#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/StaticImage.h"
#include "Media/CS/TransferFunc.h"
#include "Media/ImageGen/RingsImageGen.h"

Media::ImageGen::RingsImageGen::RingsImageGen()
{
}

Media::ImageGen::RingsImageGen::~RingsImageGen()
{
}

const UTF8Char *Media::ImageGen::RingsImageGen::GetName()
{
	return (const UTF8Char*)"Rings Image";
}

Media::Image *Media::ImageGen::RingsImageGen::GenerateImage(Media::ColorProfile *colorProfile, OSInt width, OSInt height)
{
	Media::StaticImage *outImage;
	Int64 *imgPtr;
	UInt8 *imgPtr2;
	UInt16 c[4];
	OSInt i;
	OSInt j;
	Double v;
	Double dx;
	Double dy;
	Double d = Math::OSInt2Double(width);
	if (height > d)
	{
		d = Math::OSInt2Double(height);
	}
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
			dx = (width * 0.5) - i - 0.5;
			dy = (height * 0.5) - j - 0.5;
			v = 0.5 + Math::Sin((dx * dx + dy * dy) / (d / 2)) * 0.5;
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
