#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/ImageResizer.h"
#include "Media/CS/TransferFunc.h"

Media::ImageResizer::ImageResizer(Media::AlphaType srcAlphaType)
{
	this->targetSize = Math::Size2D<UOSInt>(0, 0);
	this->srcAlphaType = srcAlphaType;
	this->rar = Media::ImageResizer::RAR_KEEPAR;
}

void Media::ImageResizer::SetTargetSize(Math::Size2D<UOSInt> targetSize)
{
	if ((OSInt)targetSize.x < 0)
	{
		this->targetSize.x = (UOSInt)-(OSInt)targetSize.x;
	}
	else
	{
		this->targetSize.x = targetSize.x;
	}
	if ((OSInt)targetSize.y < 0)
	{
		this->targetSize.y = (UOSInt)-(OSInt)targetSize.y;
	}
	else
	{
		this->targetSize.y = targetSize.y;
	}
}

void Media::ImageResizer::SetResizeAspectRatio(ResizeAspectRatio rar)
{
	this->rar = rar;
}

void Media::ImageResizer::SetSrcAlphaType(Media::AlphaType alphaType)
{
	this->srcAlphaType = alphaType;
}

void Media::ImageResizer::SetSrcRefLuminance(Double srcRefLuminance)
{
}

Media::StaticImage *Media::ImageResizer::ProcessToNew(NN<const Media::RasterImage> srcImage)
{
	this->SetSrcRefLuminance(Media::CS::TransferFunc::GetRefLuminance(srcImage->info.color.rtransfer));
	return ProcessToNewPartial(srcImage, Math::Size2DDbl(0, 0), srcImage->info.dispSize.ToDouble());
}

void Media::ImageResizer::CalOutputSize(NN<const Media::FrameInfo> srcInfo, Math::Size2D<UOSInt> targetSize, NN<Media::FrameInfo> destInfo, Media::ImageResizer::ResizeAspectRatio rar)
{
	destInfo->Set(srcInfo);
	if (targetSize.x == 0 && targetSize.y == 0)
		return;
	if (rar == Media::ImageResizer::RAR_IGNOREAR)
	{
		destInfo->dispSize = targetSize;
		destInfo->storeSize = destInfo->dispSize;
		destInfo->par2 = srcInfo->par2 * UOSInt2Double(srcInfo->dispSize.GetHeight()) / UOSInt2Double(srcInfo->dispSize.GetWidth()) * UOSInt2Double(destInfo->dispSize.GetWidth()) / UOSInt2Double(destInfo->dispSize.GetHeight());
		destInfo->hdpi = srcInfo->hdpi * UOSInt2Double(targetSize.GetWidth()) / UOSInt2Double(srcInfo->dispSize.GetWidth());
		destInfo->vdpi = srcInfo->vdpi * UOSInt2Double(targetSize.GetHeight()) / UOSInt2Double(srcInfo->dispSize.GetHeight());
	}
	else if (rar == Media::ImageResizer::RAR_KEEPAR)
	{
		if (targetSize.GetWidth() * srcInfo->dispSize.GetHeight() > targetSize.GetHeight() * srcInfo->dispSize.GetWidth())
		{
			destInfo->dispSize.y = targetSize.GetHeight();
			destInfo->dispSize.x = (UOSInt)Double2Int32((Double)targetSize.GetHeight() * (Double)srcInfo->dispSize.GetWidth() / (Double)srcInfo->dispSize.GetHeight());
		}
		else
		{
			destInfo->dispSize.x = targetSize.GetWidth();
			destInfo->dispSize.y = (UOSInt)Double2Int32((Double)targetSize.GetWidth() * (Double)srcInfo->dispSize.GetHeight() / (Double)srcInfo->dispSize.GetWidth());
		}
		destInfo->storeSize = destInfo->dispSize;
		destInfo->par2 = srcInfo->par2 * UOSInt2Double(srcInfo->dispSize.GetHeight()) / UOSInt2Double(srcInfo->dispSize.GetWidth()) * UOSInt2Double(destInfo->dispSize.GetWidth()) / UOSInt2Double(destInfo->dispSize.GetHeight());
		destInfo->hdpi = srcInfo->hdpi * UOSInt2Double(targetSize.GetWidth()) / UOSInt2Double(srcInfo->dispSize.GetWidth());
		destInfo->vdpi = srcInfo->vdpi * UOSInt2Double(targetSize.GetHeight()) / UOSInt2Double(srcInfo->dispSize.GetHeight());
	}
	else
	{
		if (UOSInt2Double(targetSize.GetWidth()) * UOSInt2Double(srcInfo->dispSize.GetHeight()) * srcInfo->par2 > UOSInt2Double(targetSize.GetHeight()) * UOSInt2Double(srcInfo->dispSize.GetWidth()))
		{
			destInfo->dispSize.y = targetSize.GetHeight();
			destInfo->dispSize.x = (UOSInt)Double2Int32(UOSInt2Double(targetSize.GetHeight()) / srcInfo->par2 * UOSInt2Double(srcInfo->dispSize.GetWidth()) / UOSInt2Double(srcInfo->dispSize.GetHeight()));
		}
		else
		{
			destInfo->dispSize.x = targetSize.GetWidth();
			destInfo->dispSize.y = (UOSInt)Double2Int32(UOSInt2Double(targetSize.GetWidth()) * UOSInt2Double(srcInfo->dispSize.GetHeight()) * srcInfo->par2 / UOSInt2Double(srcInfo->dispSize.GetWidth()));
		}
		destInfo->storeSize = destInfo->dispSize;
		destInfo->par2 = srcInfo->par2 * UOSInt2Double(srcInfo->dispSize.GetHeight()) / UOSInt2Double(srcInfo->dispSize.GetWidth()) * UOSInt2Double(destInfo->dispSize.GetWidth()) / UOSInt2Double(destInfo->dispSize.GetHeight());
		destInfo->hdpi = srcInfo->hdpi * UOSInt2Double(targetSize.GetWidth()) / UOSInt2Double(srcInfo->dispSize.GetWidth());
		destInfo->vdpi = srcInfo->vdpi * UOSInt2Double(targetSize.GetHeight()) / UOSInt2Double(srcInfo->dispSize.GetHeight());
	}
}
