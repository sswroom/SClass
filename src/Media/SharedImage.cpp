#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/StaticImage.h"
#include "Media/SharedImage.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Sync/MutexUsage.h"

Media::SharedImage::SharedImage(Media::SharedImage::ImageStatus *status)
{
	this->imgStatus = status;
	Sync::MutexUsage mutUsage(this->imgStatus->mut);
	this->imgStatus->useCnt++;
	mutUsage.EndUse();
}

Media::SharedImage::SharedImage(Media::ImageList *imgList, Bool genPreview)
{
	UOSInt imgCnt = imgList->GetCount();
	this->imgStatus = MemAlloc(Media::SharedImage::ImageStatus, 1);
	this->imgStatus->imgList = imgList;
	this->imgStatus->prevList = 0;
	this->imgStatus->useCnt = 1;
	this->imgStatus->imgIndex = 0;
	this->imgStatus->imgDelay = 0;
	this->imgStatus->lastTimeTick = 0;
	NEW_CLASS(this->imgStatus->mut, Sync::Mutex());
	UOSInt i = imgCnt;
	while (i-- > 0)
	{
		imgList->ToStaticImage(i);
	}
	if (genPreview && imgCnt == 1)
	{
		Media::StaticImage *img = (Media::StaticImage*)imgList->GetImage(0, 0);
		if (img->info->dispWidth >= 640 || img->info->dispHeight >= 640)
		{
			UOSInt currWidth = img->info->dispWidth;
			UOSInt currHeight = img->info->dispHeight;
			Media::StaticImage *simg;
			Media::Resizer::LanczosResizer8_C8 resizer(3, 3, img->info->color, img->info->color, 0, img->info->atype);
			img->To32bpp();
			NEW_CLASS(this->imgStatus->prevList, Data::ArrayList<Media::StaticImage*>());

			while (currWidth >= 640 || currHeight >= 640)
			{
				currWidth >>= 1;
				currHeight >>= 1;
				resizer.SetTargetWidth(currWidth);
				resizer.SetTargetHeight(currHeight);
				simg = resizer.ProcessToNew(img);
				if (simg)
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
		DEL_CLASS(this->imgStatus->mut);
		DEL_CLASS(this->imgStatus->imgList);
		if (this->imgStatus->prevList)
		{
			UOSInt i;
			Media::StaticImage *simg;
			i = this->imgStatus->prevList->GetCount();
			while (i-- > 0)
			{
				simg = this->imgStatus->prevList->GetItem(i);
				DEL_CLASS(simg);
			}
			DEL_CLASS(this->imgStatus->prevList);
		}
		MemFree(this->imgStatus);
	}
}

Media::SharedImage *Media::SharedImage::Clone()
{
	Media::SharedImage *newImg;
	NEW_CLASS(newImg, Media::SharedImage(this->imgStatus));
	return newImg;
}

Media::StaticImage *Media::SharedImage::GetImage(UInt32 *imgTimeMS)
{
	UInt32 currDelay;
	Media::StaticImage *img;
	Int64 currTimeTick;
	if (imgTimeMS == 0)
	{
		imgTimeMS = &currDelay;
	}
	if (this->imgStatus->imgDelay == 0)
	{
		img = (Media::StaticImage*)this->imgStatus->imgList->GetImage(this->imgStatus->imgIndex, &currDelay);
		if (currDelay == 0)
		{
			*imgTimeMS = 0;
			return img;
		}
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		this->imgStatus->lastTimeTick = dt.ToTicks();
		this->imgStatus->imgDelay = currDelay;
		*imgTimeMS = this->imgStatus->imgDelay;
		return img;
	}
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	currTimeTick = dt.ToTicks();
	if ((currTimeTick - this->imgStatus->lastTimeTick) >= this->imgStatus->imgDelay)
	{
		this->imgStatus->imgIndex++;
		if (this->imgStatus->imgIndex >= this->imgStatus->imgList->GetCount())
		{
			this->imgStatus->imgIndex = 0;
		}
		img = (Media::StaticImage*)this->imgStatus->imgList->GetImage(this->imgStatus->imgIndex, &currDelay);
		this->imgStatus->lastTimeTick = currTimeTick;
		this->imgStatus->imgDelay = currDelay;
		*imgTimeMS = this->imgStatus->imgDelay;
		return img;
	}
	else
	{
		img = (Media::StaticImage*)this->imgStatus->imgList->GetImage(this->imgStatus->imgIndex, &currDelay);
		*imgTimeMS = (UInt32)(this->imgStatus->imgDelay - (UInt64)(currTimeTick - this->imgStatus->lastTimeTick));
		return img;
	}
}

Media::StaticImage *Media::SharedImage::GetPrevImage(Double width, Double height, UInt32 *imgTimeMS)
{
	if (this->imgStatus->prevList == 0)
	{
		return this->GetImage(imgTimeMS);
	}
	Media::StaticImage *currImg;
	UOSInt i;
	Media::StaticImage *minImg = 0;
	UOSInt minWidth = 0;
	i = this->imgStatus->prevList->GetCount();
	while (i-- > 0)
	{
		currImg = this->imgStatus->prevList->GetItem(i);
		if (UOSInt2Double(currImg->info->dispWidth) >= width && UOSInt2Double(currImg->info->dispHeight) >= height)
		{
			if (minImg == 0 || minWidth > currImg->info->dispWidth)
			{
				minImg = currImg;
				minWidth = currImg->info->dispWidth;
			}
		}
	}
	if (imgTimeMS)
	{
		*imgTimeMS = 0;
	}
	if (minImg)
		return minImg;
	return (Media::StaticImage*)this->imgStatus->imgList->GetImage(0, 0);
}
