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

Text::CStringNN Media::ImageGen::RingsImageGen::GetName() const
{
	return CSTR("Rings Image");
}

Optional<Media::RasterImage> Media::ImageGen::RingsImageGen::GenerateImage(NN<const Media::ColorProfile> colorProfile, Math::Size2D<UOSInt> size)
{
	Media::StaticImage *outImage;
	UnsafeArray<Int64> imgPtr;
	UnsafeArray<UInt8> imgPtr2;
	UInt16 c[4];
	UOSInt i;
	UOSInt j;
	Double v;
	Double dx;
	Double dy;
	Double dwidth = UOSInt2Double(size.x);
	Double dheight = UOSInt2Double(size.y);
	Double d = dwidth;
	if (dheight > d)
	{
		d = dheight;
	}
	if (size.x < 8 || size.x < 4)
		return 0;
	NN<Media::CS::TransferFunc> rfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetRTranParamRead());
	NN<Media::CS::TransferFunc> gfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetGTranParamRead());
	NN<Media::CS::TransferFunc> bfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetBTranParamRead());
	NEW_CLASS(outImage, Media::StaticImage(size, 0, 64, Media::PF_LE_B16G16R16A16, 0, colorProfile, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA_ALL_FF, Media::YCOFST_C_CENTER_LEFT));

	imgPtr2 = outImage->data;
	j = 0;
	while (j < size.y)
	{
		imgPtr = UnsafeArray<Int64>::ConvertFrom(imgPtr2);
		i = 0;
		while (i < size.x)
		{
			dx = (dwidth * 0.5) - UOSInt2Double(i) - 0.5;
			dy = (dheight * 0.5) - UOSInt2Double(j) - 0.5;
			v = 0.5 + Math_Sin((dx * dx + dy * dy) / (d / 2)) * 0.5;
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
	bfunc.Delete();
	gfunc.Delete();
	rfunc.Delete();
	return outImage;
}
