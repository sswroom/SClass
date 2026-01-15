#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/StaticImage.h"
#include "Media/Batch/BatchTo32bpp.h"

Media::Batch::BatchTo32bpp::BatchTo32bpp(Optional<Media::Batch::BatchHandler> hdlr)
{
	this->hdlr = hdlr;
}

Media::Batch::BatchTo32bpp::~BatchTo32bpp()
{
}


void Media::Batch::BatchTo32bpp::SetHandler(Optional<Media::Batch::BatchHandler> hdlr)
{
	this->hdlr = hdlr;
}

void Media::Batch::BatchTo32bpp::ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId)
{
	UIntOS j;
	UIntOS k;
	NN<Media::StaticImage> newImg;
	j = 0;
	k = imgList->GetCount();
	while (j < k)
	{
		if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(j, 0)).SetTo(newImg))
		{
			newImg->ToB8G8R8A8();
		}
		j++;
	}

	NN<BatchHandler> hdlr;
	if (this->hdlr.SetTo(hdlr))
		hdlr->ImageOutput(imgList, fileId, subId);
}
