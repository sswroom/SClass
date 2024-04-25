#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/StaticImage.h"
#include "Media/CS/TransferFunc.h"
#include "Media/ImageGen/Gamma64ImageGen.h"

Media::ImageGen::Gamma64ImageGen::Gamma64ImageGen()
{
}

Media::ImageGen::Gamma64ImageGen::~Gamma64ImageGen()
{
}

Text::CStringNN Media::ImageGen::Gamma64ImageGen::GetName() const
{
	return CSTR("Gamma Test 64-bit");
}

Media::RasterImage *Media::ImageGen::Gamma64ImageGen::GenerateImage(NN<const Media::ColorProfile> colorProfile, Math::Size2D<UOSInt> size)
{
	Media::StaticImage *outImage;
	UInt16 *imgPtr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt bpl = size.x << 4;
	if (size.x < 2 || size.y < 2)
		return 0;
	NN<Media::CS::TransferFunc> rfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetRTranParamRead());
	NN<Media::CS::TransferFunc> gfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetGTranParamRead());
	NN<Media::CS::TransferFunc> bfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetBTranParamRead());
	NEW_CLASS(outImage, Media::StaticImage(size, 0, 64, Media::PF_LE_B16G16R16A16, 0, colorProfile, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	imgPtr = (UInt16*)outImage->data;
	i = size.x >> 1;
	j = 0;
	while (j < i)
	{
		imgPtr[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(UOSInt2Double(j) / UOSInt2Double(i - 1)) * 65535.0);
		imgPtr[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(UOSInt2Double(j) / UOSInt2Double(i - 1)) * 65535.0);
		imgPtr[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(UOSInt2Double(j) / UOSInt2Double(i - 1)) * 65535.0);
		imgPtr[3] = 0xffff;
		imgPtr += 4;

		j++;
	}
	i = size.x - j;
	k = size.x - j - 1;
	j = 0;
	while (j < i)
	{
		imgPtr[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(UOSInt2Double(j) / UOSInt2Double(k)) * 65535.0);
		imgPtr[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(UOSInt2Double(j) / UOSInt2Double(k)) * 65535.0);
		imgPtr[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(UOSInt2Double(j) / UOSInt2Double(k)) * 65535.0);
		imgPtr[3] = 0xffff;
		imgPtr += 4;

		j++;
	}

	i = size.x >> 1;
	j = 0;
	while (j < i)
	{
		imgPtr[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(1 - UOSInt2Double(j) / UOSInt2Double(i - 1)) * 65535.0);
		imgPtr[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(1 - UOSInt2Double(j) / UOSInt2Double(i - 1)) * 65535.0);
		imgPtr[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(1 - UOSInt2Double(j) / UOSInt2Double(i - 1)) * 65535.0);
		imgPtr[3] = 0xffff;
		imgPtr += 4;

		j++;
	}
	i = size.x - j;
	k = size.x - j - 1;
	j = 0;
	while (j < i)
	{
		imgPtr[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(1 - UOSInt2Double(j) / UOSInt2Double(k)) * 65535.0);
		imgPtr[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(1 - UOSInt2Double(j) / UOSInt2Double(k)) * 65535.0);
		imgPtr[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(1 - UOSInt2Double(j) / UOSInt2Double(k)) * 65535.0);
		imgPtr[3] = 0xffff;
		imgPtr += 4;

		j++;
	}
	i = 1;
	j = size.y >> 1;
	while (i < j)
	{
		MemCopyNO(imgPtr, outImage->data, bpl);
		imgPtr += bpl >> 1;
		i++;
	}
	if (size.y & 1)
	{
		MemCopyNO(imgPtr, outImage->data, bpl >> 1);
	}
	bfunc.Delete();
	gfunc.Delete();
	rfunc.Delete();
	return outImage;
}
