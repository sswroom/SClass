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

Text::CStringNN Media::ImageGen::ColorImageGen::GetName() const
{
	return CSTR("Color Test");
}

Optional<Media::RasterImage> Media::ImageGen::ColorImageGen::GenerateImage(NN<const Media::ColorProfile> colorProfile, Math::Size2D<UOSInt> size)
{
	Media::StaticImage *outImage;
	Int64 *imgPtr;
	UInt8 *imgPtr2;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UInt16 c[4];
	UOSInt bpl = size.x << 3;
	if (size.x < 8 || size.y < 4)
		return 0;
	NN<Media::CS::TransferFunc> rfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetRTranParamRead());
	NN<Media::CS::TransferFunc> gfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetGTranParamRead());
	NN<Media::CS::TransferFunc> bfunc = Media::CS::TransferFunc::CreateFunc(colorProfile->GetBTranParamRead());
	NEW_CLASS(outImage, Media::StaticImage(size, 0, 64, Media::PF_LE_B16G16R16A16, 0, colorProfile, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));

	imgPtr2 = outImage->data;
	k = 0;

	imgPtr = (Int64*)imgPtr2;
	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(1) * 65535.0);
	c[3] = 65535;
	i = 0;
	j = (size.x * 1) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.125) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.875) * 65535.0);
	c[3] = 65535;
	j = (size.x * 2) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.25) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.75) * 65535.0);
	c[3] = 65535;
	j = (size.x * 3) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.375) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.625) * 65535.0);
	c[3] = 65535;
	j = (size.x * 4) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.5) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.5) * 65535.0);
	c[3] = 65535;
	j = (size.x * 5) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.625) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.375) * 65535.0);
	c[3] = 65535;
	j = (size.x * 6) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.75) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.25) * 65535.0);
	c[3] = 65535;
	j = (size.x * 7) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.875) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.125) * 65535.0);
	c[3] = 65535;
	j = size.x;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	l = (size.y * 1) >> 2;
	k++;
	imgPtr2 += bpl;
	while (k < l)
	{
		MemCopyNO(imgPtr2, imgPtr, bpl);
		imgPtr2 += bpl;
		k++;
	}

	imgPtr = (Int64*)imgPtr2;
	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(1) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	i = 0;
	j = (size.x * 1) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.125) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.875) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	j = (size.x * 2) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.25) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.75) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	j = (size.x * 3) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.375) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.625) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	j = (size.x * 4) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.5) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.5) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	j = (size.x * 5) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.625) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.375) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	j = (size.x * 6) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.75) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.25) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	j = (size.x * 7) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.875) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.125) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	j = size.x;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	l = (size.y * 2) >> 2;
	k++;
	imgPtr2 += bpl;
	while (k < l)
	{
		MemCopyNO(imgPtr2, imgPtr, bpl);
		imgPtr2 += bpl;
		k++;
	}

	imgPtr = (Int64*)imgPtr2;
	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(1) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	i = 0;
	j = (size.x * 1) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.875) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.125) * 65535.0);
	c[3] = 65535;
	j = (size.x * 2) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.75) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.25) * 65535.0);
	c[3] = 65535;
	j = (size.x * 3) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.625) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.375) * 65535.0);
	c[3] = 65535;
	j = (size.x * 4) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.5) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.5) * 65535.0);
	c[3] = 65535;
	j = (size.x * 5) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.375) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.625) * 65535.0);
	c[3] = 65535;
	j = (size.x * 6) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.25) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.75) * 65535.0);
	c[3] = 65535;
	j = (size.x * 7) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.125) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.875) * 65535.0);
	c[3] = 65535;
	j = size.x;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	l = (size.y * 3) >> 2;
	k++;
	imgPtr2 += bpl;
	while (k < l)
	{
		MemCopyNO(imgPtr2, imgPtr, (UOSInt)bpl);
		imgPtr2 += bpl;
		k++;
	}

	imgPtr = (Int64*)imgPtr2;
	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0) * 65535.0);
	c[3] = 65535;
	i = 0;
	j = (size.x * 1) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.125) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.125) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.125) * 65535.0);
	c[3] = 65535;
	j = (size.x * 2) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.25) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.25) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.25) * 65535.0);
	c[3] = 65535;
	j = (size.x * 3) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.375) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.375) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.375) * 65535.0);
	c[3] = 65535;
	j = (size.x * 4) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.5) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.5) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.5) * 65535.0);
	c[3] = 65535;
	j = (size.x * 5) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.625) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.625) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.625) * 65535.0);
	c[3] = 65535;
	j = (size.x * 6) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.75) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.75) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.75) * 65535.0);
	c[3] = 65535;
	j = (size.x * 7) >> 3;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	c[0] = (UInt16)Double2Int32(bfunc->ForwardTransfer(0.875) * 65535.0);
	c[1] = (UInt16)Double2Int32(gfunc->ForwardTransfer(0.875) * 65535.0);
	c[2] = (UInt16)Double2Int32(rfunc->ForwardTransfer(0.875) * 65535.0);
	c[3] = 65535;
	j = size.x;
	while (i < j)
	{
		imgPtr[i] = *(Int64*)c;
		i++;
	}

	l = size.y;
	k++;
	imgPtr2 += bpl;
	while (k < l)
	{
		MemCopyNO(imgPtr2, imgPtr, bpl);
		imgPtr2 += bpl;
		k++;
	}
	bfunc.Delete();
	gfunc.Delete();
	rfunc.Delete();
	return outImage;
}
