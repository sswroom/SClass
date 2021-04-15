#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/IImgResizer.h"
#include "Media/CS/TransferFunc.h"

Media::IImgResizer::IImgResizer(Media::AlphaType srcAlphaType)
{
	this->targetWidth = 0;
	this->targetHeight = 0;
	this->srcAlphaType = srcAlphaType;
	this->rar = Media::IImgResizer::RAR_KEEPAR;
}

void Media::IImgResizer::SetTargetWidth(UOSInt targetWidth)
{
	if ((OSInt)targetWidth < 0)
	{
		this->targetWidth = -(OSInt)targetWidth;
	}
	else
	{
		this->targetWidth = targetWidth;
	}
}

void Media::IImgResizer::SetTargetHeight(UOSInt targetHeight)
{
	if ((OSInt)targetHeight < 0)
	{
		this->targetHeight = -(OSInt)targetHeight;
	}
	else
	{
		this->targetHeight = targetHeight;
	}
}

void Media::IImgResizer::SetResizeAspectRatio(ResizeAspectRatio rar)
{
	this->rar = rar;
}

void Media::IImgResizer::SetSrcAlphaType(Media::AlphaType alphaType)
{
	this->srcAlphaType = alphaType;
}

void Media::IImgResizer::SetSrcRefLuminance(Double srcRefLuminance)
{
}

Media::StaticImage *Media::IImgResizer::ProcessToNew(Media::StaticImage *srcImage)
{
	this->SetSrcRefLuminance(Media::CS::TransferFunc::GetRefLuminance(srcImage->info->color->rtransfer));
	return ProcessToNewPartial(srcImage, 0, 0, Math::OSInt2Double(srcImage->info->dispWidth), Math::OSInt2Double(srcImage->info->dispHeight));
}

void Media::IImgResizer::CalOutputSize(Media::FrameInfo *srcInfo, OSInt targetWidth, OSInt targetHeight, Media::FrameInfo *destInfo, Media::IImgResizer::ResizeAspectRatio rar)
{
	destInfo->Set(srcInfo);
	if (rar == Media::IImgResizer::RAR_IGNOREAR)
	{
		destInfo->dispWidth = targetWidth;
		destInfo->dispHeight = targetHeight;
		destInfo->storeWidth = destInfo->dispWidth;
		destInfo->storeHeight = destInfo->dispHeight;
		destInfo->par2 = srcInfo->par2 * srcInfo->dispHeight / srcInfo->dispWidth * destInfo->dispWidth / destInfo->dispHeight;
		destInfo->hdpi = srcInfo->hdpi * targetWidth / srcInfo->dispWidth;
		destInfo->vdpi = srcInfo->vdpi * targetHeight / srcInfo->dispHeight;
	}
	else if (rar == Media::IImgResizer::RAR_KEEPAR)
	{
		if (targetWidth * srcInfo->dispHeight > targetHeight * srcInfo->dispWidth)
		{
			destInfo->dispHeight = targetHeight;
			destInfo->dispWidth = Math::Double2Int32(targetHeight * (Double)srcInfo->dispWidth / (Double)srcInfo->dispHeight);
		}
		else
		{
			destInfo->dispWidth = targetWidth;
			destInfo->dispHeight = Math::Double2Int32(targetWidth * (Double)srcInfo->dispHeight / (Double)srcInfo->dispWidth);
		}
		destInfo->storeWidth = destInfo->dispWidth;
		destInfo->storeHeight = destInfo->dispHeight;
		destInfo->par2 = srcInfo->par2 * srcInfo->dispHeight / srcInfo->dispWidth * destInfo->dispWidth / destInfo->dispHeight;
		destInfo->hdpi = srcInfo->hdpi * targetWidth / srcInfo->dispWidth;
		destInfo->vdpi = srcInfo->vdpi * targetHeight / srcInfo->dispHeight;
	}
	else
	{
		if (targetWidth * srcInfo->dispHeight * srcInfo->par2 > targetHeight * srcInfo->dispWidth)
		{
			destInfo->dispHeight = targetHeight;
			destInfo->dispWidth = Math::Double2Int32(targetHeight / srcInfo->par2 * Math::UOSInt2Double(srcInfo->dispWidth) / Math::UOSInt2Double(srcInfo->dispHeight));
		}
		else
		{
			destInfo->dispWidth = targetWidth;
			destInfo->dispHeight = Math::Double2Int32(targetWidth * Math::UOSInt2Double(srcInfo->dispHeight) * srcInfo->par2 / Math::UOSInt2Double(srcInfo->dispWidth));
		}
		destInfo->storeWidth = destInfo->dispWidth;
		destInfo->storeHeight = destInfo->dispHeight;
		destInfo->par2 = srcInfo->par2 * Math::UOSInt2Double(srcInfo->dispHeight) / Math::UOSInt2Double(srcInfo->dispWidth) * Math::UOSInt2Double(destInfo->dispWidth) / Math::UOSInt2Double(destInfo->dispHeight);
		destInfo->hdpi = srcInfo->hdpi * targetWidth / srcInfo->dispWidth;
		destInfo->vdpi = srcInfo->vdpi * targetHeight / srcInfo->dispHeight;
	}
}
