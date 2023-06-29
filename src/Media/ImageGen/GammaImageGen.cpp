#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/StaticImage.h"
#include "Media/CS/TransferFunc.h"
#include "Media/ImageGen/GammaImageGen.h"

Media::ImageGen::GammaImageGen::GammaImageGen()
{
}

Media::ImageGen::GammaImageGen::~GammaImageGen()
{
}

Text::CString Media::ImageGen::GammaImageGen::GetName()
{
	return CSTR("Gamma Test");
}

Media::Image *Media::ImageGen::GammaImageGen::GenerateImage(Media::ColorProfile *colorProfile, Math::Size2D<UOSInt> size)
{
	Media::StaticImage *outImage;
	UInt8 *imgPtr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt bpl = size.x << 3;
	if (size.x < 2 || size.y < 2)
		return 0;
	Media::CS::TransferFunc *rfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetRTranParam());
	Media::CS::TransferFunc *gfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetGTranParam());
	Media::CS::TransferFunc *bfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetBTranParam());
	NEW_CLASS(outImage, Media::StaticImage(size, 0, 32, Media::PF_B8G8R8A8, 0, colorProfile, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	imgPtr = (UInt8*)outImage->data;
	i = size.x >> 1;
	j = 0;
	while (j < i)
	{
		imgPtr[0] = (UInt8)Double2Int32(bfunc->ForwardTransfer(UOSInt2Double(j) / UOSInt2Double(i - 1)) * 255.0);
		imgPtr[1] = (UInt8)Double2Int32(gfunc->ForwardTransfer(UOSInt2Double(j) / UOSInt2Double(i - 1)) * 255.0);
		imgPtr[2] = (UInt8)Double2Int32(rfunc->ForwardTransfer(UOSInt2Double(j) / UOSInt2Double(i - 1)) * 255.0);
		imgPtr[3] = 0xff;
		imgPtr += 4;

		j++;
	}
	i = size.x - j;
	k = size.x - j - 1;
	j = 0;
	while (j < i)
	{
		imgPtr[0] = (UInt8)Double2Int32(bfunc->ForwardTransfer(UOSInt2Double(j) / UOSInt2Double(k)) * 255.0);
		imgPtr[1] = (UInt8)Double2Int32(gfunc->ForwardTransfer(UOSInt2Double(j) / UOSInt2Double(k)) * 255.0);
		imgPtr[2] = (UInt8)Double2Int32(rfunc->ForwardTransfer(UOSInt2Double(j) / UOSInt2Double(k)) * 255.0);
		imgPtr[3] = 0xff;
		imgPtr += 4;

		j++;
	}

	i = size.x >> 1;
	j = 0;
	while (j < i)
	{
		imgPtr[0] = (UInt8)Double2Int32(bfunc->ForwardTransfer(1 - UOSInt2Double(j) / UOSInt2Double(i - 1)) * 255.0);
		imgPtr[1] = (UInt8)Double2Int32(gfunc->ForwardTransfer(1 - UOSInt2Double(j) / UOSInt2Double(i - 1)) * 255.0);
		imgPtr[2] = (UInt8)Double2Int32(rfunc->ForwardTransfer(1 - UOSInt2Double(j) / UOSInt2Double(i - 1)) * 255.0);
		imgPtr[3] = 0xff;
		imgPtr += 4;

		j++;
	}
	i = size.x - j;
	k = size.x - j - 1;
	j = 0;
	while (j < i)
	{
		imgPtr[0] = (UInt8)Double2Int32(bfunc->ForwardTransfer(1 - UOSInt2Double(j) / UOSInt2Double(k)) * 255.0);
		imgPtr[1] = (UInt8)Double2Int32(gfunc->ForwardTransfer(1 - UOSInt2Double(j) / UOSInt2Double(k)) * 255.0);
		imgPtr[2] = (UInt8)Double2Int32(rfunc->ForwardTransfer(1 - UOSInt2Double(j) / UOSInt2Double(k)) * 255.0);
		imgPtr[3] = 0xff;
		imgPtr += 4;

		j++;
	}
	i = 1;
	j = size.y >> 1;
	while (i < j)
	{
		MemCopyNO(imgPtr, outImage->data, bpl);
		imgPtr += bpl;
		i++;
	}
	if (size.y & 1)
	{
		MemCopyNO(imgPtr, outImage->data, bpl >> 1);
	}
	DEL_CLASS(bfunc);
	DEL_CLASS(gfunc);
	DEL_CLASS(rfunc);
	return outImage;
}
