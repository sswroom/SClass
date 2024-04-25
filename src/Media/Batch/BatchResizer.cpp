#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/Batch/BatchResizer.h"
#include "Sync/Event.h"
#include "Sync/MutexUsage.h"

Media::Batch::BatchResizer::BatchResizer(Media::IImgResizer *resizer, Media::Batch::BatchHandler *hdlr)
{
	this->resizer = resizer;
	this->hdlr = hdlr;
}

Media::Batch::BatchResizer::~BatchResizer()
{
	ClearTargetSizes();
}

void Media::Batch::BatchResizer::AddTargetSize(UInt32 targetWidth, UInt32 targetHeight, NN<Text::String> targetId)
{
	TargetParam *param;
	param = MemAlloc(TargetParam, 1);
	param->width = targetWidth;
	param->height = targetHeight;
	param->sizeType = 0;
	param->targetId = targetId->Clone();
	this->targetParam.Add(param);
}

void Media::Batch::BatchResizer::AddTargetDPI(UInt32 targetHDPI, UInt32 targetVDPI, NN<Text::String> targetId)
{
	TargetParam *param;
	param = MemAlloc(TargetParam, 1);
	param->width = targetHDPI;
	param->height = targetVDPI;
	param->sizeType = 1;
	param->targetId = targetId->Clone();
	this->targetParam.Add(param);
}

void Media::Batch::BatchResizer::ClearTargetSizes()
{
	TargetParam *param;
	UOSInt i = this->targetParam.GetCount();
	while (i-- > 0)
	{
		param = this->targetParam.RemoveAt(i);
		param->targetId->Release();
		MemFree(param);
	}
}

void Media::Batch::BatchResizer::SetHandler(Media::Batch::BatchHandler *hdlr)
{
	this->hdlr = hdlr;
}

void Media::Batch::BatchResizer::ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	TargetParam *param;
	NN<Media::StaticImage> newImg;
	Media::StaticImage *rImg;
	Bool succ;
	UTF8Char sbuff[256];
	UTF8Char *sptr;


	i = this->targetParam.GetCount();
	while (i-- > 0)
	{
		param = this->targetParam.GetItem(i);

		Sync::MutexUsage mutUsage(this->resizeMut);
		if (param->sizeType == 0)
		{
			resizer->SetTargetSize(Math::Size2D<UOSInt>(param->width, param->height));
		}

		succ = true;
		sptr = imgList->GetSourceName(sbuff);
		Media::ImageList newImgList(CSTRP(sbuff, sptr));
		j = 0;
		k = imgList->GetCount();
		while (j < k)
		{
			if (newImg.Set((Media::StaticImage*)imgList->GetImage(j, 0)))
			{
				if (param->sizeType == 1)
				{
					resizer->SetTargetSize(Math::Size2D<UOSInt>((UInt32)Double2Int32(UOSInt2Double(newImg->info.dispSize.x * param->width) / newImg->info.hdpi),
						(UInt32)Double2Int32(UOSInt2Double(newImg->info.dispSize.y * param->height) / newImg->info.vdpi)));
					rImg = resizer->ProcessToNew(newImg);
				}
				else
				{
					rImg = resizer->ProcessToNew(newImg);
				}

				if (!newImg.Set(rImg))
				{
					succ = false;
					break;
				}
				else
				{
					newImgList.AddImage(newImg, 0);
				}
			}
			j++;
		}
		mutUsage.EndUse();
		if (succ)
		{
			if (this->hdlr)
				this->hdlr->ImageOutput(newImgList, fileId, param->targetId->ToCString());
		}
	}
}
