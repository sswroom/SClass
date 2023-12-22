#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/StaticImage.h"
#include "Media/Batch/BatchTo32bpp.h"

Media::Batch::BatchTo32bpp::BatchTo32bpp(Media::Batch::BatchHandler *hdlr)
{
	this->hdlr = hdlr;
}

Media::Batch::BatchTo32bpp::~BatchTo32bpp()
{
}


void Media::Batch::BatchTo32bpp::SetHandler(Media::Batch::BatchHandler *hdlr)
{
	this->hdlr = hdlr;
}

void Media::Batch::BatchTo32bpp::ImageOutput(NotNullPtr<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId)
{
	OSInt j;
	OSInt k;
	Media::StaticImage *newImg;
	j = 0;
	k = imgList->GetCount();
	while (j < k)
	{
		newImg = (Media::StaticImage*)imgList->GetImage(j, 0);
		newImg->To32bpp();
		j++;
	}

	if (this->hdlr)
		this->hdlr->ImageOutput(imgList, fileId, subId);
}
