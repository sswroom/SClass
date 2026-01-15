#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/StaticImage.h"
#include "Media/SharedImage.h"
#include "Sync/MutexUsage.h"

Media::SharedImage::SharedImage(NN<Media::SharedImage::ImageStatus> status)
{
	this->imgStatus = status;
	Sync::MutexUsage mutUsage(this->imgStatus->mut);
	this->imgStatus->useCnt++;
	mutUsage.EndUse();
}

Media::SharedImage::SharedImage(NN<Media::ImageList> imgList, Optional<Data::ArrayListNN<Media::StaticImage>> previewImages)
{
	UIntOS imgCnt = imgList->GetCount();
	NEW_CLASSNN(this->imgStatus, ImageStatus());
	this->imgStatus->imgList = imgList;
	this->imgStatus->prevList = nullptr;
	this->imgStatus->useCnt = 1;
	this->imgStatus->imgIndex = 0;
	this->imgStatus->imgDelay = 0;
	this->imgStatus->lastTimeTick = 0;
	UIntOS i = imgCnt;
	while (i-- > 0)
	{
		imgList->ToStaticImage(i);
	}
	NN<Data::ArrayListNN<Media::StaticImage>> nnpreviewImages;
	if (previewImages.SetTo(nnpreviewImages) && nnpreviewImages->GetCount() > 0)
	{
		NN<Data::ArrayListNN<Media::StaticImage>> prevList;
		NEW_CLASSNN(prevList, Data::ArrayListNN<Media::StaticImage>());
		this->imgStatus->prevList = prevList;
		prevList->AddAll(nnpreviewImages);
	}
}

Media::SharedImage::~SharedImage()
{
	Bool toDelete = false;
	Sync::MutexUsage mutUsage(this->imgStatus->mut);
	if (--this->imgStatus->useCnt <= 0)
	{
		toDelete = true;
	}
	mutUsage.EndUse();
	if (toDelete)
	{
		this->imgStatus->imgList.Delete();
		NN<Data::ArrayListNN<Media::StaticImage>> nnprevList;
		if (this->imgStatus->prevList.SetTo(nnprevList))
		{
			nnprevList->DeleteAll();
			this->imgStatus->prevList.Delete();
		}
		this->imgStatus.Delete();
	}
}

NN<Media::SharedImage> Media::SharedImage::Clone() const
{
	NN<Media::SharedImage> newImg;
	NEW_CLASSNN(newImg, Media::SharedImage(this->imgStatus));
	return newImg;
}

Optional<Media::StaticImage> Media::SharedImage::GetImage(OptOut<UInt32> imgTimeMS) const
{
	UInt32 currDelay;
	Optional<Media::StaticImage> img;
	Int64 currTimeTick;
	if (this->imgStatus->imgDelay == 0)
	{
		img = Optional<Media::StaticImage>::ConvertFrom(this->imgStatus->imgList->GetImage(this->imgStatus->imgIndex, currDelay));
		if (currDelay == 0)
		{
			imgTimeMS.Set(0);
			return img;
		}
		this->imgStatus->lastTimeTick = Data::DateTimeUtil::GetCurrTimeMillis();
		this->imgStatus->imgDelay = currDelay;
		imgTimeMS.Set(this->imgStatus->imgDelay);
		return img;
	}
	currTimeTick = Data::DateTimeUtil::GetCurrTimeMillis();
	if ((currTimeTick - this->imgStatus->lastTimeTick) >= this->imgStatus->imgDelay)
	{
		this->imgStatus->imgIndex++;
		if (this->imgStatus->imgIndex >= this->imgStatus->imgList->GetCount())
		{
			this->imgStatus->imgIndex = 0;
		}
		img = Optional<Media::StaticImage>::ConvertFrom(this->imgStatus->imgList->GetImage(this->imgStatus->imgIndex, currDelay));
		this->imgStatus->lastTimeTick = currTimeTick;
		this->imgStatus->imgDelay = currDelay;
		imgTimeMS.Set(this->imgStatus->imgDelay);
		return img;
	}
	else
	{
		img = Optional<Media::StaticImage>::ConvertFrom(this->imgStatus->imgList->GetImage(this->imgStatus->imgIndex, currDelay));
		imgTimeMS.Set((UInt32)(this->imgStatus->imgDelay - (UInt64)(currTimeTick - this->imgStatus->lastTimeTick)));
		return img;
	}
}

Optional<Media::StaticImage> Media::SharedImage::GetPrevImage(Double width, Double height, OptOut<UInt32> imgTimeMS) const
{
	NN<Data::ArrayListNN<Media::StaticImage>> nnprevList;
	if (!this->imgStatus->prevList.SetTo(nnprevList))
	{
		return this->GetImage(imgTimeMS);
	}
	NN<Media::StaticImage> currImg;
	UIntOS i;
	Optional<Media::StaticImage> minImg = nullptr;
	UIntOS minWidth = 0;
	i = nnprevList->GetCount();
	while (i-- > 0)
	{
		currImg = nnprevList->GetItemNoCheck(i);
		if (UIntOS2Double(currImg->info.dispSize.x) >= width && UIntOS2Double(currImg->info.dispSize.y) >= height)
		{
			if (minImg.IsNull() || minWidth > currImg->info.dispSize.x)
			{
				minImg = currImg;
				minWidth = currImg->info.dispSize.x;
			}
		}
	}
	imgTimeMS.Set(0);
	if (minImg.NotNull())
		return minImg;
	return Optional<Media::StaticImage>::ConvertFrom(this->imgStatus->imgList->GetImage(0, 0));
}
