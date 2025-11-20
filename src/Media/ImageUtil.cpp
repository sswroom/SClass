#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Media/ImageCopy_C.h"
#include "Media/ImageUtil.h"
#include "Media/ImageUtil_C.h"

void Media::ImageUtil::ColorReplaceAlpha32(UnsafeArray<UInt8> pixelPtr, UOSInt w, UOSInt h, UInt32 col)
{
	UInt8 lut[256];
	UInt8 b;
	UOSInt i;
	i = 256;
	b = (UInt8)(col >> 24);
	while (i-- > 0)
	{
		lut[i] = (UInt8)(((i << 1) + 1) * b / 510);
	}

	i = w * h;
	while (i-- > 0)
	{
		b = pixelPtr[0];
		if (b)
		{
			WriteUInt32(&pixelPtr[0], col);
			pixelPtr[3] = lut[b];
		}
		pixelPtr += 4;
	}
}

void Media::ImageUtil::DrawHLineNA32(UnsafeArray<UInt8> pixelPtr, UOSInt w, UOSInt h, UOSInt bpl, OSInt y, OSInt x1, OSInt x2, UInt32 col)
{
	if (y < 0 || (UOSInt)y >= h || x1 >= (OSInt)w || x2 < 0)
	{
		return;
	}
	if (x1 < 0)
	{
		x1 = 0;
	}
	if (x2 >= (OSInt)w)
	{
		x2 = (OSInt)w;
	}
	if (x2 <= x1)
	{
		return;
	}
	UOSInt cnt = (UOSInt)(x2 - x1);
	pixelPtr = pixelPtr + (UOSInt)y * bpl + (UOSInt)x1 * 4;
	while (cnt-- > 0)
	{
		WriteUInt32(&pixelPtr[0], col);
		pixelPtr += 4;
	}
}

void Media::ImageUtil::DrawVLineNA32(UnsafeArray<UInt8> pixelPtr, UOSInt w, UOSInt h, UOSInt bpl, OSInt x, OSInt y1, OSInt y2, UInt32 col)
{
	if (x < 0 || (UOSInt)x >= w || y1 >= (OSInt)h || y2 < 0)
	{
		return;
	}
	if (y1 < 0)
	{
		y1 = 0;
	}
	if (y2 >= (OSInt)h)
	{
		y2 = (OSInt)h;
	}
	if (y2 <= y1)
	{
		return;
	}
	UOSInt cnt = (UOSInt)(y2 - y1);
	pixelPtr = pixelPtr + (UOSInt)y1 * bpl + (UOSInt)x * 4;
	while (cnt-- > 0)
	{
		WriteUInt32(&pixelPtr[0], col);
		pixelPtr += bpl;
	}
}

void Media::ImageUtil::ImageCopyR(UnsafeArray<UInt8> destPtr, OSInt destBpl, UnsafeArray<const UInt8> srcPtr, OSInt srcBpl, OSInt srcX, OSInt srcY, UOSInt srcw, UOSInt srch, UOSInt bpp, Bool upSideDown, Media::RotateType srcRotate, Media::RotateType destRotate)
{
	RotateType rt = Media::RotateTypeCalc(srcRotate, destRotate);
	switch (srcRotate)
	{
	case RotateType::CW_90:
	case RotateType::HFLIP_CW_90:
		srcPtr = srcPtr + srcX * srcBpl + (srcY * (OSInt)bpp >> 3);
		break;
	case RotateType::HFLIP_CW_180:
	case RotateType::CW_180:
		srcPtr = srcPtr + srcY * srcBpl + (srcX * (OSInt)bpp >> 3);
		break;
	case RotateType::HFLIP_CW_270:
	case RotateType::CW_270:
		srcPtr = srcPtr + srcX * srcBpl + (srcY * (OSInt)bpp >> 3);
		break;
	default:
	case RotateType::HFLIP:
	case RotateType::None:
		srcPtr = srcPtr + srcY * srcBpl + (srcX * (OSInt)bpp >> 3);
		break;
	}
	if (rt == RotateType::None)
	{
		ImageCopy_ImgCopyR(srcPtr.Ptr(), destPtr.Ptr(), srcw * (bpp >> 3), srch, (UOSInt)srcBpl, (UOSInt)destBpl, upSideDown);
	}
	else if (rt == RotateType::HFLIP)
	{
		if (bpp == 32)
			ImageUtil_HFlip32(srcPtr.Ptr(), destPtr.Ptr(), srcw, srch, (UOSInt)srcBpl, (UOSInt)destBpl, upSideDown);
		else if (bpp == 64)
			ImageUtil_HFlip64(srcPtr.Ptr(), destPtr.Ptr(), srcw, srch, (UOSInt)srcBpl, (UOSInt)destBpl, upSideDown);
	}
	else
	{
		if (upSideDown)
		{
			if (rt == RotateType::CW_90 || rt == RotateType::CW_270 || rt == RotateType::HFLIP_CW_90 || rt == RotateType::HFLIP_CW_270)
			{
				destPtr += (srcw - 1) * (UOSInt)destBpl;
			}
			else
			{
				destPtr += (srch - 1) * (UOSInt)destBpl;
			}
			destBpl = -destBpl;
			upSideDown = false;
		}
		if (bpp == 32)
		{
			switch (rt)
			{
			case RotateType::CW_90:
				ImageUtil_Rotate32_CW90(srcPtr.Ptr(), destPtr.Ptr(), srcw, srch, (UOSInt)srcBpl, (UOSInt)destBpl);
				break;
			case RotateType::CW_180:
				ImageUtil_Rotate32_CW180(srcPtr.Ptr(), destPtr.Ptr(), srcw, srch, (UOSInt)srcBpl, (UOSInt)destBpl);
				break;
			case RotateType::CW_270:
				ImageUtil_Rotate32_CW270(srcPtr.Ptr(), destPtr.Ptr(), srcw, srch, (UOSInt)srcBpl, (UOSInt)destBpl);
				break;
			case RotateType::HFLIP_CW_90:
				ImageUtil_HFRotate32_CW90(srcPtr.Ptr(), destPtr.Ptr(), srcw, srch, (UOSInt)srcBpl, (UOSInt)destBpl);
				break;
			case RotateType::HFLIP_CW_180:
				ImageUtil_HFRotate32_CW180(srcPtr.Ptr(), destPtr.Ptr(), srcw, srch, (UOSInt)srcBpl, (UOSInt)destBpl);
				break;
			case RotateType::HFLIP_CW_270:
				ImageUtil_HFRotate32_CW270(srcPtr.Ptr(), destPtr.Ptr(), srcw, srch, (UOSInt)srcBpl, (UOSInt)destBpl);
				break;
			case RotateType::HFLIP:
			case RotateType::None:
			default:
				ImageCopy_ImgCopy(srcPtr.Ptr(), destPtr.Ptr(), srcw * (bpp >> 3), srch, srcBpl, destBpl);
				break;
			}
		}
		else if (bpp == 64)
		{
			switch (rt)
			{
			case RotateType::CW_90:
				ImageUtil_Rotate64_CW90(srcPtr.Ptr(), destPtr.Ptr(), srcw, srch, (UOSInt)srcBpl, (UOSInt)destBpl);
				break;
			case RotateType::CW_180:
				ImageUtil_Rotate64_CW180(srcPtr.Ptr(), destPtr.Ptr(), srcw, srch, (UOSInt)srcBpl, (UOSInt)destBpl);
				break;
			case RotateType::CW_270:
				ImageUtil_Rotate64_CW270(srcPtr.Ptr(), destPtr.Ptr(), srcw, srch, (UOSInt)srcBpl, (UOSInt)destBpl);
				break;
			case RotateType::HFLIP_CW_90:
				ImageUtil_HFRotate64_CW90(srcPtr.Ptr(), destPtr.Ptr(), srcw, srch, (UOSInt)srcBpl, (UOSInt)destBpl);
				break;
			case RotateType::HFLIP_CW_180:
				ImageUtil_HFRotate64_CW180(srcPtr.Ptr(), destPtr.Ptr(), srcw, srch, (UOSInt)srcBpl, (UOSInt)destBpl);
				break;
			case RotateType::HFLIP_CW_270:
				ImageUtil_HFRotate64_CW270(srcPtr.Ptr(), destPtr.Ptr(), srcw, srch, (UOSInt)srcBpl, (UOSInt)destBpl);
				break;
			case RotateType::HFLIP:
			case RotateType::None:
			default:
				ImageCopy_ImgCopy(srcPtr.Ptr(), destPtr.Ptr(), srcw * (bpp >> 3), srch, srcBpl, destBpl);
				break;
			}
		}
		else
		{
			ImageCopy_ImgCopyR(srcPtr.Ptr(), destPtr.Ptr(), srcw * (bpp >> 3), srch, (UOSInt)srcBpl, (UOSInt)destBpl, upSideDown);
		}
	}
}
