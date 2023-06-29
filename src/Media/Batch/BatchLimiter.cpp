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

void Media::Batch::BatchLimiter::ImageOutput(Media::ImageList *imgList, const UTF8Char *fileId, const UTF8Char *subId)
{
	UOSInt j;
	UOSInt k;
	Media::StaticImage *newImg;
	j = 0;
	k = imgList->GetCount();
	while (j < k)
	{
		newImg = (Media::StaticImage*)imgList->GetImage(j, 0);
		if (newImg->info.fourcc == *(UInt32*)"LRGB")
		{
			LRGBLimiter_LimitImageLRGB(newImg->data, newImg->info.storeSize.x, newImg->info.storeSize.y);
		}
		j++;
	}

	if (this->hdlr)
		this->hdlr->ImageOutput(imgList, fileId, subId);
}
