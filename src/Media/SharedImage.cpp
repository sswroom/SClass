#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/StaticImage.h"
#include "Media/SharedImage.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Sync/MutexUsage.h"

Media::SharedImage::SharedImage(NN<Media::SharedImage::ImageStatus> status)
{
	this->imgStatus = status;
	Sync::MutexUsage mutUsage(this->imgStatus->mut);
	this->imgStatus->useCnt++;
	mutUsage.EndUse();
}

Media::SharedImage::SharedImage(NN<Media::ImageList> imgList, Bool genPreview)
{
	NN<Media::StaticImage> img;
	UOSInt imgCnt = imgList->GetCount();
	NEW_CLASSNN(this->imgStatus, ImageStatus());
	this->imgStatus->imgList = imgList;
	this->imgStatus->prevList = 0;
	this->imgStatus->useCnt = 1;
	this->imgStatus->imgIndex = 0;
	this->imgStatus->imgDelay = 0;
	this->imgStatus->lastTimeTick = 0;
	UOSInt i = imgCnt;
	while (i-- > 0)
	{
		imgList->ToStaticImage(i);
	}
	if (genPreview && imgCnt == 1)
	{
		if (img.Set((Media::StaticImage*)imgList->GetImage(0, 0)) && (img->info.dispSize.x >= 640 || img->info.dispSize.y >= 640))
		{
			UOSInt currWidth = img->info.dispSize.x;
			UOSInt currHeight = img->info.dispSize.y;
			NN<Media::StaticImage> simg;
			Media::Resizer::LanczosResizer8_C8 resizer(3, 3, img->info.color, img->info.color, 0, img->info.atype);
			img->To32bpp();
			NEW_CLASS(this->imgStatus->prevList, Data::ArrayListNN<Media::StaticImage>());

			while (currWidth >= 640 || currHeight >= 640)
			{
				currWidth >>= 1;
				currHeight >>= 1;
				resizer.SetTargetSize(Math::Size2D<UOSInt>(currWidth, currHeight));
				if (simg.Set(resizer.ProcessToNew(img)))
				{
					this->imgStatus->prevList->Add(simg);
				}
			}
		}
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
		if (this->imgStatus->prevList)
		{
			UOSInt i;
			NN<Media::StaticImage> simg;
			i = this->imgStatus->prevList->GetCount();
			while (i-- > 0)
			{
				simg = this->imgStatus->prevList->GetItemNoCheck(i);
				simg.Delete();
			}
			DEL_CLASS(this->imgStatus->prevList);
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

Media::StaticImage *Media::SharedImage::GetImage(OptOut<UInt32> imgTimeMS) const
{
	UInt32 currDelay;
	Media::StaticImage *img;
	Int64 currTimeTick;
	if (this->imgStatus->imgDelay == 0)
	{
		img = (Media::StaticImage*)this->imgStatus->imgList->GetImage(this->imgStatus->imgIndex, currDelay);
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
		img = (Media::StaticImage*)this->imgStatus->imgList->GetImage(this->imgStatus->imgIndex, currDelay);
		this->imgStatus->lastTimeTick = currTimeTick;
		this->imgStatus->imgDelay = currDelay;
		imgTimeMS.Set(this->imgStatus->imgDelay);
		return img;
	}
	else
	{
		img = (Media::StaticImage*)this->imgStatus->imgList->GetImage(this->imgStatus->imgIndex, currDelay);
		imgTimeMS.Set((UInt32)(this->imgStatus->imgDelay - (UInt64)(currTimeTick - this->imgStatus->lastTimeTick)));
		return img;
	}
}

Optional<Media::StaticImage> Media::SharedImage::GetPrevImage(Double width, Double height, OptOut<UInt32> imgTimeMS) const
{
	if (this->imgStatus->prevList == 0)
	{
		return this->GetImage(imgTimeMS);
	}
	NN<Media::StaticImage> currImg;
	UOSInt i;
	Optional<Media::StaticImage> minImg = 0;
	UOSInt minWidth = 0;
	i = this->imgStatus->prevList->GetCount();
	while (i-- > 0)
	{
		currImg = this->imgStatus->prevList->GetItemNoCheck(i);
		if (UOSInt2Double(currImg->info.dispSize.x) >= width && UOSInt2Double(currImg->info.dispSize.y) >= height)
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
	return (Media::StaticImage*)this->imgStatus->imgList->GetImage(0, 0);
}
