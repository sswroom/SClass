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
	NEW_CLASS(this->resizeMut, Sync::Mutex());
	NEW_CLASS(this->targetParam, Data::ArrayList<TargetParam*>());
}

Media::Batch::BatchResizer::~BatchResizer()
{
	ClearTargetSizes();
	DEL_CLASS(this->targetParam);
	DEL_CLASS(this->resizeMut);
}

void Media::Batch::BatchResizer::AddTargetSize(Int32 targetWidth, Int32 targetHeight, const UTF8Char *targetId)
{
	TargetParam *param;
	param = MemAlloc(TargetParam, 1);
	param->width = targetWidth;
	param->height = targetHeight;
	param->sizeType = 0;
	param->targetId = Text::StrCopyNew(targetId);
	this->targetParam->Add(param);
}

void Media::Batch::BatchResizer::AddTargetDPI(Int32 targetHDPI, Int32 targetVDPI, const UTF8Char *targetId)
{
	TargetParam *param;
	param = MemAlloc(TargetParam, 1);
	param->width = targetHDPI;
	param->height = targetVDPI;
	param->sizeType = 1;
	param->targetId = Text::StrCopyNew(targetId);
	this->targetParam->Add(param);
}

void Media::Batch::BatchResizer::ClearTargetSizes()
{
	TargetParam *param;
	OSInt i = this->targetParam->GetCount();
	while (i-- > 0)
	{
		param = this->targetParam->RemoveAt(i);
		Text::StrDelNew(param->targetId);
		MemFree(param);
	}
}

void Media::Batch::BatchResizer::SetHandler(Media::Batch::BatchHandler *hdlr)
{
	this->hdlr = hdlr;
}

void Media::Batch::BatchResizer::ImageOutput(Media::ImageList *imgList, const UTF8Char *fileId, const UTF8Char *subId)
{
	OSInt i;
	OSInt j;
	OSInt k;
	TargetParam *param;
	Media::ImageList *newImgList;
	Media::StaticImage *newImg;
	Bool succ;
	UTF8Char u8buff[256];


	i = this->targetParam->GetCount();
	while (i-- > 0)
	{
		param = this->targetParam->GetItem(i);

		Sync::MutexUsage mutUsage(this->resizeMut);
		if (param->sizeType == 0)
		{
			resizer->SetTargetWidth(param->width);
			resizer->SetTargetHeight(param->height);
		}

		succ = true;
		imgList->GetSourceName(u8buff);
		NEW_CLASS(newImgList, Media::ImageList(u8buff));
		j = 0;
		k = imgList->GetCount();
		while (j < k)
		{
			newImg = (Media::StaticImage*)imgList->GetImage(j, 0);
			if (param->sizeType == 1)
			{
				resizer->SetTargetWidth(Math::Double2Int32(newImg->info->dispWidth * param->width / newImg->info->hdpi));
				resizer->SetTargetHeight(Math::Double2Int32(newImg->info->dispHeight * param->height / newImg->info->vdpi));
				newImg = resizer->ProcessToNew(newImg);
			}
			else
			{
				newImg = resizer->ProcessToNew(newImg);
			}

			if (newImg == 0)
			{
				succ = false;
				break;
			}
			else
			{
				newImgList->AddImage(newImg, 0);
			}
			j++;
		}
		mutUsage.EndUse();
		if (succ)
		{
			if (this->hdlr)
				this->hdlr->ImageOutput(newImgList, fileId, param->targetId);
		}
		else
		{
			newImg = 0;
		}
		DEL_CLASS(newImgList);
	}
}
