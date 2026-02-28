#include "Stdafx.h"
#include "Math/Math_C.h"
#include "Media/ImageResizer.h"
#include "Media/CS/TransferFunc.h"

Media::ImageResizer::ImageResizer(Media::AlphaType srcAlphaType)
{
	this->targetSize = Math::Size2D<UIntOS>(0, 0);
	this->srcAlphaType = srcAlphaType;
	this->rar = Media::ImageResizer::RAR_KEEPAR;
}

void Media::ImageResizer::SetTargetSize(Math::Size2D<UIntOS> targetSize)
{
	if ((IntOS)targetSize.x < 0)
	{
		this->targetSize.x = (UIntOS)-(IntOS)targetSize.x;
	}
	else
	{
		this->targetSize.x = targetSize.x;
	}
	if ((IntOS)targetSize.y < 0)
	{
		this->targetSize.y = (UIntOS)-(IntOS)targetSize.y;
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

Optional<Media::StaticImage> Media::ImageResizer::ProcessToNew(NN<const Media::RasterImage> srcImage)
{
	this->SetSrcRefLuminance(Media::CS::TransferFunc::GetRefLuminance(srcImage->info.color.rtransfer));
	return ProcessToNewPartial(srcImage, Math::Size2DDbl(0, 0), srcImage->info.dispSize.ToDouble());
}

void Media::ImageResizer::CalOutputSize(NN<const Media::FrameInfo> srcInfo, Math::Size2D<UIntOS> targetSize, NN<Media::FrameInfo> destInfo, Media::ImageResizer::ResizeAspectRatio rar)
{
	destInfo->Set(srcInfo);
	if (targetSize.x == 0 && targetSize.y == 0)
		return;
	if (rar == Media::ImageResizer::RAR_IGNOREAR)
	{
		destInfo->dispSize = targetSize;
		destInfo->storeSize = destInfo->dispSize;
		destInfo->hdpi = srcInfo->hdpi * UIntOS2Double(targetSize.GetWidth()) / UIntOS2Double(srcInfo->dispSize.GetWidth());
		destInfo->vdpi = srcInfo->vdpi * UIntOS2Double(targetSize.GetHeight()) / UIntOS2Double(srcInfo->dispSize.GetHeight());
	}
	else if (rar == Media::ImageResizer::RAR_KEEPAR)
	{
		if (targetSize.GetWidth() * srcInfo->dispSize.GetHeight() > targetSize.GetHeight() * srcInfo->dispSize.GetWidth())
		{
			destInfo->dispSize.y = targetSize.GetHeight();
			destInfo->dispSize.x = (UIntOS)Double2Int32((Double)targetSize.GetHeight() * (Double)srcInfo->dispSize.GetWidth() / (Double)srcInfo->dispSize.GetHeight());
		}
		else
		{
			destInfo->dispSize.x = targetSize.GetWidth();
			destInfo->dispSize.y = (UIntOS)Double2Int32((Double)targetSize.GetWidth() * (Double)srcInfo->dispSize.GetHeight() / (Double)srcInfo->dispSize.GetWidth());
		}
		destInfo->storeSize = destInfo->dispSize;
		destInfo->hdpi = srcInfo->hdpi * UIntOS2Double(targetSize.GetWidth()) / UIntOS2Double(srcInfo->dispSize.GetWidth());
		destInfo->vdpi = srcInfo->vdpi * UIntOS2Double(targetSize.GetHeight()) / UIntOS2Double(srcInfo->dispSize.GetHeight());
	}
	else
	{
		if (UIntOS2Double(targetSize.GetWidth()) * UIntOS2Double(srcInfo->dispSize.GetHeight()) * srcInfo->CalcPAR() > UIntOS2Double(targetSize.GetHeight()) * UIntOS2Double(srcInfo->dispSize.GetWidth()))
		{
			destInfo->dispSize.y = targetSize.GetHeight();
			destInfo->dispSize.x = (UIntOS)Double2Int32(UIntOS2Double(targetSize.GetHeight()) / srcInfo->CalcPAR() * UIntOS2Double(srcInfo->dispSize.GetWidth()) / UIntOS2Double(srcInfo->dispSize.GetHeight()));
		}
		else
		{
			destInfo->dispSize.x = targetSize.GetWidth();
			destInfo->dispSize.y = (UIntOS)Double2Int32(UIntOS2Double(targetSize.GetWidth()) * UIntOS2Double(srcInfo->dispSize.GetHeight()) * srcInfo->CalcPAR() / UIntOS2Double(srcInfo->dispSize.GetWidth()));
		}
		destInfo->storeSize = destInfo->dispSize;
		destInfo->hdpi = srcInfo->hdpi * UIntOS2Double(targetSize.GetWidth()) / UIntOS2Double(srcInfo->dispSize.GetWidth());
		destInfo->vdpi = srcInfo->vdpi * UIntOS2Double(targetSize.GetHeight()) / UIntOS2Double(srcInfo->dispSize.GetHeight());
	}
}
