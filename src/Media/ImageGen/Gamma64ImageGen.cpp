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

const UTF8Char *Media::ImageGen::Gamma64ImageGen::GetName()
{
	return (const UTF8Char*)"Gamma Test 64-bit";
}

Media::Image *Media::ImageGen::Gamma64ImageGen::GenerateImage(Media::ColorProfile *colorProfile, UOSInt width, UOSInt height)
{
	Media::StaticImage *outImage;
	UInt16 *imgPtr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt bpl = width << 4;
	if (width < 2 || height < 2)
		return 0;
	Media::CS::TransferFunc *rfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetRTranParam());
	Media::CS::TransferFunc *gfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetGTranParam());
	Media::CS::TransferFunc *bfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetBTranParam());
	NEW_CLASS(outImage, Media::StaticImage(width, height, 0, 64, Media::PF_LE_B16G16R16A16, 0, colorProfile, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	imgPtr = (UInt16*)outImage->data;
	i = width >> 1;
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
	i = width - j;
	k = width - j - 1;
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

	i = width >> 1;
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
	i = width - j;
	k = width - j - 1;
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
	j = height >> 1;
	while (i < j)
	{
		MemCopyNO(imgPtr, outImage->data, bpl);
		imgPtr += bpl >> 1;
		i++;
	}
	if (height & 1)
	{
		MemCopyNO(imgPtr, outImage->data, bpl >> 1);
	}
	DEL_CLASS(bfunc);
	DEL_CLASS(gfunc);
	DEL_CLASS(rfunc);
	return outImage;
}
