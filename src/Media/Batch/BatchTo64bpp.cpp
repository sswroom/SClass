#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/StaticImage.h"
#include "Media/Batch/BatchTo64bpp.h"

Media::Batch::BatchTo64bpp::BatchTo64bpp(Media::Batch::BatchHandler *hdlr)
{
	this->hdlr = hdlr;
}

Media::Batch::BatchTo64bpp::~BatchTo64bpp()
{
}


void Media::Batch::BatchTo64bpp::SetHandler(Media::Batch::BatchHandler *hdlr)
{
	this->hdlr = hdlr;
}

void Media::Batch::BatchTo64bpp::ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId)
{
	UOSInt j;
	UOSInt k;
	NN<Media::StaticImage> newImg;
	j = 0;
	k = imgList->GetCount();
	while (j < k)
	{
		if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(j, 0)).SetTo(newImg))
		{
			newImg->To64bpp();
		}
		j++;
	}

	if (this->hdlr)
		this->hdlr->ImageOutput(imgList, fileId, subId);
}
