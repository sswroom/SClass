#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/StaticImage.h"
#include "Media/Batch/BatchToLRGB.h"
#include "Sync/MutexUsage.h"

Media::Batch::BatchToLRGB::BatchToLRGB(Media::ColorProfile *srcProfile, Media::ColorProfile *destProfile, Media::Batch::BatchHandler *hdlr)
{
	NEW_CLASS(this->srcProfile, Media::ColorProfile(srcProfile));
	NEW_CLASS(this->destProfile, Media::ColorProfile(destProfile));
	this->hdlr = hdlr;
	NEW_CLASS(this->mut, Sync::Mutex());
	this->csconv = 0;
	this->srcFCC = 0;
	this->srcBpp = 0;
}

Media::Batch::BatchToLRGB::~BatchToLRGB()
{
	SDEL_CLASS(this->csconv);
	DEL_CLASS(this->mut);
	DEL_CLASS(this->srcProfile);
	DEL_CLASS(this->destProfile);
}

void Media::Batch::BatchToLRGB::SetHandler(Media::Batch::BatchHandler *hdlr)
{
	this->hdlr = hdlr;
}

void Media::Batch::BatchToLRGB::ImageOutput(Media::ImageList *imgList, const UTF8Char *fileId, const UTF8Char *subId)
{
	UOSInt i;
	UOSInt j;
	Media::StaticImage *simg;
	Media::StaticImage *dimg;
	i = 0;
	j = imgList->GetCount();
	while (i < j)
	{
		simg = (Media::StaticImage*)imgList->GetImage(i, 0);
		if (simg->info.fourcc != *(UInt32*)"LRGB")
		{
			Sync::MutexUsage mutUsage(this->mut);
			if (this->csconv == 0 || this->srcFCC != simg->info.fourcc || this->srcBpp != simg->info.storeBPP || this->srcPF != simg->info.pf || !simg->info.color->Equals(this->srcProfile))
			{
				SDEL_CLASS(this->csconv);
				this->srcFCC = simg->info.fourcc;
				this->srcBpp = simg->info.storeBPP;
				this->srcPF = simg->info.pf;
				this->srcProfile->Set(simg->info.color);
				this->csconv = Media::CS::CSConverter::NewConverter(this->srcFCC, this->srcBpp, this->srcPF, this->srcProfile, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, this->destProfile, simg->info.yuvType, 0);
			}
			if (this->csconv)
			{
				NEW_CLASS(dimg, Media::StaticImage(simg->info.dispWidth, simg->info.dispHeight, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, this->destProfile, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				dimg->info.hdpi = simg->info.hdpi;
				dimg->info.vdpi = simg->info.vdpi;
				this->csconv->ConvertV2(&simg->data, dimg->data, simg->info.dispWidth, simg->info.dispHeight, simg->info.storeWidth, simg->info.storeHeight, (OSInt)dimg->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
				imgList->ReplaceImage(i, dimg);
			}
			mutUsage.EndUse();
		}
		i++;
	}

	if (this->hdlr)
		this->hdlr->ImageOutput(imgList, fileId, subId);
}
