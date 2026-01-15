#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/StaticImage.h"
#include "Media/Batch/BatchTo64bpp.h"

Media::Batch::BatchTo64bpp::BatchTo64bpp(Optional<Media::Batch::BatchHandler> hdlr)
{
	this->hdlr = hdlr;
}

Media::Batch::BatchTo64bpp::~BatchTo64bpp()
{
}


void Media::Batch::BatchTo64bpp::SetHandler(Optional<Media::Batch::BatchHandler> hdlr)
{
	this->hdlr = hdlr;
}

void Media::Batch::BatchTo64bpp::ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId)
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
			newImg->ToB16G16R16A16();
		}
		j++;
	}

	NN<Media::Batch::BatchHandler> hdlr;
	if (this->hdlr.SetTo(hdlr))
		hdlr->ImageOutput(imgList, fileId, subId);
}
