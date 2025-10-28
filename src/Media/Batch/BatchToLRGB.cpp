#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/StaticImage.h"
#include "Media/Batch/BatchToLRGB.h"
#include "Sync/MutexUsage.h"

Media::Batch::BatchToLRGB::BatchToLRGB(NN<const Media::ColorProfile> srcProfile, NN<const Media::ColorProfile> destProfile, Optional<Media::Batch::BatchHandler> hdlr) : srcProfile(srcProfile), destProfile(destProfile)
{
	this->hdlr = hdlr;
	this->csconv = 0;
	this->srcFCC = 0;
	this->srcBpp = 0;
}

Media::Batch::BatchToLRGB::~BatchToLRGB()
{
	this->csconv.Delete();
}

void Media::Batch::BatchToLRGB::SetHandler(Optional<Media::Batch::BatchHandler> hdlr)
{
	this->hdlr = hdlr;
}

void Media::Batch::BatchToLRGB::ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId)
{
	UOSInt i;
	UOSInt j;
	NN<Media::StaticImage> simg;
	NN<Media::StaticImage> dimg;
	i = 0;
	j = imgList->GetCount();
	while (i < j)
	{
		if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(i, 0)).SetTo(simg) && simg->info.fourcc != *(UInt32*)"LRGB")
		{
			Sync::MutexUsage mutUsage(this->mut);
			if (this->csconv.IsNull() || this->srcFCC != simg->info.fourcc || this->srcBpp != simg->info.storeBPP || this->srcPF != simg->info.pf || !simg->info.color.Equals(this->srcProfile))
			{
				this->csconv.Delete();
				this->srcFCC = simg->info.fourcc;
				this->srcBpp = simg->info.storeBPP;
				this->srcPF = simg->info.pf;
				this->srcProfile.Set(simg->info.color);
				this->csconv = Media::CS::CSConverter::NewConverter(this->srcFCC, this->srcBpp, this->srcPF, this->srcProfile, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, this->destProfile, simg->info.yuvType, 0);
			}
			NN<Media::CS::CSConverter> csconv;
			if (this->csconv.SetTo(csconv))
			{
				NEW_CLASSNN(dimg, Media::StaticImage(simg->info.dispSize, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, this->destProfile, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				dimg->info.hdpi = simg->info.hdpi;
				dimg->info.vdpi = simg->info.vdpi;
				csconv->ConvertV2(&simg->data, dimg->data, simg->info.dispSize.x, simg->info.dispSize.y, simg->info.storeSize.x, simg->info.storeSize.y, (OSInt)dimg->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
				imgList->ReplaceImage(i, dimg);
			}
			mutUsage.EndUse();
		}
		i++;
	}

	NN<Media::Batch::BatchHandler> hdlr;
	if (this->hdlr.SetTo(hdlr))
		hdlr->ImageOutput(imgList, fileId, subId);
}
