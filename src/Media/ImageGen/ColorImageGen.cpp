#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/StaticImage.h"
#include "Media/CS/TransferFunc.h"
#include "Media/ImageGen/ColorImageGen.h"

Media::ImageGen::ColorImageGen::ColorImageGen()
{
}

Media::ImageGen::ColorImageGen::~ColorImageGen()
{
}

const UTF8Char *Media::ImageGen::ColorImageGen::GetName()
{
	return (const UTF8Char*)"Color Test";
}

Media::Image *Media::ImageGen::ColorImageGen::GenerateImage(Media::ColorProfile *colorProfile, UOSInt width, UOSInt height)
{
	Media::StaticImage *outImage;
	Int64 *imgPtr;
	UInt8 *imgPtr2;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UInt16 c[4];
	OSInt bpl = (OSInt)width << 3;
	if (width < 8 || height < 4)
		return 0;
	Media::CS::TransferFunc *rfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetRTranParam());
	Media::CS::TransferFunc *gfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetRTranParam());
	Media::CS::TransferFunc *bfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetRTranParam());
	NEW_CLASS(outImage, Media::StaticImage(width, height, 0, 64, Media::PF_LE_B16G16R16A16, 0, colorProfile, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));

	imgPtr2 = outImage->data;
	k = 0;

	imgPtr = (Int64*)imgPtr2;
	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(1) * 65535.0);
	c[3] = 65535;
	i = 0;
	j = (width * 1) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.125) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.875) * 65535.0);
	c[3] = 65535;
	j = (width * 2) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.25) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.75) * 65535.0);
	c[3] = 65535;
	j = (width * 3) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.375) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.625) * 65535.0);
	c[3] = 65535;
	j = (width * 4) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.5) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.5) * 65535.0);
	c[3] = 65535;
	j = (width * 5) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.625) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.375) * 65535.0);
	c[3] = 65535;
	j = (width * 6) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.75) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.25) * 65535.0);
	c[3] = 65535;
	j = (width * 7) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.875) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.125) * 65535.0);
	c[3] = 65535;
	j = width;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	l = (height * 1) >> 2;
	k++;
	imgPtr2 += bpl;
	while (k < l)
	{
		MemCopyNO(imgPtr2, imgPtr, (UOSInt)bpl);
		imgPtr2 += bpl;
		k++;
	}

	imgPtr = (Int64*)imgPtr2;
	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(1) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	i = 0;
	j = (width * 1) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.125) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.875) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	j = (width * 2) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.25) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.75) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	j = (width * 3) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.375) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.625) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	j = (width * 4) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.5) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.5) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	j = (width * 5) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.625) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.375) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	j = (width * 6) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.75) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.25) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	j = (width * 7) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.875) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.125) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	j = width;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	l = (height * 2) >> 2;
	k++;
	imgPtr2 += bpl;
	while (k < l)
	{
		MemCopyNO(imgPtr2, imgPtr, bpl);
		imgPtr2 += bpl;
		k++;
	}

	imgPtr = (Int64*)imgPtr2;
	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(1) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	i = 0;
	j = (width * 1) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.875) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.125) * 65535.0);
	c[3] = 65535;
	j = (width * 2) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.75) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.25) * 65535.0);
	c[3] = 65535;
	j = (width * 3) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.625) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.375) * 65535.0);
	c[3] = 65535;
	j = (width * 4) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.5) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.5) * 65535.0);
	c[3] = 65535;
	j = (width * 5) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.375) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.625) * 65535.0);
	c[3] = 65535;
	j = (width * 6) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.25) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.75) * 65535.0);
	c[3] = 65535;
	j = (width * 7) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.125) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.875) * 65535.0);
	c[3] = 65535;
	j = width;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	l = (height * 3) >> 2;
	k++;
	imgPtr2 += bpl;
	while (k < l)
	{
		MemCopyNO(imgPtr2, imgPtr, (UOSInt)bpl);
		imgPtr2 += bpl;
		k++;
	}

	imgPtr = (Int64*)imgPtr2;
	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	i = 0;
	j = (width * 1) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.125) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.125) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.125) * 65535.0);
	c[3] = 65535;
	j = (width * 2) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.25) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.25) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.25) * 65535.0);
	c[3] = 65535;
	j = (width * 3) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.375) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.375) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.375) * 65535.0);
	c[3] = 65535;
	j = (width * 4) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.5) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.5) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.5) * 65535.0);
	c[3] = 65535;
	j = (width * 5) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.625) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.625) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.625) * 65535.0);
	c[3] = 65535;
	j = (width * 6) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.75) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.75) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.75) * 65535.0);
	c[3] = 65535;
	j = (width * 7) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Math::Double2Int32(bfunc->ForwardTransfer(0.875) * 65535.0);
	c[1] = (UInt16)Math::Double2Int32(gfunc->ForwardTransfer(0.875) * 65535.0);
	c[2] = (UInt16)Math::Double2Int32(rfunc->ForwardTransfer(0.875) * 65535.0);
	c[3] = 65535;
	j = width;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	l = height;
	k++;
	imgPtr2 += bpl;
	while (k < l)
	{
		MemCopyNO(imgPtr2, imgPtr, (UOSInt)bpl);
		imgPtr2 += bpl;
		k++;
	}
	DEL_CLASS(bfunc);
	DEL_CLASS(gfunc);
	DEL_CLASS(rfunc);

	return outImage;
}
