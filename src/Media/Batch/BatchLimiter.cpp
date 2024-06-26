#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/LRGBLimiterC.h"
#include "Media/StaticImage.h"
#include "Media/Batch/BatchLimiter.h"

Media::Batch::BatchLimiter::BatchLimiter(Media::Batch::BatchHandler *hdlr)
{
	this->hdlr = hdlr;
}

Media::Batch::BatchLimiter::~BatchLimiter()
{
}


void Media::Batch::BatchLimiter::SetHandler(Media::Batch::BatchHandler *hdlr)
{
	this->hdlr = hdlr;
}

void Media::Batch::BatchLimiter::ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId)
{
	UOSInt j;
	UOSInt k;
	NN<Media::StaticImage> newImg;
	j = 0;
	k = imgList->GetCount();
	while (j < k)
	{
		if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(j, 0)).SetTo(newImg) && newImg->info.fourcc == *(UInt32*)"LRGB")
		{
			LRGBLimiter_LimitImageLRGB(newImg->data.Ptr(), newImg->info.storeSize.x, newImg->info.storeSize.y);
		}
		j++;
	}

	if (this->hdlr)
		this->hdlr->ImageOutput(imgList, fileId, subId);
}
