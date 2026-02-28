#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Math/Math_C.h"
#include "Media/ImageResizer.h"
#include "Media/ImageTo8Bit.h"
#include "Media/ImageUtil.h"
#include "Media/ImageUtil_C.h"
#include "Media/StaticImage.h"

#include <stdio.h>

Media::StaticImage::StaticImage(Math::Size2D<UIntOS> dispSize, UInt32 fourcc, UInt32 bpp, Media::PixelFormat pf, UIntOS maxSize, NN<const Media::ColorProfile> color, Media::ColorProfile::YUVType yuvType, Media::AlphaType atype, Media::YCOffset ycOfst) : Media::RasterImage(dispSize, Math::Size2D<UIntOS>(0, 0), fourcc, bpp, pf, maxSize, color, yuvType, atype, ycOfst)
{
	this->data = MemAllocAArr(UInt8, this->info.byteSize + 4);
}

Media::StaticImage::StaticImage(NN<const Media::FrameInfo> imgInfo) : Media::RasterImage(imgInfo->dispSize, imgInfo->storeSize, imgInfo->fourcc, imgInfo->storeBPP, imgInfo->pf, imgInfo->byteSize, imgInfo->color, imgInfo->yuvType, imgInfo->atype, imgInfo->ycOfst)
{
	this->info.hdpi = imgInfo->hdpi;
	this->info.vdpi = imgInfo->vdpi;
	this->info.ftype = imgInfo->ftype;
	this->info.rotateType = imgInfo->rotateType;
	this->data = MemAllocAArr(UInt8, this->info.byteSize + 4);
}

Media::StaticImage::~StaticImage()
{
	MemFreeAArr(data);
}

NN<Media::RasterImage> Media::StaticImage::Clone() const
{
	NN<Media::StaticImage> img;
	NEW_CLASSNN(img, Media::StaticImage(this->info));
	MemCopyANC(img->data.Ptr(), this->data.Ptr(), this->info.byteSize + 4);
	if (this->info.fourcc == 0)
	{
		UnsafeArray<UInt8> imgPal;
		UnsafeArray<UInt8> pal;
		if (this->pal.SetTo(pal) && img->pal.SetTo(imgPal))
		{
			UIntOS palSize = (UIntOS)(4 << this->info.storeBPP);
			MemCopyNO(&imgPal[0], &pal[0], palSize);
		}
	}
	if (this->hasHotSpot)
	{
		img->SetHotSpot(this->hotSpotX, this->hotSpotY);
	}
	return img;
}

Media::RasterImage::ImageClass Media::StaticImage::GetImageClass() const
{
	return Media::RasterImage::ImageClass::StaticImage;
}

void Media::StaticImage::GetRasterData(UnsafeArray<UInt8> destBuff, IntOS left, IntOS top, UIntOS width, UIntOS height, UIntOS destBpl, Bool upsideDown, Media::RotateType destRotate) const
{
	UIntOS srcBpl = this->GetDataBpl();
	if (this->info.pf == Media::PF_PAL_1_A1 || this->info.pf == Media::PF_PAL_2_A1 || this->info.pf == Media::PF_PAL_4_A1 || this->info.pf == Media::PF_PAL_8_A1)
	{
		if (left < 0)
		{
//			destBuff += (-left * (this->info.storeBPP - 1) >> 3;
			width = (UIntOS)((IntOS)width + left);
			left = 0;
		}
		if (top < 0)
		{
			destBuff += (UIntOS)-top * destBpl;
			height = (UIntOS)((IntOS)height + top);
			top = 0;
		}
		if (left + (IntOS)width > (IntOS)this->info.dispSize.x)
		{
			width = this->info.dispSize.x - (UIntOS)left;
		}
		if (top + (IntOS)height > (IntOS)this->info.dispSize.y)
		{
			height = this->info.dispSize.y - (UIntOS)top;
		}
		if ((IntOS)width > 0 && (IntOS)height > 0)
		{
			UnsafeArray<UInt8> srcBuff = this->data;
			UnsafeArray<UInt8> srcBuff2 = srcBuff + ((this->info.storeSize.x * (this->info.storeBPP - 1) + 7) >> 3);
			UIntOS lineSize1 = (width * (this->info.storeBPP - 1) + 7) >> 3;
			UIntOS lineSize2 = (width + 7) >> 3;
			srcBuff = srcBuff + (((UIntOS)left * (this->info.storeBPP - 1)) >> 3) + (UIntOS)top * srcBpl;
			srcBuff2 = srcBuff2 + (UIntOS)(left >> 3) + (UIntOS)top * srcBpl;
			while (height-- > 0)
			{
				MemCopyNANC(destBuff.Ptr(), srcBuff.Ptr(), lineSize1);
				MemCopyNANC(destBuff.Ptr() + lineSize1, srcBuff2.Ptr(), lineSize2);
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
			destBuff += ((UIntOS)(-left) * this->info.storeBPP) >> 3;
			width = (UIntOS)((IntOS)width + left);
			left = 0;
		}
		if (top < 0)
		{
			destBuff += (UIntOS)-top * destBpl;
			height = (UIntOS)((IntOS)height + top);
			top = 0;
		}
		if (left + (IntOS)width > (IntOS)this->info.dispSize.x)
		{
			width = this->info.dispSize.x - (UIntOS)left;
		}
		if (top + (IntOS)height > (IntOS)this->info.dispSize.y)
		{
			height = this->info.dispSize.y - (UIntOS)top;
		}
		if (width > 0 && height > 0)
		{
			UnsafeArray<UInt8> srcBuff = this->data;
			Media::ImageUtil::ImageCopyR(destBuff, (IntOS)destBpl, srcBuff, (IntOS)srcBpl, left, top, width, height, this->info.storeBPP, upsideDown, this->info.rotateType, destRotate);
		}
	}
}

Bool Media::StaticImage::ToB8G8R8A8()
{
	if (this->info.fourcc != 0)
		return false;

	UIntOS dispWidth = this->info.dispSize.x;
	UIntOS dispHeight = this->info.dispSize.y;
	UIntOS storeWidth = this->info.storeSize.x;
	UnsafeArray<UInt8> pBits = this->data;
	UInt8 *buff;
	UnsafeArray<UInt8> pal;
	UIntOS buffSize;
	switch (this->info.pf)
	{
	case Media::PF_PAL_1:
	case Media::PF_PAL_W1:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP1_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth >> 3, (IntOS)dispWidth << 2, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_PAL_2:
	case Media::PF_PAL_W2:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP2_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth >> 2, (IntOS)dispWidth << 2, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_PAL_4:
	case Media::PF_PAL_W4:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP4_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth >> 1, (IntOS)dispWidth << 2, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_PAL_8:
	case Media::PF_PAL_W8:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP8_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth, (IntOS)dispWidth << 2, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_R5G5B5:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvB5G5R5_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 1, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_R5G6B5:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvB5G6R5_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 1, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_B8G8R8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvB8G8R8_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth * 3, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_R8G8B8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvR8G8B8_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth * 3, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_R8G8B8A8:
		ImageUtil_SwapRGB(this->data.Ptr(), this->info.storeSize.CalcArea(), 32);
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_B8G8R8A8:
		return true;
	case Media::PF_B8G8R8A1:
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_B16G16R16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvB16G16R16_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth * 6, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_R16G16B16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvR16G16B16_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth * 6, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_B16G16R16A16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvB16G16R16A16_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 3, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_R16G16B16A16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvR16G16B16A16_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 3, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_W16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvW16_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 1, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_W16A16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvW16A16_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 2, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_W8A8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvW8A8_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 1, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_A2B10G10R10:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvA2B10G10R10_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 2, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_FB32G32R32A32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvFB32G32R32A32_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 4, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_FR32G32B32A32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvFR32G32B32A32_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 4, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_FB32G32R32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvFB32G32R32_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth * 12, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_FR32G32B32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvFR32G32B32_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth * 12, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_FW32A32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvFW32A32_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 3, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_LE_FW32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 2));
		ImageUtil_ConvFW32_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 2, (IntOS)dispWidth << 2);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_PAL_1_A1:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP1_A1_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth, (IntOS)dispWidth << 2, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_PAL_2_A1:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP2_A1_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth, (IntOS)dispWidth << 2, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_PAL_4_A1:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP4_A1_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth, (IntOS)dispWidth << 2, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 2);
		this->info.storeBPP = 32;
		this->info.pf = Media::PF_B8G8R8A8;
		return true;
	case Media::PF_PAL_8_A1:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 2) + 4);
		ImageUtil_ConvP8_A1_B8G8R8A8(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth, (IntOS)dispWidth << 2, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
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

Bool Media::StaticImage::ToB16G16R16A16()
{
	if (this->info.fourcc != 0)
		return false;

	UIntOS dispWidth = this->info.dispSize.x;
	UIntOS dispHeight = this->info.dispSize.y;
	UIntOS storeWidth = this->info.storeSize.x;
	UnsafeArray<UInt8> pBits = this->data;
	UInt8 *buff;
	UIntOS buffSize;
	UnsafeArray<UInt8> pal;
	switch (this->info.pf)
	{
	case Media::PF_PAL_1:
	case Media::PF_PAL_W1:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP1_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth >> 3, (IntOS)dispWidth << 3, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_PAL_2:
	case Media::PF_PAL_W2:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP2_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth >> 2, (IntOS)dispWidth << 3, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_PAL_4:
	case Media::PF_PAL_W4:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP4_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth >> 1, (IntOS)dispWidth << 3, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_PAL_8:
	case Media::PF_PAL_W8:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP8_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth, (IntOS)dispWidth << 3, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_R5G5B5:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvB5G5R5_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 1, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_R5G6B5:
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvB5G6R5_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 1, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_B8G8R8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvB8G8R8_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth * 3, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_R8G8B8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvR8G8B8_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth * 3, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_B8G8R8A1:
	case Media::PF_B8G8R8A8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvB8G8R8A8_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 2, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_R8G8B8A8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvR8G8B8A8_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 2, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_B16G16R16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvB16G16R16_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth * 6, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_R16G16B16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvR16G16B16_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth * 6, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_B16G16R16A16:
		return true;
	case Media::PF_LE_R16G16B16A16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvR16G16B16A16_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth * 6, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_W16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvW16_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 1, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_W16A16:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvW16A16_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 2, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_W8A8:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvW8A8_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 1, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_A2B10G10R10:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvA2B10G10R10_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 2, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_FB32G32R32A32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvFB32G32R32A32_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 4, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_FR32G32B32A32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvFR32G32B32A32_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 4, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_FB32G32R32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvFB32G32R32_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth * 12, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_FR32G32B32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvFR32G32B32_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth * 12, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_FW32A32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvFW32A32_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 3, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_LE_FW32:
		buff = MemAllocA(UInt8, buffSize = (dispWidth * dispHeight << 3));
		ImageUtil_ConvFW32_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth << 2, (IntOS)dispWidth << 3);
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispHeight << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_PAL_1_A1:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP1_A1_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth, (IntOS)dispWidth << 3, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_PAL_2_A1:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP2_A1_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth, (IntOS)dispWidth << 3, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_PAL_4_A1:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP4_A1_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth, (IntOS)dispWidth << 3, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = (dispWidth * dispWidth << 3);
		this->info.storeBPP = 64;
		this->info.pf = Media::PF_LE_B16G16R16A16;
		return true;
	case Media::PF_PAL_8_A1:
		if (!this->pal.SetTo(pal))
			return false;
		buff = MemAllocA(UInt8, (dispWidth * dispHeight << 3) + 4);
		ImageUtil_ConvP8_A1_B16G16R16A16(pBits.Ptr(), buff, dispWidth, dispHeight, (IntOS)storeWidth, (IntOS)dispWidth << 3, pal.Ptr());
		MemFreeAArr(this->data);
		MemFreeArr(pal);
		this->pal = nullptr;
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
		UIntOS i;
		UIntOS j;
		UInt32 c;
		UInt8 *buff;
		UInt8 *dptr;
		UnsafeArray<UInt8> sptr;
		UIntOS lineAdd;
		UnsafeArray<UInt8> pal;
		switch (this->info.pf)
		{
		case PF_PAL_W8:
			this->info.pf = Media::PF_PAL_W8;
			return true;
		case PF_B8G8R8A8:
			buff = MemAllocA(UInt8, this->info.dispSize.CalcArea());
			if (!this->pal.SetTo(pal))
			{
				this->pal = pal = MemAllocArr(UInt8, 1024);
			}
			i = 0;
			j = 256;
			while (i < j)
			{
				c = (UInt32)(i | (i << 8) | (i << 16) | 0xff000000);
				WriteUInt32(&pal[i * 4], c);
				i++;
			}
			Media::ColorProfile::GetYUVConstants(this->info.yuvType, kr, kb);
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
			MemFreeAArr(this->data);
			this->data = buff;
			this->info.storeSize = this->info.dispSize;
			this->info.byteSize = this->info.dispSize.CalcArea();
			this->info.storeBPP = 8;
			this->info.pf = Media::PF_PAL_W8;
			return true;
		case PF_B8G8R8:
			buff = MemAllocA(UInt8, this->info.dispSize.CalcArea());
			if (!this->pal.SetTo(pal))
			{
				this->pal = pal = MemAllocArr(UInt8, 1024);
			}
			i = 0;
			j = 256;
			while (i < j)
			{
				c = (UInt32)(i | (i << 8) | (i << 16) | 0xff000000);
				WriteUInt32(&pal[i * 4], c);
				i++;
			}
			Media::ColorProfile::GetYUVConstants(this->info.yuvType, kr, kb);
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
			MemFreeAArr(this->data);
			this->data = buff;
			this->info.storeSize = this->info.dispSize;
			this->info.byteSize = this->info.dispSize.CalcArea();
			this->info.storeBPP = 8;
			this->info.pf = Media::PF_PAL_W8;
			return true;
		case PF_R8G8B8:
			buff = MemAllocA(UInt8, this->info.dispSize.CalcArea());
			if (!this->pal.SetTo(pal))
			{
				this->pal = pal = MemAllocArr(UInt8, 1024);
			}
			i = 0;
			j = 256;
			while (i < j)
			{
				c = (UInt32)(i | (i << 8) | (i << 16) | 0xff000000);
				WriteUInt32(&pal[i * 4], c);
				i++;
			}
			Media::ColorProfile::GetYUVConstants(this->info.yuvType, kr, kb);
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
			MemFreeAArr(this->data);
			this->data = buff;
			this->info.storeSize = this->info.dispSize;
			this->info.byteSize = this->info.dispSize.CalcArea();
			this->info.storeBPP = 8;
			this->info.pf = Media::PF_PAL_W8;
			return true;
		case PF_LE_FR32G32B32A32:
		case PF_LE_FB32G32R32A32:
		case PF_LE_FR32G32B32:
		case PF_LE_FB32G32R32:
		case PF_LE_R16G16B16A16:
		case PF_LE_B16G16R16A16:
		case PF_LE_R16G16B16:
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
	UnsafeArray<UInt8> pal;
	UnsafeArray<UInt8> oldPal;
	switch (this->info.pf)
	{
	case Media::PF_LE_FB32G32R32A32:
	case Media::PF_LE_FR32G32B32A32:
	case Media::PF_LE_FB32G32R32:
	case Media::PF_LE_FR32G32B32:
	case Media::PF_LE_B16G16R16A16:
	case Media::PF_LE_R16G16B16A16:
	case Media::PF_LE_B16G16R16:
	case Media::PF_LE_R16G16B16:
	case Media::PF_B8G8R8:
		this->ToB8G8R8A8();
	case Media::PF_B8G8R8A8:
		buff = MemAllocA(UInt8, this->info.dispSize.CalcArea());
		if (!this->pal.SetTo(pal))
		{
			this->pal = pal = MemAllocArr(UInt8, 1024);
		}
		Media::ImageTo8Bit::From32bpp(this->data, buff, pal, this->info.dispSize.x, this->info.dispSize.y, (IntOS)this->GetDataBpl(), (IntOS)this->info.dispSize.x);
		MemFreeAArr(this->data);
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
	case PF_PAL_1:
		if (!this->pal.SetTo(oldPal))
		{
			return false;
		}
		buff = MemAllocA(UInt8, this->info.dispSize.CalcArea());
		ImageUtil_ConvP1_P8(this->data.Ptr(), buff, this->info.dispSize.x, this->info.dispSize.y, (IntOS)this->GetDataBpl());
		MemFreeAArr(this->data);
		this->data = buff;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = this->info.dispSize.CalcArea();
		this->info.storeBPP = 8;
		this->info.pf = Media::PF_PAL_8;
		return true;
	case PF_LE_A2B10G10R10:
	case PF_LE_R5G5B5:
	case PF_LE_R5G6B5:
	case PF_LE_FW32:
	case PF_LE_FW32A32:
	case PF_LE_W16:
	case PF_LE_W16A16:
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

Bool Media::StaticImage::PalTo8bpp()
{
	if (this->info.fourcc != 0)
		return false;

	UInt8 *buff;
	UnsafeArray<UInt8> pal;
	UIntOS i;
	UIntOS j;
	UIntOS sadd;
	UInt8 b;
	UInt8 c;
	UnsafeArray<UInt8> oriPal;
	UnsafeArray<UInt8> srcData;
	UnsafeArray<UInt8> destData;
	if (this->info.pf == Media::PF_PAL_1 && this->pal.SetTo(oriPal))
	{
		srcData = this->data;
		buff = MemAllocA(UInt8, this->info.dispSize.CalcArea());
		pal = MemAllocArr(UInt8, 1024);
		destData = buff;
		WriteNInt32(&pal[0], ReadNInt32(&oriPal[0]));
		WriteNInt32(&pal[4], ReadNInt32(&oriPal[4]));
		i = 1024;
		while (i > 8)
		{
			i -= 4;
			WriteNInt32(&pal[i], 0);
		}
		sadd = (this->info.storeSize.x >> 3) - (this->info.dispSize.x >> 3);
		j = this->info.dispSize.y;
		if (this->info.dispSize.x & 7)
		{
			while (j-- > 0)
			{
				i = this->info.dispSize.x >> 3;
				while (i-- > 0)
				{
					b = *srcData++;
					destData[0] = (UInt8)(b >> 7);
					destData[1] = (b >> 6) & 1;
					destData[2] = (b >> 5) & 1;
					destData[3] = (b >> 4) & 1;
					destData[4] = (b >> 3) & 1;
					destData[5] = (b >> 2) & 1;
					destData[6] = (b >> 1) & 1;
					destData[7] = b & 1;
					destData += 8;
				}
				b = *srcData;
				i = this->info.dispSize.x & 7;
				while (i-- > 0)
				{
					destData[0] = (UInt8)(b >> 7);
					b = (UInt8)(b << 1);
					destData++;
				}
				srcData += sadd;
			}
		}
		else
		{
			while (j-- > 0)
			{
				i = this->info.dispSize.x >> 3;
				while (i-- > 0)
				{
					b = *srcData++;
					destData[0] = (UInt8)(b >> 7);
					destData[1] = (b >> 6) & 1;
					destData[2] = (b >> 5) & 1;
					destData[3] = (b >> 4) & 1;
					destData[4] = (b >> 3) & 1;
					destData[5] = (b >> 2) & 1;
					destData[6] = (b >> 1) & 1;
					destData[7] = b & 1;
					destData += 8;
				}
				srcData += sadd;
			}
		}
		MemFreeAArr(this->data);
		MemFreeArr(oriPal);
		this->data = buff;
		this->pal = pal;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = this->info.dispSize.CalcArea();
		this->info.storeBPP = 8;
		this->info.pf = Media::PF_PAL_8;
		return true;
	}
	else if (this->info.pf == Media::PF_PAL_W1 && this->pal.SetTo(oriPal))
	{
		UInt8 cMap[] = {0, 0xff};
		srcData = this->data;
		buff = MemAllocA(UInt8, this->info.dispSize.CalcArea());
		pal = MemAllocArr(UInt8, 1024);
		destData = buff;
		i = 0;
		while (i < 256)
		{
			pal[i * 4 + 0] = (UInt8)i;
			pal[i * 4 + 1] = (UInt8)i;
			pal[i * 4 + 2] = (UInt8)i;
			pal[i * 4 + 3] = (UInt8)0xff;
			i++;
		}
		sadd = (this->info.storeSize.x >> 3) - (this->info.dispSize.x >> 3);
		j = this->info.dispSize.y;
		if (this->info.dispSize.x & 7)
		{
			while (j-- > 0)
			{
				i = this->info.dispSize.x >> 3;
				while (i-- > 0)
				{
					b = *srcData++;
					destData[0] = cMap[(b >> 7)];
					destData[1] = cMap[(b >> 6) & 1];
					destData[2] = cMap[(b >> 5) & 1];
					destData[3] = cMap[(b >> 4) & 1];
					destData[4] = cMap[(b >> 3) & 1];
					destData[5] = cMap[(b >> 2) & 1];
					destData[6] = cMap[(b >> 1) & 1];
					destData[7] = cMap[b & 1];
					destData += 8;
				}
				b = *srcData;
				i = this->info.dispSize.x & 7;
				while (i-- > 0)
				{
					destData[0] = cMap[(b >> 7)];
					b = (UInt8)(b << 1);
					destData++;
				}
				srcData += sadd;
			}
		}
		else
		{
			while (j-- > 0)
			{
				i = this->info.dispSize.x >> 3;
				while (i-- > 0)
				{
					b = *srcData++;
					destData[0] = cMap[(b >> 7)];
					destData[1] = cMap[(b >> 6) & 1];
					destData[2] = cMap[(b >> 5) & 1];
					destData[3] = cMap[(b >> 4) & 1];
					destData[4] = cMap[(b >> 3) & 1];
					destData[5] = cMap[(b >> 2) & 1];
					destData[6] = cMap[(b >> 1) & 1];
					destData[7] = cMap[b & 1];
					destData += 8;
				}
				srcData += sadd;
			}
		}
		MemFreeAArr(this->data);
		MemFreeArr(oriPal);
		this->data = buff;
		this->pal = pal;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = this->info.dispSize.CalcArea();
		this->info.storeBPP = 8;
		this->info.pf = Media::PF_PAL_W8;
		return true;
	}
	else if (this->info.pf == Media::PF_PAL_2 && this->pal.SetTo(oriPal))
	{
		srcData = this->data;
		buff = MemAllocA(UInt8, this->info.dispSize.CalcArea());
		pal = MemAllocArr(UInt8, 1024);
		destData = buff;
		WriteNInt32(&pal[0], ReadNInt32(&oriPal[0]));
		WriteNInt32(&pal[4], ReadNInt32(&oriPal[4]));
		WriteNInt32(&pal[8], ReadNInt32(&oriPal[8]));
		WriteNInt32(&pal[12], ReadNInt32(&oriPal[12]));
		i = 1024;
		while (i > 16)
		{
			i -= 4;
			WriteNInt32(&pal[i], 0);
		}
		sadd = (this->info.storeSize.x >> 2) - (this->info.dispSize.x >> 2);
		j = this->info.dispSize.y;
		if (this->info.dispSize.x & 3)
		{
			while (j-- > 0)
			{
				i = this->info.dispSize.x >> 2;
				while (i-- > 0)
				{
					b = *srcData++;
					destData[0] = (UInt8)(b >> 6);
					destData[1] = (b >> 4) & 3;
					destData[2] = (b >> 2) & 3;
					destData[3] = b & 3;
					destData += 4;
				}
				b = *srcData;
				i = this->info.dispSize.x & 3;
				while (i-- > 0)
				{
					destData[0] = (UInt8)(b >> 6);
					b = (UInt8)(b << 2);
					destData++;
				}
				srcData += sadd;
			}
		}
		else
		{
			while (j-- > 0)
			{
				i = this->info.dispSize.x >> 2;
				while (i-- > 0)
				{
					b = *srcData++;
					destData[0] = (UInt8)(b >> 6);
					destData[1] = (b >> 4) & 3;
					destData[2] = (b >> 2) & 3;
					destData[3] = b & 3;
					destData += 4;
				}
				srcData += sadd;
			}
		}
		MemFreeAArr(this->data);
		MemFreeArr(oriPal);
		this->data = buff;
		this->pal = pal;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = this->info.dispSize.CalcArea();
		this->info.storeBPP = 8;
		this->info.pf = Media::PF_PAL_8;
		return true;
	}
	else if (this->info.pf == Media::PF_PAL_W2 && this->pal.SetTo(oriPal))
	{
		srcData = this->data;
		buff = MemAllocA(UInt8, this->info.dispSize.CalcArea());
		pal = MemAllocArr(UInt8, 1024);
		destData = buff;
		i = 0;
		while (i < 256)
		{
			pal[i * 4 + 0] = (UInt8)i;
			pal[i * 4 + 1] = (UInt8)i;
			pal[i * 4 + 2] = (UInt8)i;
			pal[i * 4 + 3] = (UInt8)0xff;
			i++;
		}
		sadd = (this->info.storeSize.x >> 2) - (this->info.dispSize.x >> 2);
		j = this->info.dispSize.y;
		if (this->info.dispSize.x & 3)
		{
			while (j-- > 0)
			{
				i = this->info.dispSize.x >> 2;
				while (i-- > 0)
				{
					b = *srcData++;
					c = (UInt8)(b >> 6);
					destData[0] = (UInt8)((c << 6) | (c << 4) | (c << 2) | c);
					c = (b >> 4) & 3;
					destData[1] = (UInt8)((c << 6) | (c << 4) | (c << 2) | c);
					c = (b >> 2) & 3;
					destData[2] = (UInt8)((c << 6) | (c << 4) | (c << 2) | c);
					c = b & 3;
					destData[3] = (UInt8)((c << 6) | (c << 4) | (c << 2) | c);
					destData += 4;
				}
				b = *srcData;
				i = this->info.dispSize.x & 3;
				while (i-- > 0)
				{
					c = (UInt8)(b >> 6);
					destData[0] = (UInt8)((c << 6) | (c << 4) | (c << 2) | c);
					b = (UInt8)(b << 2);
					destData++;
				}
				srcData += sadd;
			}
		}
		else
		{
			while (j-- > 0)
			{
				i = this->info.dispSize.x >> 2;
				while (i-- > 0)
				{
					b = *srcData++;
					c = (UInt8)(b >> 6);
					destData[0] = (UInt8)((c << 6) | (c << 4) | (c << 2) | c);
					c = (b >> 4) & 3;
					destData[1] = (UInt8)((c << 6) | (c << 4) | (c << 2) | c);
					c = (b >> 2) & 3;
					destData[2] = (UInt8)((c << 6) | (c << 4) | (c << 2) | c);
					c = b & 3;
					destData[3] = (UInt8)((c << 6) | (c << 4) | (c << 2) | c);
					destData += 4;
				}
				srcData += sadd;
			}
		}
		MemFreeAArr(this->data);
		MemFreeArr(oriPal);
		this->data = buff;
		this->pal = pal;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = this->info.dispSize.CalcArea();
		this->info.storeBPP = 8;
		this->info.pf = Media::PF_PAL_W8;
		return true;
	}
	else if (this->info.pf == Media::PF_PAL_4 && this->pal.SetTo(oriPal))
	{
		srcData = this->data;
		buff = MemAllocA(UInt8, this->info.dispSize.CalcArea());
		pal = MemAllocArr(UInt8, 1024);
		destData = buff;
		WriteNInt32(&pal[0], ReadNInt32(&oriPal[0]));
		WriteNInt32(&pal[4], ReadNInt32(&oriPal[4]));
		WriteNInt32(&pal[8], ReadNInt32(&oriPal[8]));
		WriteNInt32(&pal[12], ReadNInt32(&oriPal[12]));
		WriteNInt32(&pal[16], ReadNInt32(&oriPal[16]));
		WriteNInt32(&pal[20], ReadNInt32(&oriPal[20]));
		WriteNInt32(&pal[24], ReadNInt32(&oriPal[24]));
		WriteNInt32(&pal[28], ReadNInt32(&oriPal[28]));
		WriteNInt32(&pal[32], ReadNInt32(&oriPal[32]));
		WriteNInt32(&pal[36], ReadNInt32(&oriPal[36]));
		WriteNInt32(&pal[40], ReadNInt32(&oriPal[40]));
		WriteNInt32(&pal[44], ReadNInt32(&oriPal[44]));
		WriteNInt32(&pal[48], ReadNInt32(&oriPal[48]));
		WriteNInt32(&pal[52], ReadNInt32(&oriPal[52]));
		WriteNInt32(&pal[56], ReadNInt32(&oriPal[56]));
		WriteNInt32(&pal[60], ReadNInt32(&oriPal[60]));
		i = 1024;
		while (i > 64)
		{
			i -= 4;
			WriteNInt32(&pal[i], 0);
		}
		sadd = (this->info.storeSize.x >> 1) - (this->info.dispSize.x >> 1);
		j = this->info.dispSize.y;
		if (this->info.dispSize.x & 1)
		{
			while (j-- > 0)
			{
				i = this->info.dispSize.x >> 1;
				while (i-- > 0)
				{
					b = *srcData++;
					destData[0] = (UInt8)(b >> 4);
					destData[1] = b & 15;
					destData += 2;
				}
				destData[0] = (UInt8)((*srcData) >> 4);
				destData++;
				srcData += sadd;
			}
		}
		else
		{
			while (j-- > 0)
			{
				i = this->info.dispSize.x >> 1;
				while (i-- > 0)
				{
					b = *srcData++;
					destData[0] = (UInt8)(b >> 4);
					destData[1] = b & 15;
					destData += 2;
				}
				srcData += sadd;
			}
		}
		MemFreeAArr(this->data);
		MemFreeArr(oriPal);
		this->data = buff;
		this->pal = pal;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = this->info.dispSize.CalcArea();
		this->info.storeBPP = 8;
		this->info.pf = Media::PF_PAL_8;
		return true;
	}
	else if (this->info.pf == Media::PF_PAL_W4 && this->pal.SetTo(oriPal))
	{
		srcData = this->data;
		buff = MemAllocA(UInt8, this->info.dispSize.CalcArea());
		pal = MemAllocArr(UInt8, 1024);
		destData = buff;
		i = 0;
		while (i < 256)
		{
			pal[i * 4 + 0] = (UInt8)i;
			pal[i * 4 + 1] = (UInt8)i;
			pal[i * 4 + 2] = (UInt8)i;
			pal[i * 4 + 3] = (UInt8)0xff;
			i++;
		}
		sadd = (this->info.storeSize.x >> 1) - (this->info.dispSize.x >> 1);
		j = this->info.dispSize.y;
		if (this->info.dispSize.x & 1)
		{
			while (j-- > 0)
			{
				i = this->info.dispSize.x >> 1;
				while (i-- > 0)
				{
					b = *srcData++;
					c = (UInt8)(b >> 4);
					destData[0] = (UInt8)((c << 4) | c);
					c = b & 15;
					destData[1] = (UInt8)((c << 4) | c);
					destData += 2;
				}
				c = (UInt8)((*srcData) >> 4);
				destData[0] = (UInt8)((c << 4) | c);
				destData++;
				srcData += sadd;
			}
		}
		else
		{
			while (j-- > 0)
			{
				i = this->info.dispSize.x >> 1;
				while (i-- > 0)
				{
					b = *srcData++;
					c = (UInt8)(b >> 4);
					destData[0] = (UInt8)((c << 4) | c);
					c = b & 15;
					destData[1] = (UInt8)((c << 4) | c);
					destData += 2;
				}
				srcData += sadd;
			}
		}
		MemFreeAArr(this->data);
		MemFreeArr(oriPal);
		this->data = buff;
		this->pal = pal;
		this->info.storeSize = this->info.dispSize;
		this->info.byteSize = this->info.dispSize.CalcArea();
		this->info.storeBPP = 8;
		this->info.pf = Media::PF_PAL_W8;
		return true;
	}
	return false;
}

Bool Media::StaticImage::FillColor(UInt32 color)
{
	if (this->info.fourcc == 0 && this->info.storeBPP == 32 && this->info.pf == Media::PF_B8G8R8A8)
	{
		ImageUtil_ColorFill32(this->data.Ptr(), this->info.storeSize.CalcArea(), color);
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
			UInt8 atable[256];
			Int32 i32a;
			UIntOS i = 256;
			while (i-- > 0)
			{
				i32a = Double2Int32(alpha * UIntOS2Double(i));
				if (i32a < 0)
					atable[i] = 0;
				else if (i32a > 255)
					atable[i] = 255;
				else
					atable[i] = (UInt8)i32a;
			}
			UnsafeArray<UInt8> ptr = this->data;
			i = this->info.storeSize.CalcArea();
			while (i-- > 0)
			{
				ptr[0] = atable[ptr[0]];
				ptr[1] = atable[ptr[1]];
				ptr[2] = atable[ptr[2]];
				ptr[3] = atable[ptr[3]];
				ptr += 4;
			}
			return true;
		}
		else if (this->info.atype == Media::AT_ALPHA_ALL_FF || this->info.atype == Media::AT_IGNORE_ALPHA)
		{
			Int32 i32a = Double2Int32(alpha * 255);
			UInt8 a;
			if (i32a < 0)
				a = 0;
			else if (i32a > 255)
				a = 255;
			else
				a = (UInt8)i32a;
			UnsafeArray<UInt8> ptr = this->data;
			UIntOS cnt = this->info.storeSize.CalcArea();
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
			UIntOS i = 256;
			while (i-- > 0)
			{
				i32a = Double2Int32(alpha * UIntOS2Double(i));
				if (i32a < 0)
					atable[i] = 0;
				else if (i32a > 255)
					atable[i] = 255;
				else
					atable[i] = (UInt8)i32a;
			}
			UnsafeArray<UInt8> ptr = this->data;
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
		ImageUtil_ImageColorMul32(this->data.Ptr(), this->info.storeSize.x, this->info.storeSize.y, this->info.storeSize.x << 2, color);
		return true;
	}
	return false;
}

Bool Media::StaticImage::Resize(NN<Media::ImageResizer> resizer, Math::Size2D<UIntOS> newSize)
{
	if (this->info.fourcc != 0)
		return false;
	if (this->ToB8G8R8A8())
	{
		UInt8 *outBuff = MemAllocA(UInt8, (newSize.CalcArea() << 2) + 4);
		resizer->Resize(this->data.Ptr(), (IntOS)this->info.storeSize.x << 2, UIntOS2Double(this->info.dispSize.x), UIntOS2Double(this->info.dispSize.y), 0, 0, outBuff, (IntOS)newSize.x << 2, newSize.x, newSize.y);
		MemFreeAArr(this->data);
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
		UIntOS srcWidth = this->info.dispSize.x;
		UIntOS srcHeight = this->info.dispSize.y;
		UInt8 *outBuff;
		if (this->info.storeBPP <= 32 && this->info.pf != Media::PF_LE_W16)
		{
			if (this->ToB8G8R8A8())
			{
				if (rtype == RotateType::CW90)
				{
					outBuff = MemAllocA(UInt8, (this->info.dispSize.CalcArea() << 2) + 4);
					ImageUtil_Rotate32_CW90(this->data.Ptr(), outBuff, this->info.dispSize.x, this->info.dispSize.y, this->info.storeSize.x << 2, this->info.dispSize.y << 2);
					MemFreeAArr(this->data);
					this->data = outBuff;
					this->info.dispSize = Math::Size2D<UIntOS>(srcHeight, srcWidth);
					this->info.storeSize = this->info.dispSize;
					return true;
				}
				else if (rtype == RotateType::CW180)
				{
					outBuff = MemAllocA(UInt8, (this->info.dispSize.CalcArea() << 2) + 4);
					ImageUtil_Rotate32_CW180(this->data.Ptr(), outBuff, this->info.dispSize.x, this->info.dispSize.y, this->info.storeSize.x << 2, this->info.dispSize.x << 2);
					MemFreeAArr(this->data);
					this->data = outBuff;
					this->info.storeSize = this->info.dispSize;
					return true;
				}
				else if (rtype == RotateType::CW270)
				{
					outBuff = MemAllocA(UInt8, (this->info.dispSize.CalcArea() << 2) + 4);
					ImageUtil_Rotate32_CW270(this->data.Ptr(), outBuff, this->info.dispSize.x, this->info.dispSize.y, this->info.storeSize.x << 2, this->info.dispSize.y << 2);
					MemFreeAArr(this->data);
					this->data = outBuff;
					this->info.dispSize = Math::Size2D<UIntOS>(srcHeight, srcWidth);
					this->info.storeSize = this->info.dispSize;
					return true;
				}
			}
		}
		else
		{
			if (this->ToB16G16R16A16())
			{
				if (rtype == RotateType::CW90)
				{
					outBuff = MemAllocA(UInt8, (this->info.dispSize.CalcArea() << 3) + 4);
					ImageUtil_Rotate64_CW90(this->data.Ptr(), outBuff, this->info.dispSize.x, this->info.dispSize.y, this->info.storeSize.x << 3, this->info.dispSize.y << 3);
					MemFreeAArr(this->data);
					this->data = outBuff;
					this->info.dispSize = Math::Size2D<UIntOS>(srcHeight, srcWidth);
					this->info.storeSize = this->info.dispSize;
					return true;
				}
				else if (rtype == RotateType::CW180)
				{
					outBuff = MemAllocA(UInt8, (this->info.dispSize.CalcArea() << 3) + 4);
					ImageUtil_Rotate64_CW180(this->data.Ptr(), outBuff, this->info.dispSize.x, this->info.dispSize.y, this->info.storeSize.x << 3, this->info.dispSize.x << 3);
					MemFreeAArr(this->data);
					this->data = outBuff;
					this->info.storeSize = this->info.dispSize;
					return true;
				}
				else if (rtype == RotateType::CW270)
				{
					outBuff = MemAllocA(UInt8, (this->info.dispSize.CalcArea() << 3) + 4);
					ImageUtil_Rotate64_CW270(this->data.Ptr(), outBuff, this->info.dispSize.x, this->info.dispSize.y, this->info.storeSize.x << 3, this->info.dispSize.y << 3);
					MemFreeAArr(this->data);
					this->data = outBuff;
					this->info.dispSize = Math::Size2D<UIntOS>(srcHeight, srcWidth);
					this->info.storeSize = this->info.dispSize;
					return true;
				}
			}
		}
	}
	return false;
}

Double Media::StaticImage::CalcPSNR(NN<Media::StaticImage> simg) const
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
		UIntOS i;
		UIntOS j;
		Int64 sum = 0;
		Int32 v;
		UnsafeArray<UInt8> sptr = this->data;
		UnsafeArray<UInt8> dptr = simg->data;
		UIntOS sAdd = (this->info.storeSize.x - this->info.dispSize.x) * 8; 
		UIntOS dAdd = (simg->info.storeSize.x - simg->info.dispSize.x) * 8;
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
		return 20 * Math_Log10(65535) - 10 * Math_Log10((Double)sum / UIntOS2Double(this->info.dispSize.CalcArea() * 3));
	}
	else if (this->info.pf == Media::PF_B8G8R8A8)
	{
		UIntOS i;
		UIntOS j;
		Int64 sum = 0;
		Int32 v;
		UnsafeArray<UInt8> sptr = this->data;
		UnsafeArray<UInt8> dptr = simg->data;
		UIntOS sAdd = (this->info.storeSize.x - this->info.dispSize.x) * 4; 
		UIntOS dAdd = (simg->info.storeSize.x - simg->info.dispSize.x) * 4;
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
		return 20 * Math_Log10(255) - 10 * Math_Log10((Double)sum / UIntOS2Double(this->info.dispSize.CalcArea() * 3));
	}
	else
	{
//		printf("PixelFormat not support\r\n");
		return 0;
	}
}

Double Media::StaticImage::CalcAvgContrast(OptOut<UIntOS> bgPxCnt) const
{
	UIntOS i;
	UIntOS j;
	UnsafeArray<UInt8> ptr = this->data;
	UIntOS dataBpl = this->GetDataBpl();
	Double sum;
	Double thisPx;
	UIntOS cnt;
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
		bgPxCnt.Set(cnt);
		return sum / UIntOS2Double(this->info.dispSize.x - 1) / UIntOS2Double(this->info.dispSize.y - 1) * 0.5;
	}
	return 0;
}

Double Media::StaticImage::CalcColorRate() const
{
	UIntOS i;
	UIntOS j;
	UnsafeArray<UInt8> ptr = this->data;
	UIntOS dataBpl = this->GetDataBpl();
	UIntOS lineAdd;
	Double sum;
	if (this->info.pf == Media::PF_B8G8R8A8)
	{
		sum = 0;
		lineAdd = dataBpl - this->info.dispSize.x * 4;
		UIntOS thisPx;
		UIntOS maxPx = 0;
		UInt8 maxR = 0;
		UInt8 maxG = 0;
		UInt8 maxB = 0;

		i = this->info.dispSize.y;
		while (i-- > 0)
		{
			j = this->info.dispSize.x;
			while (j-- > 0)
			{
				thisPx = (UIntOS)ptr[0] + ptr[1] + ptr[2];
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
		return sum / UIntOS2Double(this->info.dispSize.x) / UIntOS2Double(this->info.dispSize.y);
	}
	return 0;
}

UnsafeArrayOpt<UInt8> Media::StaticImage::CreateNearPixelMask(Math::Coord2D<UIntOS> pxCoord, Int32 maxRate)
{
	if (this->info.pf == Media::PF_B8G8R8A8)
	{
		UIntOS w = this->info.dispSize.x;
		UIntOS h = this->info.dispSize.y;
		UInt8 *selMask = MemAlloc(UInt8, w * h);
		UIntOS bpl = this->GetDataBpl();
		UnsafeArray<UInt8> imgPtr = this->data;
		MemClear(selMask, w * h);
		selMask[pxCoord.y * w + pxCoord.x] = 0xff;
		CalcNearPixelMaskH32(selMask, pxCoord.x, pxCoord.y, imgPtr + pxCoord.y * bpl + pxCoord.x * 4, maxRate);
		CalcNearPixelMaskV32(selMask, pxCoord.x, pxCoord.y, imgPtr + pxCoord.y * bpl + pxCoord.x * 4, maxRate);
		return selMask;
	}
	else if (this->info.pf == Media::PF_B8G8R8 || this->info.pf == Media::PF_R8G8B8)
	{
		UIntOS w = this->info.dispSize.x;
		UIntOS h = this->info.dispSize.y;
		UInt8 *selMask = MemAlloc(UInt8, w * h);
		UIntOS bpl = this->GetDataBpl();
		UnsafeArray<UInt8> imgPtr = this->data;
		MemClear(selMask, w * h);
		selMask[pxCoord.y * w + pxCoord.x] = 0xff;
		CalcNearPixelMaskH32(selMask, pxCoord.x, pxCoord.y, imgPtr + pxCoord.y * bpl + pxCoord.x * 3, maxRate);
		CalcNearPixelMaskV32(selMask, pxCoord.x, pxCoord.y, imgPtr + pxCoord.y * bpl + pxCoord.x * 3, maxRate);
		return selMask;
	}
	else
	{
		return nullptr;
	}
}

Math::RectArea<UIntOS> Media::StaticImage::CalcNearPixelRange(Math::Coord2D<UIntOS> pxCoord, Int32 maxRate)
{
	UnsafeArray<UInt8> selMask;
	if (CreateNearPixelMask(pxCoord, maxRate).SetTo(selMask))
	{
		Math::Coord2D<UIntOS> min = pxCoord;
		Math::Coord2D<UIntOS> max = pxCoord;
		UnsafeArray<UInt8> currPtr = selMask;
		UIntOS w = this->info.dispSize.x;
		UIntOS h = this->info.dispSize.y;
		UIntOS i = 0;
		UIntOS j;
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
		MemFreeArr(selMask);
		return Math::RectArea<UIntOS>(min.x, min.y, max.x - min.x + 1, max.y - min.y + 1);
	}
	else
	{
		return Math::RectArea<UIntOS>(pxCoord.x, pxCoord.y, 1, 1);
	}
}

Data::ByteArray Media::StaticImage::GetDataArray() const
{
	return Data::ByteArray(this->data, this->GetDataBpl() * this->info.dispSize.y);
}

void Media::StaticImage::CalcNearPixelMaskH32(UnsafeArray<UInt8> pixelMask, UIntOS x, UIntOS y, UnsafeArray<UInt8> c, Int32 maxRate)
{
	UIntOS w = this->info.dispSize.x;
	UIntOS bpl = this->GetDataBpl();
	UnsafeArray<UInt8> imgPtr = this->data;
	UIntOS pxSize = this->info.storeBPP >> 3;
	UnsafeArray<UInt8> c2;
	UnsafeArray<UInt8> lastC;
	UnsafeArray<UInt8> lastC2;
	Int32 r;
	Int32 g;
	Int32 b;
	Int32 total;
	UIntOS i;
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

void Media::StaticImage::CalcNearPixelMaskV32(UnsafeArray<UInt8> pixelMask, UIntOS x, UIntOS y, UnsafeArray<UInt8> c, Int32 maxRate)
{
	UIntOS w = this->info.dispSize.x;
	UIntOS h = this->info.dispSize.y;
	UIntOS bpl = this->GetDataBpl();
	UIntOS pxSize = this->info.storeBPP >> 3;
	UnsafeArray<UInt8> imgPtr = this->data;
	UnsafeArray<UInt8> lastC;
	UnsafeArray<UInt8> lastC2;
	UnsafeArray<UInt8> c2;
	Int32 r;
	Int32 g;
	Int32 b;
	Int32 total;
	UIntOS i;
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
