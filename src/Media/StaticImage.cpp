#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/IImgResizer.h"
#include "Media/ImageTo8Bit.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"

#include <stdio.h>

Media::StaticImage::StaticImage(Math::Size2D<UOSInt> dispSize, UInt32 fourcc, UInt32 bpp, Media::PixelFormat pf, UOSInt maxSize, NotNullPtr<const Media::ColorProfile> color, Media::ColorProfile::YUVType yuvType, Media::AlphaType atype, Media::YCOffset ycOfst) : Media::RasterImage(dispSize, Math::Size2D<UOSInt>(0, 0), fourcc, bpp, pf, maxSize, color, yuvType, atype, ycOfst)
{
	this->data = MemAllocA(UInt8, this->info.byteSize + 4);
}

Media::StaticImage::StaticImage(NotNullPtr<const Media::FrameInfo> imgInfo) : Media::RasterImage(imgInfo->dispSize, imgInfo->storeSize, imgInfo->fourcc, imgInfo->storeBPP, imgInfo->pf, imgInfo->byteSize, imgInfo->color, imgInfo->yuvType, imgInfo->atype, imgInfo->ycOfst)
{
	this->info.par2 = imgInfo->par2;
	this->info.hdpi = imgInfo->hdpi;
	this->info.vdpi = imgInfo->vdpi;
	this->info.ftype = imgInfo->ftype;
	this->info.rotateType = imgInfo->rotateType;
	this->data = MemAllocA(UInt8, this->info.byteSize + 4);
}

Media::StaticImage::~StaticImage()
{
	MemFreeA(data);
}

NotNullPtr<Media::RasterImage> Media::StaticImage::Clone() const
{
	NotNullPtr<Media::StaticImage> img;
	NEW_CLASSNN(img, Media::StaticImage(this->info));
	MemCopyANC(img->data, this->data, this->info.byteSize + 4);
	if (this->info.fourcc == 0)
	{
		if (this->pal)
		{
			UOSInt palSize = (UOSInt)(4 << this->info.storeBPP);
			MemCopyNO(img->pal, this->pal, palSize);
		}
	}
	if (this->hasHotSpot)
	{
		img->SetHotSpot(this->hotSpotX, this->hotSpotY);
	}
	return img;
}

Media::RasterImage::ImageType Media::StaticImage::GetImageType() const
{
	return Media::RasterImage::ImageType::Static;
}

void Media::StaticImage::GetRasterData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const
{
	UOSInt srcBpl = this->GetDataBpl();
	if (this->info.pf == Media::PF_PAL_1_A1 || this->info.pf == Media::PF_PAL_2_A1 || this->info.pf == Media::PF_PAL_4_A1 || this->info.pf == Media::PF_PAL_8_A1)
	{
		if (left < 0)
		{
//			destBuff += (-left * (this->info.storeBPP - 1) >> 3;
			width = (UOSInt)((OSInt)width + left);
			left = 0;
		}
		if (top < 0)
		{
			destBuff += (UOSInt)-top * destBpl;
			height = (UOSInt)((OSInt)height + top);
			top = 0;
		}
		if (left + (OSInt)width > (OSInt)this->info.dispSize.x)
		{
			width = this->info.dispSize.x - (UOSInt)left;
		}
		if (top + (OSInt)height > (OSInt)this->info.dispSize.y)
		{
			height = this->info.dispSize.y - (UOSInt)top;
		}
		if ((OSInt)width > 0 && (OSInt)height > 0)
		{
			UInt8 *srcBuff = this->data;
			UInt8 *srcBuff2 = srcBuff + ((this->info.storeSize.x * (this->info.storeBPP - 1) + 7) >> 3);
			UOSInt lineSize1 = (width * (this->info.storeBPP - 1) + 7) >> 3;
			UOSInt lineSize2 = (width + 7) >> 3;
			srcBuff = srcBuff + (((UOSInt)left * (this->info.storeBPP - 1)) >> 3) + (UOSInt)top * srcBpl;
			srcBuff2 = srcBuff2 + (UOSInt)(left >> 3) + (UOSInt)top * srcBpl;
			while (height-- > 0)
			{
				MemCopyNANC(destBuff, srcBuff, lineSize1);
				MemCopyNANC(destBuff + lineSize1, srcBuff2, lineSize2);
				srcBuff += srcBpl;
				srcBuff2 += srcBpl;
				destBuff += destBpl;
			}
		}
	}
	else
	{
		if (left < 0)
		{
			destBuff += ((UOSInt)(-left) * this->info.storeBPP) >> 3;
			width = (UOSInt)((OSInt)width + left);
			left = 0;
		}
		if (top < 0)
		{
			destBuff += (UOSInt)-top * destBpl;
			height = (UOSInt)((OSInt)height + top);
			top = 0;
		}
		if (left + (OSInt)width > (OSInt)this->info.dispSize.x)
		{
			width = this->info.dispSize.x - (UOSInt)left;
		}
		if (top + (OSInt)height > (OSInt)this->info.dispSize.y)
		{
			height = this->info.dispSize.y - (UOSInt)top;
		}
		if (width > 0 && height > 0)
		{
			UInt8 *srcBuff = this->data;
			Media::ImageUtil::ImageCopyR(destBuff, (OSInt)destBpl, srcBuff, (OSInt)srcBpl, left, top, width, height, this->info.storeBPP, upsideDown, this->info.rotateType, destRotate);
		}
	}
}

Bool Media::StaticImage::To32bpp()
{
	if (this->info.fourcc != 0)
		return false;

	UOSInt dispWidth = this->info.dispSize.x;
	UOSInt dispHeight = this->info.dispSize.y;
	UOSInt storeWidth = this->info.storeSize.x;
	UInt8 *pBits = (UInt8*)this->data;
	UInt8 *buff;
	UInt8 *pal = this->pal;
	UOSInt buffSize;
	switch (this->info.pf)
	{
	case Media::PF_PAL_1:
	case Media::PF_PAL_W1:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP1_ARGB32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth >> 3, (OSInt)dispWidth << 2, pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_PAL_2:
	case Media::PF_PAL_W2:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP2_ARGB32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth >> 2, (OSInt)dispWidth << 2, pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_PAL_4:
	case Media::PF_PAL_W4:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP4_ARGB32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth >> 1, (OSInt)dispWidth << 2, pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_PAL_8:
	case Media::PF_PAL_W8:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP8_ARGB32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth, (OSInt)dispWidth << 2, pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_R5G5B5:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvB5G5R5_ARGB32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 1, (OSInt)dispWidth << 2);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_R5G6B5:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvB5G6R5_ARGB32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 1, (OSInt)dispWidth << 2);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_B8G8R8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvB8G8R8_ARGB32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth * 3, (OSInt)dispWidth << 2);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_R8G8B8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvR8G8B8_ARGB32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth * 3, (OSInt)dispWidth << 2);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_R8G8B8A8:
		ImageUtil_SwapRGB(this->data, this->info.storeSize.CalcArea(), 32);
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_B8G8R8A8:
		return true;
	case Media::PF_B8G8R8A1:
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_B16G16R16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvARGB48_32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth * 6, (OSInt)dispWidth << 2);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_B16G16R16A16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvARGB64_32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 3, (OSInt)dispWidth << 2);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_W16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvW16_ARGB32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 1, (OSInt)dispWidth << 2);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_W16A16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvW16A16_ARGB32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 2, (OSInt)dispWidth << 2);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_W8A8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvW8A8_ARGB32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 1, (OSInt)dispWidth << 2);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_A2B10G10R10:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvA2B10G10R10_32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 2, (OSInt)dispWidth << 2);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_FB32G32R32A32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvFB32G32R32A32_32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 4, (OSInt)dispWidth << 2);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_FB32G32R32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvFB32G32R32_32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth * 12, (OSInt)dispWidth << 2);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_FW32A32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvFW32A32_32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 3, (OSInt)dispWidth << 2);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_FW32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvFW32_32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 2, (OSInt)dispWidth << 2);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_PAL_1_A1:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP1_A1_ARGB32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth, (OSInt)dispWidth << 2, pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_PAL_2_A1:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP2_A1_ARGB32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth, (OSInt)dispWidth << 2, pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_PAL_4_A1:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP4_A1_ARGB32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth, (OSInt)dispWidth << 2, pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_PAL_8_A1:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP8_A1_ARGB32(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth, (OSInt)dispWidth << 2, pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_UNKNOWN:
	default:
		return false;
	}
}

Bool Media::StaticImage::To64bpp()
{
	if (this->info.fourcc != 0)
		return false;

	UOSInt dispWidth = this->info.dispSize.x;
	UOSInt dispHeight = this->info.dispSize.y;
	UOSInt storeWidth = this->info.storeSize.x;
	UInt8 *pBits = (UInt8*)this->data;
	UInt8 *buff;
	UOSInt buffSize;
	switch (this->info.pf)
	{
	case Media::PF_PAL_1:
	case Media::PF_PAL_W1:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP1_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth >> 3, (OSInt)dispWidth << 3, this->pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_PAL_2:
	case Media::PF_PAL_W2:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP2_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth >> 2, (OSInt)dispWidth << 3, this->pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_PAL_4:
	case Media::PF_PAL_W4:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP4_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth >> 1, (OSInt)dispWidth << 3, this->pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_PAL_8:
	case Media::PF_PAL_W8:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP8_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth, (OSInt)dispWidth << 3, this->pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_R5G5B5:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvB5G5R5_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 1, (OSInt)dispWidth << 3);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_R5G6B5:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvB5G6R5_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 1, (OSInt)dispWidth << 3);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_B8G8R8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvB8G8R8_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth * 3, (OSInt)dispWidth << 3);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_R8G8B8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvR8G8B8_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth * 3, (OSInt)dispWidth << 3);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_B8G8R8A1:
	case Media::PF_B8G8R8A8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvARGB32_64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 2, (OSInt)dispWidth << 3);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_R8G8B8A8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvR8G8B8A8_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 2, (OSInt)dispWidth << 3);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_B16G16R16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvARGB48_64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth * 6, (OSInt)dispWidth << 3);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_B16G16R16A16:
		return true;
	case Media::PF_LE_W16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvW16_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 1, (OSInt)dispWidth << 3);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_W16A16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvW16A16_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 2, (OSInt)dispWidth << 3);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_W8A8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvW8A8_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 1, (OSInt)dispWidth << 3);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_A2B10G10R10:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvA2B10G10R10_64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 2, (OSInt)dispWidth << 3);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_FB32G32R32A32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvFB32G32R32A32_64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 4, (OSInt)dispWidth << 3);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_FB32G32R32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvFB32G32R32_64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth * 12, (OSInt)dispWidth << 3);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_FW32A32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvFW32A32_64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 3, (OSInt)dispWidth << 3);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_FW32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvFW32_64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth << 2, (OSInt)dispWidth << 3);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_PAL_1_A1:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP1_A1_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth, (OSInt)dispWidth << 3, this->pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_PAL_2_A1:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP2_A1_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth, (OSInt)dispWidth << 3, this->pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_PAL_4_A1:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP4_A1_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth, (OSInt)dispWidth << 3, this->pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_PAL_8_A1:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP8_A1_ARGB64(pBits, buff, dispWidth, dispHeight, (OSInt)storeWidth, (OSInt)dispWidth << 3, this->pal);
		MemFreeA(this->data);
		MemFree(this->pal);
		this->pal = 0;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_UNKNOWN:
	default:
		return false;
	}
}

Bool Media::StaticImage::ToW8()
{
	Double kr;
	Double kg;
	Double kb;
	if (this->info.fourcc == 0)
	{
		UOSInt i;
		UOSInt j;
		UInt32 c;
		UInt8 *buff;
		UInt8 *dptr;
		UInt8 *sptr;
		UOSInt lineAdd;
		switch (this->info.pf)
		{
		case PF_PAL_W8:
			this->info.pf = Media::PF_PAL_W8;
			return true;
		case PF_B8G8R8A8:
			buff = MemAllocA(UInt8, this->info.dispSize.CalcArea());
			if (this->pal == 0)
			{
				this->pal = MemAlloc(UInt8, 1024);
			}
			i = 0;
			j = 256;
			while (i < j)
			{
				c = (UInt32)(i | (i << 8) | (i << 16) | 0xff000000);
				WriteUInt32(&this->pal[i * 4], c);
				i++;
			}
			Media::ColorProfile::GetYUVConstants(this->info.yuvType, &kr, &kb);
			kg = 1 - kr - kb;
			dptr = buff;
			sptr = this->data;
			lineAdd = (this->info.storeSize.x - this->info.dispSize.x) * 4;
			i = this->info.dispSize.y;
			while (i-- > 0)
			{
				j = this->info.dispSize.x;
				while (j-- > 0)
				{
					*dptr = Math::SDouble2UInt8(sptr[0] * kb + sptr[1] * kg + sptr[2] * kr);
					sptr += 4;
					dptr += 1;
				}
				sptr += lineAdd;
			}
			MemFreeA(this->data);
			this->data = buff;
			this->info.storeSize = this->info.dispSize;
			this->info.byteSize = this->info.dispSize.CalcArea();
			this->info.storeBPP = 8;
			this->info.pf = Media::PF_PAL_W8;
			return true;
		case PF_B8G8R8:
			buff = MemAllocA(UInt8, this->info.dispSize.CalcArea());
			if (this->pal == 0)
			{
				this->pal = MemAlloc(UInt8, 1024);
			}
			i = 0;
			j = 256;
			while (i < j)
			{
				c = (UInt32)(i | (i << 8) | (i << 16) | 0xff000000);
				WriteUInt32(&this->pal[i * 4], c);
				i++;
			}
			Media::ColorProfile::GetYUVConstants(this->info.yuvType, &kr, &kb);
			kg = 1 - kr - kb;
			dptr = buff;
			sptr = this->data;
			lineAdd = (this->info.storeSize.x - this->info.dispSize.x) * 3;
			i = this->info.dispSize.y;
			while (i-- > 0)
			{
				j = this->info.dispSize.x;
				while (j-- > 0)
				{
					*dptr = Math::SDouble2UInt8(sptr[0] * kb + sptr[1] * kg + sptr[2] * kr);
					sptr += 3;
					dptr += 1;
				}
				sptr += lineAdd;
			}
			MemFreeA(this->data);
			this->data = buff;
			this->info.storeSize = this->info.dispSize;
			this->info.byteSize = this->info.dispSize.CalcArea();
			this->info.storeBPP = 8;
			this->info.pf = Media::PF_PAL_W8;
			return true;
		case PF_R8G8B8:
			buff = MemAllocA(UInt8, this->info.dispSize.CalcArea());
			if (this->pal == 0)
			{
				this->pal = MemAlloc(UInt8, 1024);
			}
			i = 0;
			j = 256;
			while (i < j)
			{
				c = (UInt32)(i | (i << 8) | (i << 16) | 0xff000000);
				WriteUInt32(&this->pal[i * 4], c);
				i++;
			}
			#define SET_DATA_BYTE 	( 	  	pdata,n,val) 		   (*(l_uint8 *)((l_uintptr_t)((l_uint8 *)(pdata) + (n)) ^ 3) = (val))
			Media::ColorProfile::GetYUVConstants(this->info.yuvType, &kr, &kb);
			kg = 1 - kr - kb;
			dptr = buff;
			sptr = this->data;
			lineAdd = (this->info.storeSize.x - this->info.dispSize.x) * 3;
			i = this->info.dispSize.y;
			while (i-- > 0)
			{
				j = this->info.dispSize.x;
				while (j-- > 0)
				{
					*dptr = Math::SDouble2UInt8(sptr[0] * kr + sptr[1] * kg + sptr[2] * kb);
					sptr += 3;
					dptr += 1;
				}
				sptr += lineAdd;
			}
			MemFreeA(this->data);
			this->data = buff;
			this->info.storeSize = this->info.dispSize;
			this->info.byteSize = this->info.dispSize.CalcArea();
			this->info.storeBPP = 8;
			this->info.pf = Media::PF_PAL_W8;
			return true;
		case PF_LE_FB32G32R32A32:
		case PF_LE_FB32G32R32:
		case PF_LE_B16G16R16A16:
		case PF_LE_B16G16R16:
		case PF_PAL_8:
		case PF_LE_A2B10G10R10:
		case PF_LE_R5G5B5:
		case PF_LE_R5G6B5:
		case PF_LE_FW32:
		case PF_LE_FW32A32:
		case PF_LE_W16:
		case PF_LE_W16A16:
		case PF_PAL_1:
		case PF_PAL_1_A1:
		case PF_PAL_2:
		case PF_PAL_2_A1:
		case PF_PAL_4:
		case PF_PAL_4_A1:
		case PF_PAL_8_A1:
		case PF_PAL_W1:
		case PF_PAL_W2:
		case PF_PAL_W4:
		case PF_R8G8B8A8:
		case PF_B8G8R8A1:
		case PF_W8A8:
		case PF_UNKNOWN:
		default:
			printf("StaticImage.ToW8: not supported\r\n");
			return false;
		}
	}
	else
	{
		printf("StaticImage.ToW8: not supported\r\n");
	}
	return false;
}

Bool Media::StaticImage::ToPal8()
{
	if (this->info.fourcc != 0)
		return false;

	UInt8 *buff;
	switch (this->info.pf)
	{
	case Media::PF_LE_FB32G32R32A32:
	case Media::PF_LE_FB32G32R32:
	case Media::PF_LE_B16G16R16A16:
	case Media::PF_LE_B16G16R16:
	case Media::PF_B8G8R8:
		this->To32bpp();
	case Media::PF_B8G8R8A8:
		buff = MemAllocA(UInt8, this->info.dispSize.CalcArea());
		if (this->pal == 0)
		{
			this->pal = MemAlloc(UInt8, 1024);
		}
		Media::ImageTo8Bit::From32bpp(this->data, buff, this->pal, this->info.dispSize.x, this->info.dispSize.y, (OSInt)this->GetDataBpl(), (OSInt)this->info.dispSize.x);
		MemFreeA(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = this->info.dispSize.CalcArea();
		this->info.storeBPP = 8;
		this->info.pf = Media::PF_PAL_8;
		return true;
	case PF_PAL_8:
		return true;
	case PF_PAL_W8:
		this->info.pf = Media::PF_PAL_W8;
		return true;
	case PF_LE_A2B10G10R10:
	case PF_LE_R5G5B5:
	case PF_LE_R5G6B5:
	case PF_LE_FW32:
	case PF_LE_FW32A32:
	case PF_LE_W16:
	case PF_LE_W16A16:
	case PF_PAL_1:
	case PF_PAL_1_A1:
	case PF_PAL_2:
	case PF_PAL_2_A1:
	case PF_PAL_4:
	case PF_PAL_4_A1:
	case PF_PAL_8_A1:
	case PF_PAL_W1:
	case PF_PAL_W2:
	case PF_PAL_W4:
	case PF_R8G8B8:
	case PF_R8G8B8A8:
	case PF_B8G8R8A1:
	case PF_W8A8:
	case PF_UNKNOWN:
	default:
		return false;
	}
	return false;
}

Bool Media::StaticImage::FillColor(UInt32 color)
{
	if (this->info.fourcc == 0 && this->info.storeBPP == 32 && this->info.pf == Media::PF_B8G8R8A8)
	{
		ImageUtil_ColorFill32(this->data, this->info.storeSize.CalcArea(), color);
		return true;
	}
	return false;
}

Bool Media::StaticImage::MultiplyAlpha(Double alpha)
{
	if (this->info.fourcc == 0 && this->info.storeBPP == 32 && this->info.pf == Media::PF_B8G8R8A8)
	{
		if (this->info.atype == Media::AT_PREMUL_ALPHA)
		{
			return false;
		}
		else if (this->info.atype == Media::AT_NO_ALPHA)
		{
			Int32 i32a = Double2Int32(alpha * 255);
			UInt8 a;
			if (i32a < 0)
				a = 0;
			else if (i32a > 255)
				a = 255;
			else
				a = (UInt8)i32a;
			UInt8 *ptr = this->data;
			UOSInt cnt = this->info.storeSize.CalcArea();
			while (cnt-- > 0)
			{
				ptr[3] = a;
				ptr += 4;
			}
			this->info.atype = Media::AT_ALPHA;
			return true;
		}
		else
		{
			UInt8 atable[256];
			Int32 i32a;
			UOSInt i = 256;
			while (i-- > 0)
			{
				i32a = Double2Int32(alpha * UOSInt2Double(i));
				if (i32a < 0)
					atable[i] = 0;
				else if (i32a > 255)
					atable[i] = 255;
				else
					atable[i] = (UInt8)i32a;
			}
			UInt8 *ptr = this->data;
			i = this->info.storeSize.CalcArea();
			while (i-- > 0)
			{
				ptr[3] = atable[ptr[3]];
				ptr += 4;
			}
			return true;
		}
	}
	return false;
}

Bool Media::StaticImage::MultiplyColor(UInt32 color)
{
	if (this->info.fourcc == 0 && this->info.storeBPP == 32 && this->info.pf == Media::PF_B8G8R8A8)
	{
		ImageUtil_ImageColorMul32(this->data, this->info.storeSize.x, this->info.storeSize.y, this->info.storeSize.x << 2, color);
		return true;
	}
	return false;
}

Bool Media::StaticImage::Resize(Media::IImgResizer *resizer, Math::Size2D<UOSInt> newSize)
{
	if (this->info.fourcc != 0)
		return false;
	if (this->To32bpp())
	{
		UInt8 *outBuff = MemAllocA(UInt8, (newSize.CalcArea() << 2) + 4);
		resizer->Resize((UInt8*)this->data, (OSInt)this->info.storeSize.x << 2, UOSInt2Double(this->info.dispSize.x), UOSInt2Double(this->info.dispSize.y), 0, 0, outBuff, (OSInt)newSize.x << 2, newSize.x, newSize.y);
		MemFreeA(this->data);
		this->data = outBuff;
		this->info.dispSize = newSize;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = newSize.CalcArea() << 2;
		return true;
	}
	return false;
}

Bool Media::StaticImage::RotateImage(RotateType rtype)
{
	if (this->info.fourcc == 0)
	{
		UOSInt srcWidth = this->info.dispSize.x;
		UOSInt srcHeight = this->info.dispSize.y;
		UInt8 *outBuff;
		if (this->info.storeBPP <= 32 && this->info.pf != Media::PF_LE_W16)
		{
			if (this->To32bpp())
			{
				if (rtype == RotateType::CW90)
				{
					outBuff = MemAllocA(UInt8, (this->info.dispSize.CalcArea() << 2) + 4);
					ImageUtil_Rotate32_CW90(this->data, outBuff, this->info.dispSize.x, this->info.dispSize.y, this->info.storeSize.x << 2, this->info.dispSize.y << 2);
					MemFreeA(this->data);
					this->data = outBuff;
					this->info.dispSize = Math::Size2D<UOSInt>(srcHeight, srcWidth);
					this->info.storeSize = this->info.dispSize;
					return true;
				}
				else if (rtype == RotateType::CW180)
				{
					outBuff = MemAllocA(UInt8, (this->info.dispSize.CalcArea() << 2) + 4);
					ImageUtil_Rotate32_CW180(this->data, outBuff, this->info.dispSize.x, this->info.dispSize.y, this->info.storeSize.x << 2, this->info.dispSize.x << 2);
					MemFreeA(this->data);
					this->data = outBuff;
					this->info.storeSize = this->info.dispSize;
					return true;
				}
				else if (rtype == RotateType::CW270)
				{
					outBuff = MemAllocA(UInt8, (this->info.dispSize.CalcArea() << 2) + 4);
					ImageUtil_Rotate32_CW270(this->data, outBuff, this->info.dispSize.x, this->info.dispSize.y, this->info.storeSize.x << 2, this->info.dispSize.y << 2);
					MemFreeA(this->data);
					this->data = outBuff;
					this->info.dispSize = Math::Size2D<UOSInt>(srcHeight, srcWidth);
					this->info.storeSize = this->info.dispSize;
					return true;
				}
			}
		}
		else
		{
			if (this->To64bpp())
			{
				if (rtype == RotateType::CW90)
				{
					outBuff = MemAllocA(UInt8, (this->info.dispSize.CalcArea() << 3) + 4);
					ImageUtil_Rotate64_CW90(this->data, outBuff, this->info.dispSize.x, this->info.dispSize.y, this->info.storeSize.x << 3, this->info.dispSize.y << 3);
					MemFreeA(this->data);
					this->data = outBuff;
					this->info.dispSize = Math::Size2D<UOSInt>(srcHeight, srcWidth);
					this->info.storeSize = this->info.dispSize;
					return true;
				}
				else if (rtype == RotateType::CW180)
				{
					outBuff = MemAllocA(UInt8, (this->info.dispSize.CalcArea() << 3) + 4);
					ImageUtil_Rotate64_CW180(this->data, outBuff, this->info.dispSize.x, this->info.dispSize.y, this->info.storeSize.x << 3, this->info.dispSize.x << 3);
					MemFreeA(this->data);
					this->data = outBuff;
					this->info.storeSize = this->info.dispSize;
					return true;
				}
				else if (rtype == RotateType::CW270)
				{
					outBuff = MemAllocA(UInt8, (this->info.dispSize.CalcArea() << 3) + 4);
					ImageUtil_Rotate64_CW270(this->data, outBuff, this->info.dispSize.x, this->info.dispSize.y, this->info.storeSize.x << 3, this->info.dispSize.y << 3);
					MemFreeA(this->data);
					this->data = outBuff;
					this->info.dispSize = Math::Size2D<UOSInt>(srcHeight, srcWidth);
					this->info.storeSize = this->info.dispSize;
					return true;
				}
			}
		}
	}
	return false;
}

Double Media::StaticImage::CalcPSNR(Media::StaticImage *simg) const
{
//	printf("CalcPSNR\r\n");
	if (simg->info.dispSize != this->info.dispSize)
	{
//		printf("dispSize not equal\r\n");
		return 0;
	}
	if (simg->info.fourcc != 0 || this->info.fourcc != 0)
	{
//		printf("fourcc not valid\r\n");
		return 0;
	}
	if (simg->info.pf != this->info.pf)
	{
//		printf("PixelFormat not equal: %d != %d\r\n", this->info.pf, simg->info.pf);
		return 0;
	}
	if (this->info.pf == Media::PF_LE_B16G16R16A16)
	{
		UOSInt i;
		UOSInt j;
		Int64 sum = 0;
		Int32 v;
		UInt8 *sptr = this->data;
		UInt8 *dptr = simg->data;
		UOSInt sAdd = (this->info.storeSize.x - this->info.dispSize.x) * 8; 
		UOSInt dAdd = (simg->info.storeSize.x - simg->info.dispSize.x) * 8;
		i = this->info.dispSize.y;
		while (i-- > 0)
		{
			j = this->info.dispSize.x;
			while (j-- > 0)
			{
				v = (Int32)ReadUInt16(&sptr[0]) - (Int32)ReadUInt16(&dptr[0]);
				sum += v * v;
				v = (Int32)ReadUInt16(&sptr[2]) - (Int32)ReadUInt16(&dptr[2]);
				sum += v * v;
				v = (Int32)ReadUInt16(&sptr[4]) - (Int32)ReadUInt16(&dptr[4]);
				sum += v * v;
				sptr += 8;
				dptr += 8;
			}
			sptr += sAdd;
			dptr += dAdd;
		}
		if (sum == 0)
		{
//			printf("sum = 0\r\n");
			return 0;
		}
		return 20 * Math_Log10(65535) - 10 * Math_Log10((Double)sum / UOSInt2Double(this->info.dispSize.CalcArea() * 3));
	}
	else if (this->info.pf == Media::PF_B8G8R8A8)
	{
		UOSInt i;
		UOSInt j;
		Int64 sum = 0;
		Int32 v;
		UInt8 *sptr = this->data;
		UInt8 *dptr = simg->data;
		UOSInt sAdd = (this->info.storeSize.x - this->info.dispSize.x) * 4; 
		UOSInt dAdd = (simg->info.storeSize.x - simg->info.dispSize.x) * 4;
		i = this->info.dispSize.y;
		while (i-- > 0)
		{
			j = this->info.dispSize.x;
			while (j-- > 0)
			{
				v = sptr[0] - (Int32)dptr[0];
				sum += v * v;
				v = sptr[1] - (Int32)dptr[1];
				sum += v * v;
				v = sptr[2] - (Int32)dptr[2];
				sum += v * v;
				sptr += 4;
				dptr += 4;
			}
			sptr += sAdd;
			dptr += dAdd;
		}
		if (sum == 0)
			return 0;
		return 20 * Math_Log10(255) - 10 * Math_Log10((Double)sum / UOSInt2Double(this->info.dispSize.CalcArea() * 3));
	}
	else
	{
//		printf("PixelFormat not support\r\n");
		return 0;
	}
}

Double Media::StaticImage::CalcAvgContrast(UOSInt *bgPxCnt) const
{
	UOSInt i;
	UOSInt j;
	UInt8 *ptr = this->data;
	UOSInt dataBpl = this->GetDataBpl();
	Double sum;
	Double thisPx;
	UOSInt cnt;
	if (this->info.pf == Media::PF_B8G8R8A8)
	{
		sum = 0;
		cnt = 0;
		j = this->info.dispSize.x;
		while (j-- > 1)
		{
			thisPx  = Math_Sqr(ptr[(j * 4) + 0] - ptr[((j - 1) * 4) + 0]);
			thisPx += Math_Sqr(ptr[(j * 4) + 1] - ptr[((j - 1) * 4) + 1]);
			thisPx += Math_Sqr(ptr[(j * 4) + 2] - ptr[((j - 1) * 4) + 2]);
			sum += thisPx;
			if (thisPx < 30)
			{
				cnt++;
			}
		}
		ptr += dataBpl;
		i = this->info.dispSize.y - 1;
		while (i-- > 0)
		{
			j = this->info.dispSize.x;
			while (j-- > 1)
			{
				thisPx  = Math_Sqr(ptr[(j * 4) + 0] - ptr[((j - 1) * 4) + 0]);
				thisPx += Math_Sqr(ptr[(j * 4) + 1] - ptr[((j - 1) * 4) + 1]);
				thisPx += Math_Sqr(ptr[(j * 4) + 2] - ptr[((j - 1) * 4) + 2]);
				thisPx += Math_Sqr(ptr[(j * 4) + 0] - ptr[(j * 4) - dataBpl + 0]);
				thisPx += Math_Sqr(ptr[(j * 4) + 1] - ptr[(j * 4) - dataBpl + 1]);
				thisPx += Math_Sqr(ptr[(j * 4) + 2] - ptr[(j * 4) - dataBpl + 2]);
				sum += thisPx;
				if (thisPx < 30)
				{
					cnt++;
				}
			}
			ptr += dataBpl;
		}
		if (bgPxCnt)
		{
			*bgPxCnt = cnt;
		}
		return sum / UOSInt2Double(this->info.dispSize.x - 1) / UOSInt2Double(this->info.dispSize.y - 1) * 0.5;
	}
	return 0;
}

Double Media::StaticImage::CalcColorRate() const
{
	UOSInt i;
	UOSInt j;
	UInt8 *ptr = this->data;
	UOSInt dataBpl = this->GetDataBpl();
	UOSInt lineAdd;
	Double sum;
	if (this->info.pf == Media::PF_B8G8R8A8)
	{
		sum = 0;
		lineAdd = dataBpl - this->info.dispSize.x * 4;
		UOSInt thisPx;
		UOSInt maxPx = 0;
		UInt8 maxR = 0;
		UInt8 maxG = 0;
		UInt8 maxB = 0;

		i = this->info.dispSize.y;
		while (i-- > 0)
		{
			j = this->info.dispSize.x;
			while (j-- > 0)
			{
				thisPx = (UOSInt)ptr[0] + ptr[1] + ptr[2];
				if (thisPx > maxPx)
				{
					maxPx = thisPx;
					maxR = ptr[2];
					maxG = ptr[1];
					maxB = ptr[0];
				}
				ptr += 4;
			}
			ptr += lineAdd;
		}

		Double rMul = 255.0 / maxR;
		Double gMul = 255.0 / maxG;
		Double bMul = 255.0 / maxB;

		ptr = this->data;
		i = this->info.dispSize.y;
		while (i-- > 0)
		{
			j = this->info.dispSize.x;
			while (j-- > 0)
			{
				sum += Math_Sqr(ptr[0] * bMul - ptr[1] * gMul);
				sum += Math_Sqr(ptr[1] * gMul - ptr[2] * rMul);
				ptr += 4;
			}
			ptr += lineAdd;
		}
		return sum / UOSInt2Double(this->info.dispSize.x) / UOSInt2Double(this->info.dispSize.y);
	}
	return 0;
}

UInt8 *Media::StaticImage::CreateNearPixelMask(Math::Coord2D<UOSInt> pxCoord, Int32 maxRate)
{
	if (this->info.pf == Media::PF_B8G8R8A8)
	{
		UOSInt w = this->info.dispSize.x;
		UOSInt h = this->info.dispSize.y;
		UInt8 *selMask = MemAlloc(UInt8, w * h);
		UOSInt bpl = this->GetDataBpl();
		UInt8 *imgPtr = this->data;
		MemClear(selMask, w * h);
		selMask[pxCoord.y * w + pxCoord.x] = 0xff;
		CalcNearPixelMaskH32(selMask, pxCoord.x, pxCoord.y, imgPtr + pxCoord.y * bpl + pxCoord.x * 4, maxRate);
		CalcNearPixelMaskV32(selMask, pxCoord.x, pxCoord.y, imgPtr + pxCoord.y * bpl + pxCoord.x * 4, maxRate);
		return selMask;
	}
	else if (this->info.pf == Media::PF_B8G8R8 || this->info.pf == Media::PF_R8G8B8)
	{
		UOSInt w = this->info.dispSize.x;
		UOSInt h = this->info.dispSize.y;
		UInt8 *selMask = MemAlloc(UInt8, w * h);
		UOSInt bpl = this->GetDataBpl();
		UInt8 *imgPtr = this->data;
		MemClear(selMask, w * h);
		selMask[pxCoord.y * w + pxCoord.x] = 0xff;
		CalcNearPixelMaskH32(selMask, pxCoord.x, pxCoord.y, imgPtr + pxCoord.y * bpl + pxCoord.x * 3, maxRate);
		CalcNearPixelMaskV32(selMask, pxCoord.x, pxCoord.y, imgPtr + pxCoord.y * bpl + pxCoord.x * 3, maxRate);
		return selMask;
	}
	else
	{
		return 0;
	}
}

Math::RectArea<UOSInt> Media::StaticImage::CalcNearPixelRange(Math::Coord2D<UOSInt> pxCoord, Int32 maxRate)
{
	UInt8 *selMask = CreateNearPixelMask(pxCoord, maxRate);
	if (selMask)
	{
		Math::Coord2D<UOSInt> min = pxCoord;
		Math::Coord2D<UOSInt> max = pxCoord;
		UInt8 *currPtr = selMask;
		UOSInt w = this->info.dispSize.x;
		UOSInt h = this->info.dispSize.y;
		UOSInt i = 0;
		UOSInt j;
		while (i < h)
		{
			j = 0;
			while (j < w)
			{
				if (*currPtr)
				{
					if (j < min.x)
					{
						min.x = j;
					}
					if (j > max.x)
					{
						max.x = j;
					}
					if (i < min.y)
					{
						min.y = i;
					}
					if (i > max.y)
					{
						max.y = i;
					}
				}
				currPtr++;
				j++;
			}
			i++;
		}
		MemFree(selMask);
		return Math::RectArea<UOSInt>(min.x, min.y, max.x - min.x + 1, max.y - min.y + 1);
	}
	else
	{
		return Math::RectArea<UOSInt>(pxCoord.x, pxCoord.y, 1, 1);
	}
}

Data::ByteArray Media::StaticImage::GetDataArray() const
{
	return Data::ByteArray(this->data, this->GetDataBpl() * this->info.dispSize.y);
}

void Media::StaticImage::CalcNearPixelMaskH32(UInt8 *pixelMask, UOSInt x, UOSInt y, UInt8 *c, Int32 maxRate)
{
	UOSInt w = this->info.dispSize.x;
	UOSInt bpl = this->GetDataBpl();
	UInt8 *imgPtr = this->data;
	UOSInt pxSize = this->info.storeBPP >> 3;
	UInt8 *c2;
	UInt8 *lastC;
	UInt8 *lastC2;
	Int32 r;
	Int32 g;
	Int32 b;
	Int32 total;
	UOSInt i;
	lastC = c;
	lastC2 = c;
	i = x;
	while (i > 0)
	{
		if (pixelMask[y * w + i - 1] != 0)
		{
			break;
		}
		c2 = imgPtr + y * bpl + (i - 1) * pxSize;
		b = lastC[0] - c2[0];
		g = lastC[1] - c2[1];
		r = lastC[2] - c2[2];
		if (r < 0)
		{
			r = -r;
		}
		if (g < 0)
		{
			g = -g;
		}
		if (b < 0)
		{
			b = -b;
		}
		total = r + g + b;
		if (total >= maxRate)
		{
			break;
		}
		i--;
		pixelMask[y * w + i] = 0xff;
		lastC = lastC2;
		lastC2 = c2;
	}
	while (i < x)
	{
		CalcNearPixelMaskV32(pixelMask, i, y, imgPtr + y * bpl + i * pxSize, maxRate);
		i++;
	}
	lastC = c;
	lastC2 = c;
	i = x + 1;
	while (i < w)
	{
		if (pixelMask[y * w + i] != 0)
		{
			break;
		}
		c2 = imgPtr + y * bpl + i * pxSize;
		b = lastC[0] - c2[0];
		g = lastC[1] - c2[1];
		r = lastC[2] - c2[2];
		if (r < 0)
		{
			r = -r;
		}
		if (g < 0)
		{
			g = -g;
		}
		if (b < 0)
		{
			b = -b;
		}
		total = r + g + b;
		if (total >= maxRate)
		{
			break;
		}
		pixelMask[y * w + i] = 0xff;
		lastC = lastC2;
		lastC2 = c2;
		i++;
	}
	i--;
	while (i > x)
	{
		CalcNearPixelMaskV32(pixelMask, i, y, imgPtr + y * bpl + i * pxSize, maxRate);
		i--;
	}
}

void Media::StaticImage::CalcNearPixelMaskV32(UInt8 *pixelMask, UOSInt x, UOSInt y, UInt8 *c, Int32 maxRate)
{
	UOSInt w = this->info.dispSize.x;
	UOSInt h = this->info.dispSize.y;
	UOSInt bpl = this->GetDataBpl();
	UOSInt pxSize = this->info.storeBPP >> 3;
	UInt8 *imgPtr = this->data;
	UInt8 *lastC;
	UInt8 *lastC2;
	UInt8 *c2;
	Int32 r;
	Int32 g;
	Int32 b;
	Int32 total;
	UOSInt i;
	i = y;
	lastC = c;
	lastC2 = c;
	while (i > 0)
	{
		if (pixelMask[(i - 1) * w + x] != 0)
		{
			break;
		}
		c2 = imgPtr + (i - 1) * bpl + x * pxSize;
		b = lastC[0] - c2[0];
		g = lastC[1] - c2[1];
		r = lastC[2] - c2[2];
		if (r < 0)
		{
			r = -r;
		}
		if (g < 0)
		{
			g = -g;
		}
		if (b < 0)
		{
			b = -b;
		}
		total = r + g + b;
		if (total >= maxRate)
		{
			break;
		}
		i--;
		pixelMask[i * w + x] = 0xff;
		lastC = lastC2;
		lastC2 = c2;
	}
	while (i < y)
	{
		CalcNearPixelMaskH32(pixelMask, x, i, imgPtr + i * bpl + x * pxSize, maxRate);
		i++;
	}
	lastC = c;
	lastC2 = c;
	i = y + 1;
	while (i < h)
	{
		if (pixelMask[i * w + x] != 0)
		{
			break;
		}
		c2 = imgPtr + i * bpl + x * pxSize;
		b = lastC[0] - c2[0];
		g = lastC[1] - c2[1];
		r = lastC[2] - c2[2];
		if (r < 0)
		{
			r = -r;
		}
		if (g < 0)
		{
			g = -g;
		}
		if (b < 0)
		{
			b = -b;
		}
		total = r + g + b;
		if (total >= maxRate)
		{
			break;
		}
		pixelMask[i * w + x] = 0xff;
		lastC = lastC2;
		lastC2 = c2;
		i++;
	}
	i--;
	while (i > y)
	{
		CalcNearPixelMaskH32(pixelMask, x, i, imgPtr + i * bpl + x * pxSize, maxRate);
		i--;
	}
}
