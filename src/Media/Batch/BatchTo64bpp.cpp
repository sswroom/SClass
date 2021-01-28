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

void Media::Batch::BatchTo64bpp::ImageOutput(Media::ImageList *imgList, const UTF8Char *fileId, const UTF8Char *subId)
{
	OSInt j;
	OSInt k;
	Media::StaticImage *newImg;
	j = 0;
	k = imgList->GetCount();
	while (j < k)
	{
		newImg = (Media::StaticImage*)imgList->GetImage(j, 0);
		newImg->To64bpp();
		j++;
	}

	if (this->hdlr)
		this->hdlr->ImageOutput(imgList, fileId, subId);
}
