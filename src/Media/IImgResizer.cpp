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
		this->targetWidth = (UOSInt)-(OSInt)targetWidth;
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
		this->targetHeight = (UOSInt)-(OSInt)targetHeight;
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
	return ProcessToNewPartial(srcImage, 0, 0, Math::UOSInt2Double(srcImage->info->dispWidth), Math::UOSInt2Double(srcImage->info->dispHeight));
}

void Media::IImgResizer::CalOutputSize(Media::FrameInfo *srcInfo, UOSInt targetWidth, UOSInt targetHeight, Media::FrameInfo *destInfo, Media::IImgResizer::ResizeAspectRatio rar)
{
	destInfo->Set(srcInfo);
	if (rar == Media::IImgResizer::RAR_IGNOREAR)
	{
		destInfo->dispWidth = targetWidth;
		destInfo->dispHeight = targetHeight;
		destInfo->storeWidth = destInfo->dispWidth;
		destInfo->storeHeight = destInfo->dispHeight;
		destInfo->par2 = srcInfo->par2 * Math::UOSInt2Double(srcInfo->dispHeight) / Math::UOSInt2Double(srcInfo->dispWidth) * Math::UOSInt2Double(destInfo->dispWidth) / Math::UOSInt2Double(destInfo->dispHeight);
		destInfo->hdpi = srcInfo->hdpi * Math::UOSInt2Double(targetWidth) / Math::UOSInt2Double(srcInfo->dispWidth);
		destInfo->vdpi = srcInfo->vdpi * Math::UOSInt2Double(targetHeight) / Math::UOSInt2Double(srcInfo->dispHeight);
	}
	else if (rar == Media::IImgResizer::RAR_KEEPAR)
	{
		if (targetWidth * srcInfo->dispHeight > targetHeight * srcInfo->dispWidth)
		{
			destInfo->dispHeight = targetHeight;
			destInfo->dispWidth = (UOSInt)Math::Double2Int32((Double)targetHeight * (Double)srcInfo->dispWidth / (Double)srcInfo->dispHeight);
		}
		else
		{
			destInfo->dispWidth = targetWidth;
			destInfo->dispHeight = (UOSInt)Math::Double2Int32((Double)targetWidth * (Double)srcInfo->dispHeight / (Double)srcInfo->dispWidth);
		}
		destInfo->storeWidth = destInfo->dispWidth;
		destInfo->storeHeight = destInfo->dispHeight;
		destInfo->par2 = srcInfo->par2 * Math::UOSInt2Double(srcInfo->dispHeight) / Math::UOSInt2Double(srcInfo->dispWidth) * Math::UOSInt2Double(destInfo->dispWidth) / Math::UOSInt2Double(destInfo->dispHeight);
		destInfo->hdpi = srcInfo->hdpi * Math::UOSInt2Double(targetWidth) / Math::UOSInt2Double(srcInfo->dispWidth);
		destInfo->vdpi = srcInfo->vdpi * Math::UOSInt2Double(targetHeight) / Math::UOSInt2Double(srcInfo->dispHeight);
	}
	else
	{
		if (Math::UOSInt2Double(targetWidth) * Math::UOSInt2Double(srcInfo->dispHeight) * srcInfo->par2 > Math::UOSInt2Double(targetHeight) * Math::UOSInt2Double(srcInfo->dispWidth))
		{
			destInfo->dispHeight = targetHeight;
			destInfo->dispWidth = (UOSInt)Math::Double2Int32(Math::UOSInt2Double(targetHeight) / srcInfo->par2 * Math::UOSInt2Double(srcInfo->dispWidth) / Math::UOSInt2Double(srcInfo->dispHeight));
		}
		else
		{
			destInfo->dispWidth = targetWidth;
			destInfo->dispHeight = (UOSInt)Math::Double2Int32(Math::UOSInt2Double(targetWidth) * Math::UOSInt2Double(srcInfo->dispHeight) * srcInfo->par2 / Math::UOSInt2Double(srcInfo->dispWidth));
		}
		destInfo->storeWidth = destInfo->dispWidth;
		destInfo->storeHeight = destInfo->dispHeight;
		destInfo->par2 = srcInfo->par2 * Math::UOSInt2Double(srcInfo->dispHeight) / Math::UOSInt2Double(srcInfo->dispWidth) * Math::UOSInt2Double(destInfo->dispWidth) / Math::UOSInt2Double(destInfo->dispHeight);
		destInfo->hdpi = srcInfo->hdpi * Math::UOSInt2Double(targetWidth) / Math::UOSInt2Double(srcInfo->dispWidth);
		destInfo->vdpi = srcInfo->vdpi * Math::UOSInt2Double(targetHeight) / Math::UOSInt2Double(srcInfo->dispHeight);
	}
}
