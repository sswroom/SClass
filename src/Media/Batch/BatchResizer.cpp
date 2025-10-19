#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/Batch/BatchResizer.h"
#include "Sync/Event.h"
#include "Sync/MutexUsage.h"

Media::Batch::BatchResizer::BatchResizer(NN<Media::ImageResizer> resizer, Optional<Media::Batch::BatchHandler> hdlr)
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
	NN<TargetParam> param;
	param = MemAllocNN(TargetParam);
	param->width = targetWidth;
	param->height = targetHeight;
	param->sizeType = 0;
	param->targetId = targetId->Clone();
	this->targetParam.Add(param);
}

void Media::Batch::BatchResizer::AddTargetDPI(UInt32 targetHDPI, UInt32 targetVDPI, NN<Text::String> targetId)
{
	NN<TargetParam> param;
	param = MemAllocNN(TargetParam);
	param->width = targetHDPI;
	param->height = targetVDPI;
	param->sizeType = 1;
	param->targetId = targetId->Clone();
	this->targetParam.Add(param);
}

void Media::Batch::BatchResizer::ClearTargetSizes()
{
	NN<TargetParam> param;
	UOSInt i = this->targetParam.GetCount();
	while (i-- > 0)
	{
		param = this->targetParam.GetItemNoCheck(i);
		param->targetId->Release();
		MemFreeNN(param);
	}
	this->targetParam.Clear();
}

void Media::Batch::BatchResizer::SetHandler(Optional<Media::Batch::BatchHandler> hdlr)
{
	this->hdlr = hdlr;
}

void Media::Batch::BatchResizer::ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN subId)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	NN<TargetParam> param;
	NN<Media::StaticImage> newImg;
	Optional<Media::StaticImage> rImg;
	Bool succ;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<Media::Batch::BatchHandler> hdlr;

	i = this->targetParam.GetCount();
	while (i-- > 0)
	{
		param = this->targetParam.GetItemNoCheck(i);

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
			if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(j, 0)).SetTo(newImg))
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

				if (!rImg.SetTo(newImg))
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
			if (this->hdlr.SetTo(hdlr))
				hdlr->ImageOutput(newImgList, fileId, param->targetId->ToCString());
		}
	}
}
